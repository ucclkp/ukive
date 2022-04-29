// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/canvas.h"

#include "ukive/graphics/images/image.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/cyro_render_target.h"
#include "ukive/graphics/paint.h"
#include "ukive/text/text_layout.h"
#include "ukive/window/window.h"


namespace ukive {

    Canvas::Canvas(int width, int height, const ImageOptions& options) {
        auto buffer = OffscreenBuffer::create();
        if (buffer->onCreate(width, height, options)) {
            rt_ = CyroRenderTarget::create();
            rt_->onCreate(buffer);
        } else {
            delete buffer;
        }
    }

    Canvas::Canvas(CyroRenderTarget* rt) {
        rt_ = rt;
    }

    Canvas::~Canvas() {
        if (rt_) {
            rt_->onDestroy();
            delete rt_;
        }
    }

    GPtr<ImageFrame> Canvas::createImage(const GPtr<LcImageFrame>& frame) {
        if (!rt_ || !frame) {
            return {};
        }
        return rt_->createImage(frame);
    }

    GPtr<ImageFrame> Canvas::createImage(int width, int height) {
        if (!rt_ || width <= 0 || height <= 0) {
            return {};
        }
        return rt_->createImage(
            width, height, getBuffer()->getImageOptions());
    }

    GPtr<ImageFrame> Canvas::createImage(
        int width, int height,
        const ImageOptions& options)
    {
        if (!rt_ || width <= 0 || height <= 0) {
            return {};
        }
        return rt_->createImage(width, height, options);
    }

    GPtr<ImageFrame> Canvas::createImage(
        int width, int height,
        const GPtr<ByteData>& pixel_data, size_t stride)
    {
        if (!rt_ ||
            width <= 0 || height <= 0 ||
            !pixel_data || !stride)
        {
            return {};
        }
        return rt_->createImage(
            width, height, pixel_data, stride, getBuffer()->getImageOptions());
    }

    GPtr<ImageFrame> Canvas::createImage(
        int width, int height,
        const GPtr<ByteData>& pixel_data, size_t stride,
        const ImageOptions& options)
    {
        if (!rt_ ||
            width <= 0 || height <= 0 ||
            !pixel_data || !stride)
        {
            return {};
        }
        return rt_->createImage(
            width, height, pixel_data, stride, options);
    }

    GPtr<ImageFrame> Canvas::createImage(const GPtr<GPUTexture>& tex2d) {
        if (!rt_ || !tex2d) {
            return {};
        }
        return rt_->createImage(tex2d, getBuffer()->getImageOptions());
    }

    GPtr<ImageFrame> Canvas::createImage(
        const GPtr<GPUTexture>& tex2d, const ImageOptions& options)
    {
        if (!rt_ || !tex2d) {
            return {};
        }
        return rt_->createImage(tex2d, options);
    }

    void Canvas::setOpacity(float opacity) {
        if (rt_) {
            rt_->setOpacity(opacity);
        }
    }

    float Canvas::getOpacity() const {
        if (!rt_) {
            return 1.f;
        }
        return rt_->getOpacity();
    }

    void Canvas::clear() {
        if (rt_) {
            rt_->clear();
        }
    }

    void Canvas::clear(const Color& color) {
        if (rt_) {
            rt_->clear(color);
        }
    }

    bool Canvas::isValid() const {
        return !!rt_;
    }

    void Canvas::beginDraw() {
        if (rt_) {
            rt_->onBeginDraw();
        }
    }

    GRet Canvas::endDraw() {
        if (!rt_) {
            return GRet::Failed;
        }
        return rt_->onEndDraw();
    }

    void Canvas::pushClip(const RectF& rect) {
        if (rt_) {
            rt_->pushClip(rect);
        }
    }

    void Canvas::popClip() {
        if (rt_) {
            rt_->popClip();
        }
    }

    void Canvas::save() {
        if (rt_) {
            rt_->save();
        }
    }

    void Canvas::restore() {
        if (rt_) {
            rt_->restore();
        }
    }

    int Canvas::getWidth() const {
        if (!rt_) {
            return 0;
        }
        return rt_->getBuffer()->getSize().width();
    }

    int Canvas::getHeight() const {
        if (!rt_) {
            return 0;
        }
        return rt_->getBuffer()->getSize().height();
    }

    Size Canvas::getSize() const {
        if (!rt_) {
            return {};
        }
        return rt_->getBuffer()->getSize();
    }

    const ImageOptions& Canvas::getImageOptions() const {
        if (!rt_) {
            static ImageOptions stub;
            return stub;
        }
        return rt_->getBuffer()->getImageOptions();
    }

    CyroRenderTarget* Canvas::getRT() const {
        return rt_;
    }

    CyroBuffer* Canvas::getBuffer() const {
        if (!rt_) {
            return nullptr;
        }

        return rt_->getBuffer();
    }

    GPtr<ImageFrame> Canvas::extractImage() const {
        if (!rt_) {
            return {};
        }

        return rt_->getBuffer()->onExtractImage(
            rt_->getBuffer()->getImageOptions());
    }

    GPtr<ImageFrame> Canvas::extractImage(const ImageOptions& options) const {
        if (!rt_) {
            return {};
        }

        return rt_->getBuffer()->onExtractImage(options);
    }

    void Canvas::scale(float sx, float sy) {
        scale(sx, sy, 0.f, 0.f);
    }

    void Canvas::scale(float sx, float sy, float cx, float cy) {
        if (rt_) {
            rt_->scale(sx, sy, { cx, cy });
        }
    }

    void Canvas::rotate(float angle) {
        rotate(angle, 0.f, 0.f);
    }

    void Canvas::rotate(float angle, float cx, float cy) {
        if (rt_) {
            rt_->rotate(angle, { cx, cy });
        }
    }

