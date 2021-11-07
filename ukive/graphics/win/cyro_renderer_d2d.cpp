// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/cyro_renderer_d2d.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/text/win/dw_text_layout.h"
#include "ukive/window/window.h"
#include "ukive/graphics/paint.h"
#include "ukive/graphics/win/images/image_frame_win.h"
#include "ukive/graphics/win/images/image_options_d2d_utils.h"
#include "ukive/graphics/win/images/lc_image_frame_win.h"
#include "ukive/graphics/win/cyro_render_target_d2d.h"
#include "ukive/graphics/win/offscreen_buffer_win.h"
#include "ukive/graphics/win/path_win.h"

#include <d2d1_1.h>
#include <d2d1effects_2.h>


namespace ukive {

    D2D1_MATRIX_3X2_F convMatrix(const Matrix2x3F& m) {
        D2D1::Matrix3x2F d2d_matrix;
        d2d_matrix._11 = m.m11;
        d2d_matrix._21 = m.m12;
        d2d_matrix._31 = m.m13;
        d2d_matrix._12 = m.m21;
        d2d_matrix._22 = m.m22;
        d2d_matrix._32 = m.m23;
        return d2d_matrix;
    }

    D2D1_EXTEND_MODE convExtendMode(Paint::ImageExtendMode em) {
        switch (em) {
        case Paint::ImageExtendMode::WRAP:
            return D2D1_EXTEND_MODE_WRAP;
        case Paint::ImageExtendMode::MIRROR:
            return D2D1_EXTEND_MODE_MIRROR;
        case Paint::ImageExtendMode::CLAMP:
            return D2D1_EXTEND_MODE_CLAMP;
        }
        return D2D1_EXTEND_MODE_CLAMP;
    }

    void convColorSpace(D2D1_COLOR_F* c, const ImageOptions& options) {
        if (options.pixel_format == ImagePixelFormat::HDR) {
            auto color = D2D1ConvertColorSpace(
                D2D1_COLOR_SPACE_SRGB, D2D1_COLOR_SPACE_SCRGB, c);

            float wp_scale = 80 / D2D1_SCENE_REFERRED_SDR_WHITE_LEVEL;
            color.r *= wp_scale;
            color.g *= wp_scale;
            color.b *= wp_scale;

            *c = color;
        }
    }

}

namespace ukive {

    CyroRendererD2D::CyroRendererD2D()
        : opacity_(1.f) {
    }

    CyroRendererD2D::~CyroRendererD2D() {
        if (buffer_ && owned_buffer_) {
            buffer_->onDestroy();
            delete buffer_;
        }
    }

    bool CyroRendererD2D::initRes() {
        if (!rt_) {
            return false;
        }

        solid_brush_.reset();
        bitmap_brush_.reset();
        matrix_.identity();

        HRESULT hr = rt_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &solid_brush_);
        if (FAILED(hr)) {
            DCHECK(false);
            return false;
        }

