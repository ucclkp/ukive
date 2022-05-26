// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/cyro_render_target_d2d.h"

#include "utils/log.h"
#include "utils/numbers.hpp"

#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/paint.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_texture_d3d11.h"
#include "ukive/graphics/win/images/image_frame_win.h"
#include "ukive/graphics/win/images/image_options_d2d_utils.h"
#include "ukive/graphics/win/images/lc_image_frame_win.h"
#include "ukive/graphics/win/native_rt_d2d.h"
#include "ukive/graphics/win/path_win.h"
#include "ukive/text/win/dw_text_layout.h"

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

            float wp_scale = 320.f / D2D1_SCENE_REFERRED_SDR_WHITE_LEVEL;
            color.r *= wp_scale;
            color.g *= wp_scale;
            color.b *= wp_scale;

            *c = color;
        }
    }

}

namespace ukive {
namespace win {

    CyroRenderTargetD2D::CyroRenderTargetD2D()
        : opacity_(1.f) {
    }

    CyroRenderTargetD2D::~CyroRenderTargetD2D() {
    }

    bool CyroRenderTargetD2D::onCreate(CyroBuffer* buffer) {
        if (buffer_ || !buffer) {
            return false;
        }

        buffer_ = buffer;
        rt_ = static_cast<const NativeRTD2D*>(
            buffer_->getNativeRT())->getNative();
        if (!initialize()) {
            return false;
        }

        return true;
    }

    void CyroRenderTargetD2D::onDestroy() {
        uninitialize();
        rt_.reset();

        if (buffer_) {
            buffer_->onDestroy();
            delete buffer_;
            buffer_ = nullptr;
        }
    }

    void CyroRenderTargetD2D::onDemolish() {
        uninitialize();
        rt_.reset();

        if (buffer_) {
            buffer_->onDestroy();
        }
    }

    void CyroRenderTargetD2D::onRebuild() {
        if (buffer_) {
            if (!buffer_->onRecreate()) {
                return;
            }
        }

        rt_ = static_cast<const NativeRTD2D*>(
            buffer_->getNativeRT())->getNative();

        if (!initialize()) {
            return;
        }
    }

    bool CyroRenderTargetD2D::initialize() {
        if (!rt_) {
            return false;
        }

        HRESULT hr = rt_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &solid_brush_);
        if (FAILED(hr)) {
            ubassert(false);
            return false;
        }

        hr = rt_->CreateBitmapBrush(nullptr, &bitmap_brush_);
        if (FAILED(hr)) {
            ubassert(false);
            return false;
        }

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);