    void Canvas::translate(float dx, float dy) {
        if (rt_) {
            rt_->translate(dx, dy);
        }
    }

    void Canvas::concat(const Matrix2x3F& matrix) {
        if (rt_) {
            rt_->concat(matrix);
        }
    }

    Matrix2x3F Canvas::getMatrix() const {
        if (!rt_) {
            return {};
        }

        return rt_->getMatrix();
    }

    void Canvas::fillOpacityMask(
        float width, float height,
        ImageFrame* mask, ImageFrame* content)
    {
        if (rt_ && mask && content) {
            rt_->fillOpacityMask(width, height, mask, content);
        }
    }

    void Canvas::fillImageRepeat(const RectF& rect, ImageFrame* content) {
        if (!rt_ || !content) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::IMAGE);
        paint.setImage(content, Paint::ImageExtendMode::WRAP, Paint::ImageExtendMode::WRAP);
        rt_->drawRect(rect, paint);
    }

    void Canvas::drawLine(
        const PointF& start, const PointF& end, const Color& color)
    {
        drawLine(start, end, 1, color);
    }

    void Canvas::drawLine(
        const PointF& start, const PointF& end, float stroke_width, const Color& color)
    {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        rt_->drawLine(start, end, paint);
    }

    void Canvas::drawRect(const RectF& rect, const Color& color) {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setColor(color);
        rt_->drawRect(rect, paint);
    }

    void Canvas::drawRect(const RectF& rect, float stroke_width, const Color& color) {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        rt_->drawRect(rect, paint);
    }

    void Canvas::fillRect(const RectF& rect, const Color& color) {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        rt_->drawRect(rect, paint);
    }

    void Canvas::fillRect(const RectF& rect, ImageFrame* img) {
        if (!rt_ || !img) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::IMAGE);
        paint.setImage(img);
        rt_->drawRect(rect, paint);
    }

    void Canvas::drawRoundRect(
        const RectF& rect, float radius, const Color& color)
    {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setColor(color);
        rt_->drawRoundRect(rect, radius, paint);
    }

    void Canvas::drawRoundRect(
        const RectF& rect, float stroke_width,
        float radius, const Color& color)
    {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        rt_->drawRoundRect(rect, radius, paint);
    }

    void Canvas::fillRoundRect(
        const RectF& rect, float radius, const Color& color)
    {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        rt_->drawRoundRect(rect, radius, paint);
    }

    void Canvas::drawCircle(const PointF& cp, float radius, const Color& color) {
        drawOval(cp, radius, radius, color);
    }

    void Canvas::drawCircle(const PointF& cp, float radius, float stroke_width, const Color& color) {
        drawOval(cp, radius, radius, stroke_width, color);
    }

    void Canvas::fillCircle(const PointF& cp, float radius, const Color& color) {
        fillOval(cp, radius, radius, color);
    }

    void Canvas::fillCircle(const PointF& cp, float radius, ImageFrame* img) {
        if (!rt_ || !img) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::IMAGE);
        paint.setImage(img);
        rt_->drawCircle(cp, radius, paint);
    }

    void Canvas::drawOval(const PointF& cp, float rx, float ry, const Color& color) {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setColor(color);
        rt_->drawEllipse(cp, rx, ry, paint);
    }

    void Canvas::drawOval(const PointF& cp, float rx, float ry, float stroke_width, const Color& color) {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        rt_->drawEllipse(cp, rx, ry, paint);
    }

    void Canvas::fillOval(const PointF& cp, float rx, float ry, const Color& color) {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        rt_->drawEllipse(cp, rx, ry, paint);
    }

    void Canvas::drawPath(const Path* path, float stroke_width, const Color& color) {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        rt_->drawPath(path, paint);
    }

    void Canvas::fillPath(const Path* path, const Color& color) {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        rt_->drawPath(path, paint);
    }

    void Canvas::fillPath(const Path* path, ImageFrame* img) {
        if (!rt_ || !img) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::IMAGE);
        paint.setImage(img);
        rt_->drawPath(path, paint);
    }

    void Canvas::drawImage(ImageFrame* img) {
        if (!img) {
            return;
        }

        auto size = img->getSize();
        RectF src(0.f, 0.f, size.width(), size.height());

        drawImage(src, src, 1.f, img);
    }

    void Canvas::drawImage(float x, float y, ImageFrame* img) {
        if (!img) {
            return;
        }

        auto size = img->getSize();
        RectF src(0.f, 0.f, size.width(), size.height());
        RectF dst(x, y, size.width(), size.height());

        drawImage(src, dst, 1.f, img);
    }

    void Canvas::drawImage(float opacity, ImageFrame* img) {
        if (!img) {
            return;
        }

        auto size = img->getSize();
        RectF src(0.f, 0.f, size.width(), size.height());

        drawImage(src, src, opacity, img);
    }

    void Canvas::drawImage(const RectF& dst, float opacity, ImageFrame* img) {
        if (!img) {
            return;
        }

        auto size = img->getSize();
        RectF src(0.f, 0.f, size.width(), size.height());

        drawImage(src, dst, opacity, img);
    }

    void Canvas::drawImage(const RectF& src, const RectF& dst, float opacity, ImageFrame* img) {
        if (rt_ && img) {
            rt_->drawImage(src, dst, opacity, img);
        }
    }

    void Canvas::drawText(
        const std::u16string& text,
        const std::u16string& font_name, float font_size,
        const RectF& rect, const Color& color)
    {
        if (!rt_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        rt_->drawText(text, font_name, font_size, rect, paint);
    }

    void Canvas::drawTextLayout(
        float x, float y, TextLayout* layout, const Color& color)
    {
        if (!rt_ || !layout) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        rt_->drawTextLayout(x, y, layout, paint);
    }

}