        hr = rt_->CreateBitmapBrush(nullptr, &bitmap_brush_);
        if (FAILED(hr)) {
            DCHECK(false);
            return false;
        }

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);

        return true;
    }

    bool CyroRendererD2D::bind(CyroBuffer* buffer, bool owned) {
        if (!buffer) {
            return false;
        }
        buffer_ = buffer;
        owned_buffer_ = owned;
        rt_ = static_cast<CyroRenderTargetD2D*>(buffer_->getRT())->getNative();
        return initRes();
    }

    void CyroRendererD2D::release() {
        rt_.reset();
        matrix_.identity();
        while (!opacity_stack_.empty()) {
            opacity_stack_.pop();
        }
        while (!drawing_state_stack_.empty()) {
            drawing_state_stack_.pop();
        }

        solid_brush_.reset();
        bitmap_brush_.reset();
    }

    CyroBuffer* CyroRendererD2D::getBuffer() const {
        return buffer_;
    }

    ImageFrame* CyroRendererD2D::createImage(const LcImageFrame* frame) {
        float dpi_x, dpi_y;
        frame->getDpi(&dpi_x, &dpi_y);

        D2D1_BITMAP_PROPERTIES props =
            D2D1::BitmapProperties(D2D1::PixelFormat(), dpi_x, dpi_y);

        auto native_src = static_cast<const LcImageFrameWin*>(frame)->getNativeSrc();
        if (!native_src) {
            assert(false);
            return nullptr;
        }

        ComPtr<ID2D1Bitmap> d2d_bmp;
        HRESULT hr = rt_->CreateBitmapFromWicBitmap(native_src.get(), &props, &d2d_bmp);
        if (FAILED(hr)) {
            assert(false);
            return nullptr;
        }

        return new ImageFrameWin(d2d_bmp, rt_, native_src);
    }

    ImageFrame* CyroRendererD2D::createImage(
        int width, int height, const ImageOptions& options)
    {
        auto prop = mapBitmapProps(options);

        ComPtr<ID2D1Bitmap> d2d_bmp;
        HRESULT hr = rt_->CreateBitmap(D2D1::SizeU(width, height), prop, &d2d_bmp);
        if (FAILED(hr)) {
            DCHECK(false);
            return nullptr;
        }

        return new ImageFrameWin(d2d_bmp, rt_, {});
    }

    ImageFrame* CyroRendererD2D::createImage(
        int width, int height,
        const uint8_t* pixel_data, size_t size, size_t stride, const ImageOptions& options)
    {
        DCHECK(stride <= std::numeric_limits<UINT32>::max());

        auto prop = mapBitmapProps(options);

        ComPtr<ID2D1Bitmap> d2d_bmp;
        HRESULT hr = rt_->CreateBitmap(D2D1::SizeU(width, height), pixel_data, UINT32(stride), prop, &d2d_bmp);
        if (FAILED(hr)) {
            DCHECK(false);
            return nullptr;
        }

        return new ImageFrameWin(d2d_bmp, rt_, {});
    }

    void CyroRendererD2D::setOpacity(float opacity) {
        if (opacity == opacity_) {
            return;
        }

        opacity_ = opacity;

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    float CyroRendererD2D::getOpacity() const {
        return opacity_;
    }

    Matrix2x3F CyroRendererD2D::getMatrix() const {
        return matrix_;
    }

    void CyroRendererD2D::clear() {
        rt_->Clear();
    }

    void CyroRendererD2D::clear(const Color& c) {
        D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a };
        rt_->Clear(d2d_color);
    }

    void CyroRendererD2D::pushClip(const RectF& rect) {
        D2D1_RECT_F d2d_rect{ float(rect.left), float(rect.top), float(rect.right), float(rect.bottom) };
        rt_->PushAxisAlignedClip(
            d2d_rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }

    void CyroRendererD2D::popClip() {
        rt_->PopAxisAlignedClip();
    }

    void CyroRendererD2D::save() {
        ComPtr<ID2D1Factory> factory;
        rt_->GetFactory(&factory);

        ComPtr<ID2D1DrawingStateBlock> drawingStateBlock;
        factory->CreateDrawingStateBlock(&drawingStateBlock);
        rt_->SaveDrawingState(drawingStateBlock.get());

        opacity_stack_.push(opacity_);
        drawing_state_stack_.push(drawingStateBlock);
    }

    void CyroRendererD2D::restore() {
        if (drawing_state_stack_.empty()) {
            return;
        }

        opacity_ = opacity_stack_.top();

        auto drawingStateBlock = drawing_state_stack_.top().get();

        D2D1_DRAWING_STATE_DESCRIPTION desc;
        drawingStateBlock->GetDescription(&desc);

        matrix_.m11 = desc.transform._11;
        matrix_.m12 = desc.transform._21;
        matrix_.m13 = desc.transform._31;
        matrix_.m21 = desc.transform._12;
        matrix_.m22 = desc.transform._22;
        matrix_.m23 = desc.transform._32;

        rt_->RestoreDrawingState(drawingStateBlock);

        opacity_stack_.pop();
        drawing_state_stack_.pop();

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    void CyroRendererD2D::scale(float sx, float sy, const PointF& c) {
        matrix_.preScale(sx, sy, c.x, c.y);

        rt_->SetTransform(convMatrix(matrix_));
    }

    void CyroRendererD2D::rotate(float angle, const PointF& c) {
        matrix_.preRotate(angle, c.x, c.y);

        rt_->SetTransform(convMatrix(matrix_));
    }

    void CyroRendererD2D::translate(float dx, float dy) {
        matrix_.preTranslate(dx, dy);

        rt_->SetTransform(convMatrix(matrix_));
    }

    void CyroRendererD2D::concat(const Matrix2x3F& matrix) {
        matrix_ = matrix_ * matrix;
        rt_->SetTransform(convMatrix(matrix_));
    }

    void CyroRendererD2D::drawPoint(const PointF& p, const Paint& paint) {
        auto c(paint.getColor());
        D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a, };
        convColorSpace(&d2d_color, buffer_->getImageOptions());

        solid_brush_->SetColor(d2d_color);
        rt_->DrawLine(
            D2D1::Point2F(p.x, p.y),
            D2D1::Point2F(p.x, p.y),
            solid_brush_.get(), paint.getStrokeWidth());
    }

    void CyroRendererD2D::drawLine(const PointF& start, const PointF& end, const Paint& paint) {
        auto c(paint.getColor());
        D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a, };
        convColorSpace(&d2d_color, buffer_->getImageOptions());

        solid_brush_->SetColor(d2d_color);
        rt_->DrawLine(
            D2D1::Point2F(start.x, start.y),
            D2D1::Point2F(end.x, end.y),
            solid_brush_.get(), paint.getStrokeWidth());
    }

    void CyroRendererD2D::drawRect(const RectF& rect, const Paint& paint) {
        D2D1_RECT_F d2d_rect{
            rect.left, rect.top, rect.right, rect.bottom };

        switch (paint.getStyle()) {
        case Paint::Style::STROKE:
        {
            auto c(paint.getColor());
            D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->DrawRectangle(d2d_rect, solid_brush_.get(), paint.getStrokeWidth());
            break;
        }

        case Paint::Style::FILL:
        {
            auto c(paint.getColor());
            D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->FillRectangle(d2d_rect, solid_brush_.get());
            break;
        }

        case Paint::Style::IMAGE:
            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));

            rt_->FillRectangle(d2d_rect, bitmap_brush_.get());
            break;
        }
    }

    void CyroRendererD2D::drawRoundRect(const RectF& rect, float radius, const Paint& paint) {
        D2D1_RECT_F d2d_rect{
            rect.left, rect.top, rect.right, rect.bottom };

        switch (paint.getStyle()) {
        case Paint::Style::STROKE:
        {
            auto c(paint.getColor());
            D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->DrawRoundedRectangle(
                D2D1::RoundedRect(d2d_rect, radius, radius), solid_brush_.get(), paint.getStrokeWidth());
            break;
        }

        case Paint::Style::FILL:
        {
            auto c(paint.getColor());
            D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->FillRoundedRectangle(
                D2D1::RoundedRect(d2d_rect, radius, radius), solid_brush_.get());
            break;
        }

        case Paint::Style::IMAGE:
            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));

            rt_->FillRoundedRectangle(
                D2D1::RoundedRect(d2d_rect, radius, radius), bitmap_brush_.get());
            break;
        }
    }

    void CyroRendererD2D::drawCircle(const PointF& c, float radius, const Paint& paint) {
        drawEllipse(c, radius, radius, paint);
    }

    void CyroRendererD2D::drawEllipse(const PointF& c, float rx, float ry, const Paint& paint) {
        switch (paint.getStyle()) {
        case Paint::Style::STROKE:
        {
            auto color(paint.getColor());
            D2D1_COLOR_F d2d_color{ color.r, color.g, color.b, color.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->DrawEllipse(
                D2D1::Ellipse(D2D1::Point2F(c.x, c.y), rx, ry),
                solid_brush_.get(), paint.getStrokeWidth());
            break;
        }

        case Paint::Style::FILL:
        {
            auto color(paint.getColor());
            D2D1_COLOR_F d2d_color{ color.r, color.g, color.b, color.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(c.x, c.y), rx, ry), solid_brush_.get());
            break;
        }

        case Paint::Style::IMAGE:
            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));

            rt_->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(c.x, c.y), rx, ry), bitmap_brush_.get());
            break;
        }
    }

    void CyroRendererD2D::drawPath(const Path* path, const Paint& paint) {
        auto geo = static_cast<const PathWin*>(path)->getNative();

        switch (paint.getStyle()) {
        case Paint::Style::STROKE:
        {
            auto color(paint.getColor());
            D2D1_COLOR_F d2d_color{ color.r, color.g, color.b, color.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->DrawGeometry(
                geo, solid_brush_.get(), paint.getStrokeWidth());
            break;
        }

        case Paint::Style::FILL:
        {
            auto color(paint.getColor());
            D2D1_COLOR_F d2d_color{ color.r, color.g, color.b, color.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->FillGeometry(geo, solid_brush_.get());
            break;
        }

        case Paint::Style::IMAGE:
            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));

            rt_->FillGeometry(geo, bitmap_brush_.get());
            break;
        }
    }

    void CyroRendererD2D::drawImage(
        const RectF& src, const RectF& dst, float opacity, const ImageFrame* img)
    {
        if (!img) {
            return;
        }

        D2D1_RECT_F d2d_src_rect{
            src.left, src.top, src.right, src.bottom };
        D2D1_RECT_F d2d_dst_rect{
            dst.left, dst.top, dst.right, dst.bottom };

        rt_->DrawBitmap(
            CIF_TO_D2D_BMP(img), d2d_dst_rect, opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            d2d_src_rect);
    }

    void CyroRendererD2D::fillOpacityMask(
        float width, float height, const ImageFrame* mask, const ImageFrame* content)
    {
        bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(content));
        bitmap_brush_->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
        bitmap_brush_->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);

        D2D1_RECT_F rect = D2D1::RectF(0, 0, width, height);

        auto mode = rt_->GetAntialiasMode();
        rt_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        rt_->FillOpacityMask(
            CIF_TO_D2D_BMP(mask),
            bitmap_brush_.get(), D2D1_OPACITY_MASK_CONTENT_GRAPHICS, rect, rect);
        rt_->SetAntialiasMode(mode);
    }

    void CyroRendererD2D::drawText(
        const std::u16string& text,
        const std::u16string& font_name, float font_size,
        const RectF& rect, const Paint& paint)
    {
        std::unique_ptr<TextLayout> layout(TextLayout::create());
        if (!layout->make(
            text, font_name, font_size,
            TextLayout::FontStyle::NORMAL, TextLayout::FontWeight::NORMAL, u"zh-CN"))
        {
            return;
        }
        layout->setMaxWidth(rect.width());
        layout->setMaxHeight(rect.height());

        drawTextLayout(rect.left, rect.top, layout.get(), paint);
    }

    void CyroRendererD2D::drawTextLayout(
        float x, float y, TextLayout* layout, const Paint& paint)
    {
        if (!layout) {
            return;
        }

        auto dw_tl = static_cast<DWTextLayout*>(layout)->getNative();

        switch (paint.getStyle()) {
        case Paint::Style::IMAGE:
            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));
            rt_->DrawTextLayout(D2D1::Point2F(x, y), dw_tl, bitmap_brush_.get());
            break;

        default:
        {
            auto c(paint.getColor());
            D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->DrawTextLayout(D2D1::Point2F(x, y), dw_tl, solid_brush_.get());
            break;
        }
        }
    }

    void CyroRendererD2D::nativeDrawGlyphRun(
        float x, float y,
        const DWRITE_GLYPH_RUN* glyph_run,
        DWRITE_MEASURING_MODE measuring_mode, const Color& color)
    {
        D2D1_COLOR_F d2d_color{ color.r, color.g, color.b, color.a, };
        convColorSpace(&d2d_color, buffer_->getImageOptions());
        solid_brush_->SetColor(d2d_color);
        rt_->DrawGlyphRun(D2D1::Point2F(x, y), glyph_run, solid_brush_.get(), measuring_mode);
    }

}