        return true;
    }

    void CyroRenderTargetD2D::uninitialize() {
        solid_brush_.reset();
        bitmap_brush_.reset();
        matrix_.identity();
        save_stack_.clear();
    }

    CyroBuffer* CyroRenderTargetD2D::getBuffer() const {
        return buffer_;
    }

    GPtr<ImageFrame> CyroRenderTargetD2D::createImage(const GPtr<LcImageFrame>& frame) {
        float dpi_x, dpi_y;
        frame->getDpi(&dpi_x, &dpi_y);

        D2D1_BITMAP_PROPERTIES props =
            D2D1::BitmapProperties(D2D1::PixelFormat(), dpi_x, dpi_y);

        auto native_src = frame.cast<LcImageFrameWin>()->getNativeSrc();
        if (!native_src) {
            assert(false);
            return {};
        }

        utl::win::ComPtr<ID2D1Bitmap> d2d_bmp;
        HRESULT hr = rt_->CreateBitmapFromWicBitmap(native_src.get(), &props, &d2d_bmp);
        if (FAILED(hr)) {
            assert(false);
            return {};
        }

        return GPtr<ImageFrame>(
            new ImageFrameWin(frame->getOptions(), {}, native_src, d2d_bmp));
    }

    GPtr<ImageFrame> CyroRenderTargetD2D::createImage(
        int width, int height, const ImageOptions& options)
    {
        auto prop = mapBitmapProps(options);

        utl::win::ComPtr<ID2D1Bitmap> d2d_bmp;
        HRESULT hr = rt_->CreateBitmap(D2D1::SizeU(width, height), prop, &d2d_bmp);
        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }

        return GPtr<ImageFrame>(
            new ImageFrameWin(options, {}, {}, d2d_bmp));
    }

    GPtr<ImageFrame> CyroRenderTargetD2D::createImage(
        int width, int height,
        const GPtr<ByteData>& pixel_data, size_t stride, const ImageOptions& options)
    {
        auto prop = mapBitmapProps(options);

        utl::win::ComPtr<ID2D1Bitmap> d2d_bmp;
        HRESULT hr = rt_->CreateBitmap(
            D2D1::SizeU(width, height),
            pixel_data->getConstData(),
            utl::num_cast<UINT32>(stride), prop, &d2d_bmp);
        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }

        ImageFrameWin::ImageRawParams params;
        if (pixel_data->isRef()) {
            params.width = 0;
            params.height = 0;
            params.raw_data = {};
            params.stride = 0;
        } else {
            params.width = width;
            params.height = height;
            params.raw_data = pixel_data;
            params.stride = stride;
        }

        return GPtr<ImageFrame>(
            new ImageFrameWin(options, params, {}, d2d_bmp));
    }

    GPtr<ImageFrame> CyroRenderTargetD2D::createImage(
        const GPtr<GPUTexture>& tex2d, const ImageOptions& options)
    {
        auto& desc = tex2d->getDesc();
        if (desc.dim != GPUTexture::Dimension::_2D) {
            return {};
        }

        auto res = tex2d.cast<GPUTexture2DD3D11>()->getNative();
        if (!res) {
            return {};
        }

        utl::win::ComPtr<IDXGISurface> dxgi_surface;
        HRESULT hr = res->QueryInterface(&dxgi_surface);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to query DXGI surface: " << hr;
            return {};
        }

        D2D1_BITMAP_PROPERTIES bmp_prop = mapBitmapProps(options);

        utl::win::ComPtr<ID2D1Bitmap> d2d_bmp;
        hr = rt_->CreateSharedBitmap(
            __uuidof(IDXGISurface), dxgi_surface.get(), &bmp_prop, &d2d_bmp);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return {};
        }

        auto img_win = new ImageFrameWin(options, {}, {}, d2d_bmp);
        return GPtr<ImageFrame>(img_win);
    }

    void CyroRenderTargetD2D::setOpacity(float opacity) {
        if (opacity == opacity_) {
            return;
        }

        opacity_ = opacity;

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    float CyroRenderTargetD2D::getOpacity() const {
        return opacity_;
    }

    Size CyroRenderTargetD2D::getSize() const {
        if (!buffer_) {
            return {};
        }
        return buffer_->getSize();
    }

    Size CyroRenderTargetD2D::getPixelSize() const {
        if (!buffer_) {
            return {};
        }
        return buffer_->getPixelSize();
    }

    Matrix2x3F CyroRenderTargetD2D::getMatrix() const {
        return matrix_;
    }

    void CyroRenderTargetD2D::onBeginDraw() {
        if (!buffer_) {
            return;
        }
        buffer_->onBeginDraw();
    }

    GRet CyroRenderTargetD2D::onEndDraw() {
        if (!buffer_) {
            return GRet::Failed;
        }
        return buffer_->onEndDraw();
    }

    GRet CyroRenderTargetD2D::onResize(int width, int height) {
        if (!buffer_) {
            return GRet::Failed;
        }

        uninitialize();
        rt_.reset();

        auto ret = buffer_->onResize(width, height);
        if (ret != GRet::Succeeded) {
            return ret;
        }

        rt_ = static_cast<const NativeRTD2D*>(
            buffer_->getNativeRT())->getNative();

        if (!initialize()) {
            return GRet::Failed;
        }

        return GRet::Succeeded;
    }

    void CyroRenderTargetD2D::clear() {
        rt_->Clear();
    }

    void CyroRenderTargetD2D::clear(const Color& c) {
        D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a };
        rt_->Clear(d2d_color);
    }

    void CyroRenderTargetD2D::pushClip(const RectF& rect) {
        D2D1_RECT_F d2d_rect{
            float(rect.x()),
            float(rect.y()),
            float(rect.right()),
            float(rect.bottom())
        };
        rt_->PushAxisAlignedClip(
            d2d_rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }

    void CyroRenderTargetD2D::popClip() {
        rt_->PopAxisAlignedClip();
    }

    void CyroRenderTargetD2D::save() {
        StackData* data;
        if (save_stack_.has_space()) {
            auto& sb = save_stack_.push();
            sb.opacity = opacity_;
            data = &sb;
        } else {
            save_stack_.push({ opacity_, {} });
            data = &save_stack_.top();
        }

        if (data->state_block) {
            rt_->SaveDrawingState(data->state_block.get());
        } else {
            utl::win::ComPtr<ID2D1Factory> factory;
            rt_->GetFactory(&factory);

            HRESULT hr = factory->CreateDrawingStateBlock(&data->state_block);
            if (SUCCEEDED(hr)) {
                rt_->SaveDrawingState(data->state_block.get());
            }
        }
    }

    void CyroRenderTargetD2D::restore() {
        if (save_stack_.empty()) {
            ubassert(false);
            return;
        }

        auto& sb = save_stack_.top();
        opacity_ = sb.opacity;
        auto drawingStateBlock = sb.state_block.get();

        D2D1_DRAWING_STATE_DESCRIPTION desc;
        drawingStateBlock->GetDescription(&desc);

        matrix_.m11 = desc.transform._11;
        matrix_.m12 = desc.transform._21;
        matrix_.m13 = desc.transform._31;
        matrix_.m21 = desc.transform._12;
        matrix_.m22 = desc.transform._22;
        matrix_.m23 = desc.transform._32;

        rt_->RestoreDrawingState(drawingStateBlock);

        save_stack_.pop();

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    void CyroRenderTargetD2D::scale(float sx, float sy, const PointF& c) {
        matrix_.preScale(sx, sy, c.x(), c.y());

        rt_->SetTransform(convMatrix(matrix_));
    }

    void CyroRenderTargetD2D::rotate(float angle, const PointF& c) {
        matrix_.preRotate(angle, c.x(), c.y());

        rt_->SetTransform(convMatrix(matrix_));
    }

    void CyroRenderTargetD2D::translate(float dx, float dy) {
        matrix_.preTranslate(dx, dy);

        rt_->SetTransform(convMatrix(matrix_));
    }

    void CyroRenderTargetD2D::concat(const Matrix2x3F& matrix) {
        matrix_ = matrix_ * matrix;
        rt_->SetTransform(convMatrix(matrix_));
    }

    void CyroRenderTargetD2D::drawPoint(const PointF& p, const Paint& paint) {
        auto c(paint.getColor());
        D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a, };
        convColorSpace(&d2d_color, buffer_->getImageOptions());

        solid_brush_->SetColor(d2d_color);
        rt_->DrawLine(
            D2D1::Point2F(p.x(), p.y()),
            D2D1::Point2F(p.x(), p.y()),
            solid_brush_.get(), paint.getStrokeWidth());
    }

    void CyroRenderTargetD2D::drawLine(const PointF& start, const PointF& end, const Paint& paint) {
        auto c(paint.getColor());
        D2D1_COLOR_F d2d_color{ c.r, c.g, c.b, c.a, };
        convColorSpace(&d2d_color, buffer_->getImageOptions());

        solid_brush_->SetColor(d2d_color);
        rt_->DrawLine(
            D2D1::Point2F(start.x(), start.y()),
            D2D1::Point2F(end.x(), end.y()),
            solid_brush_.get(), paint.getStrokeWidth());
    }

    void CyroRenderTargetD2D::drawRect(const RectF& rect, const Paint& paint) {
        D2D1_RECT_F d2d_rect{
            rect.x(), rect.y(), rect.right(), rect.bottom() };

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
        {
            auto img_win = static_cast<ImageFrameWin*>(paint.getImage());
            if (!img_win->prepareForRender(rt_.get())) {
                break;
            }

            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));

            rt_->FillRectangle(d2d_rect, bitmap_brush_.get());
            break;
        }
        }
    }

    void CyroRenderTargetD2D::drawRoundRect(const RectF& rect, float radius, const Paint& paint) {
        D2D1_RECT_F d2d_rect{
            rect.x(), rect.y(), rect.right(), rect.bottom() };

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
        {
            auto img_win = static_cast<ImageFrameWin*>(paint.getImage());
            if (!img_win->prepareForRender(rt_.get())) {
                break;
            }

            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));

            rt_->FillRoundedRectangle(
                D2D1::RoundedRect(d2d_rect, radius, radius), bitmap_brush_.get());
            break;
        }
        }
    }

    void CyroRenderTargetD2D::drawCircle(const PointF& c, float radius, const Paint& paint) {
        drawEllipse(c, radius, radius, paint);
    }

    void CyroRenderTargetD2D::drawEllipse(const PointF& c, float rx, float ry, const Paint& paint) {
        switch (paint.getStyle()) {
        case Paint::Style::STROKE:
        {
            auto color(paint.getColor());
            D2D1_COLOR_F d2d_color{ color.r, color.g, color.b, color.a, };
            convColorSpace(&d2d_color, buffer_->getImageOptions());
            solid_brush_->SetColor(d2d_color);
            rt_->DrawEllipse(
                D2D1::Ellipse(D2D1::Point2F(c.x(), c.y()), rx, ry),
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
                D2D1::Ellipse(D2D1::Point2F(c.x(), c.y()), rx, ry), solid_brush_.get());
            break;
        }

        case Paint::Style::IMAGE:
        {
            auto img_win = static_cast<ImageFrameWin*>(paint.getImage());
            if (!img_win->prepareForRender(rt_.get())) {
                break;
            }

            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));

            rt_->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(c.x(), c.y()), rx, ry), bitmap_brush_.get());
            break;
        }
        }
    }

    void CyroRenderTargetD2D::drawPath(const Path* path, const Paint& paint) {
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
        {
            auto img_win = static_cast<ImageFrameWin*>(paint.getImage());
            if (!img_win->prepareForRender(rt_.get())) {
                break;
            }

            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));

            rt_->FillGeometry(geo, bitmap_brush_.get());
            break;
        }
        }
    }

    void CyroRenderTargetD2D::drawImage(
        const RectF& src, const RectF& dst, float opacity, ImageFrame* img)
    {
        if (!img) {
            return;
        }

        D2D1_RECT_F d2d_src_rect{
            src.x(), src.y(), src.right(), src.bottom() };
        D2D1_RECT_F d2d_dst_rect{
            dst.x(), dst.y(), dst.right(), dst.bottom() };

        auto img_win = static_cast<ImageFrameWin*>(img);
        if (!img_win->prepareForRender(rt_.get())) {
            return;
        }

        rt_->DrawBitmap(
            CIF_TO_D2D_BMP(img), d2d_dst_rect, opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            d2d_src_rect);
    }

    void CyroRenderTargetD2D::fillOpacityMask(
        float width, float height, ImageFrame* mask, ImageFrame* content)
    {
        auto img_win = static_cast<ImageFrameWin*>(mask);
        if (!img_win->prepareForRender(rt_.get())) {
            return;
        }

        img_win = static_cast<ImageFrameWin*>(content);
        if (!img_win->prepareForRender(rt_.get())) {
            return;
        }

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

    void CyroRenderTargetD2D::drawText(
        const std::u16string_view& text,
        const std::u16string_view& font_name, float font_size,
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

        drawTextLayout(rect.x(), rect.y(), layout.get(), paint);
    }

    void CyroRenderTargetD2D::drawTextLayout(
        float x, float y, TextLayout* layout, const Paint& paint)
    {
        if (!layout) {
            return;
        }

        auto dw_tl = static_cast<DWTextLayout*>(layout)->getNative();

        switch (paint.getStyle()) {
        case Paint::Style::IMAGE:
        {
            auto img_win = static_cast<ImageFrameWin*>(paint.getImage());
            if (!img_win->prepareForRender(rt_.get())) {
                break;
            }

            bitmap_brush_->SetBitmap(CIF_TO_D2D_BMP(paint.getImage()));
            bitmap_brush_->SetExtendModeX(convExtendMode(paint.getImageExtendModeX()));
            bitmap_brush_->SetExtendModeY(convExtendMode(paint.getImageExtendModeY()));
            rt_->DrawTextLayout(D2D1::Point2F(x, y), dw_tl, bitmap_brush_.get());
            break;
        }

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

    void CyroRenderTargetD2D::nativeDrawGlyphRun(
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
}