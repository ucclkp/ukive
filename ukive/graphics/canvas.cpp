// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/canvas.h"

#include "ukive/graphics/images/image.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/cyro_renderer.h"
#include "ukive/graphics/paint.h"
#include "ukive/text/text_layout.h"
#include "ukive/window/window.h"


namespace ukive {

    Canvas::Canvas(size_t width, size_t height, const ImageOptions& options) {
        auto buffer = OffscreenBuffer::create();
        if (buffer->onCreate(width, height, options)) {
            renderer_.reset(CyroRenderer::create());
            renderer_->bind(buffer, true);
        } else {
            delete buffer;
        }
    }

    Canvas::Canvas(const std::shared_ptr<CyroRenderer>& renderer) {
        renderer_ = renderer;
    }

    Canvas::~Canvas() {}

    ImageFrame* Canvas::createImage(const LcImageFrame* frame) {
        if (!renderer_ || !frame) {
            return nullptr;
        }
        return renderer_->createImage(frame);
    }

    ImageFrame* Canvas::createImage(int width, int height) {
        if (!renderer_ || width <= 0 || height <= 0) {
            return {};
        }
        return renderer_->createImage(
            width, height, getBuffer()->getImageOptions());
    }

    ImageFrame* Canvas::createImage(
        int width, int height,
        const ImageOptions& options)
    {
        if (!renderer_ || width <= 0 || height <= 0) {
            return {};
        }
        return renderer_->createImage(width, height, options);
    }

    ImageFrame* Canvas::createImage(
        int width, int height,
        const uint8_t* pixel_data, size_t size, size_t stride)
    {
        if (!renderer_ ||
            width <= 0 || height <= 0 ||
            !pixel_data || !stride || !size)
        {
            return {};
        }
        return renderer_->createImage(
            width, height, pixel_data, size, stride, getBuffer()->getImageOptions());
    }

    ImageFrame* Canvas::createImage(
        int width, int height,
        const uint8_t* pixel_data, size_t size, size_t stride,
        const ImageOptions& options)
    {
        if (!renderer_ ||
            width <= 0 || height <= 0 ||
            !pixel_data || !stride || !size)
        {
            return {};
        }
        return renderer_->createImage(width, height, pixel_data, size, stride, options);
    }

    void Canvas::setOpacity(float opacity) {
        if (renderer_) {
            renderer_->setOpacity(opacity);
        }
    }

    float Canvas::getOpacity() const {
        if (!renderer_) {
            return 1.f;
        }
        return renderer_->getOpacity();
    }

    void Canvas::clear() {
        if (renderer_) {
            renderer_->clear();
        }
    }

    void Canvas::clear(const Color& color) {
        if (renderer_) {
            renderer_->clear(color);
        }
    }

    void Canvas::beginDraw() {
        if (renderer_) {
            renderer_->getBuffer()->onBeginDraw();
        }
    }

    GRet Canvas::endDraw() {
        if (!renderer_) {
            return GRet::Failed;
        }
        return renderer_->getBuffer()->onEndDraw();
    }

    void Canvas::pushClip(const RectF& rect) {
        if (renderer_) {
            renderer_->pushClip(rect);
        }
    }

    void Canvas::popClip() {
        if (renderer_) {
            renderer_->popClip();
        }
    }

    void Canvas::save() {
        if (renderer_) {
            renderer_->save();
        }
    }

    void Canvas::restore() {
        if (renderer_) {
            renderer_->restore();
        }
    }

    int Canvas::getWidth() const {
        if (!renderer_) {
            return 0;
        }
        return renderer_->getBuffer()->getSize().width;
    }

    int Canvas::getHeight() const {
        if (!renderer_) {
            return 0;
        }
        return renderer_->getBuffer()->getSize().height;
    }

    CyroRenderer* Canvas::getRenderer() const {
        return renderer_.get();
    }

    CyroBuffer* Canvas::getBuffer() const {
        if (!renderer_) {
            return nullptr;
        }

        return renderer_->getBuffer();
    }

    ImageFrame* Canvas::extractImage() const {
        if (!renderer_) {
            return nullptr;
        }

        return renderer_->getBuffer()->onExtractImage(
            renderer_->getBuffer()->getImageOptions());
    }

    ImageFrame* Canvas::extractImage(const ImageOptions& options) const {
        if (!renderer_) {
            return nullptr;
        }

        return renderer_->getBuffer()->onExtractImage(options);
    }

    void Canvas::scale(float sx, float sy) {
        scale(sx, sy, 0.f, 0.f);
    }

    void Canvas::scale(float sx, float sy, float cx, float cy) {
        if (renderer_) {
            renderer_->scale(sx, sy, PointF(cx, cy));
        }
    }

    void Canvas::rotate(float angle) {
        rotate(angle, 0.f, 0.f);
    }

    void Canvas::rotate(float angle, float cx, float cy) {
        if (renderer_) {
            renderer_->rotate(angle, PointF(cx, cy));
        }
    }

    void Canvas::translate(float dx, float dy) {
        if (renderer_) {
            renderer_->translate(dx, dy);
        }
    }

    void Canvas::concat(const Matrix2x3F& matrix) {
        if (renderer_) {
            renderer_->concat(matrix);
        }
    }

    Matrix2x3F Canvas::getMatrix() const {
        if (!renderer_) {
            return {};
        }

        return renderer_->getMatrix();
    }

    void Canvas::fillOpacityMask(
        float width, float height,
        const ImageFrame* mask, const ImageFrame* content)
    {
        if (renderer_ && mask && content) {
            renderer_->fillOpacityMask(width, height, mask, content);
        }
    }

    void Canvas::fillImageRepeat(const RectF& rect, const ImageFrame* content) {
        if (!renderer_ || !content) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::IMAGE);
        paint.setImage(content, Paint::ImageExtendMode::WRAP, Paint::ImageExtendMode::WRAP);
        renderer_->drawRect(rect, paint);
    }

    void Canvas::drawLine(
        const PointF& start, const PointF& end, const Color& color)
    {
        drawLine(start, end, 1, color);
    }

    void Canvas::drawLine(
        const PointF& start, const PointF& end, float stroke_width, const Color& color)
    {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        renderer_->drawLine(start, end, paint);
    }

    void Canvas::drawRect(const RectF& rect, const Color& color) {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setColor(color);
        renderer_->drawRect(rect, paint);
    }

    void Canvas::drawRect(const RectF& rect, float stroke_width, const Color& color) {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        renderer_->drawRect(rect, paint);
    }

    void Canvas::fillRect(const RectF& rect, const Color& color) {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        renderer_->drawRect(rect, paint);
    }

    void Canvas::fillRect(const RectF& rect, const ImageFrame* img) {
        if (!renderer_ || !img) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::IMAGE);
        paint.setImage(img);
        renderer_->drawRect(rect, paint);
    }

    void Canvas::drawRoundRect(
        const RectF& rect, float radius, const Color& color)
    {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setColor(color);
        renderer_->drawRoundRect(rect, radius, paint);
    }

    void Canvas::drawRoundRect(
        const RectF& rect, float stroke_width,
        float radius, const Color& color)
    {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        renderer_->drawRoundRect(rect, radius, paint);
    }

    void Canvas::fillRoundRect(
        const RectF& rect, float radius, const Color& color)
    {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        renderer_->drawRoundRect(rect, radius, paint);
    }

    void Canvas::drawCircle(float cx, float cy, float radius, const Color& color) {
        drawOval(cx, cy, radius, radius, color);
    }

    void Canvas::drawCircle(float cx, float cy, float radius, float stroke_width, const Color& color) {
        drawOval(cx, cy, radius, radius, stroke_width, color);
    }

    void Canvas::fillCircle(float cx, float cy, float radius, const Color& color) {
        fillOval(cx, cy, radius, radius, color);
    }

    void Canvas::fillCircle(float cx, float cy, float radius, const ImageFrame* img) {
        if (!renderer_ || !img) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::IMAGE);
        paint.setImage(img);
        renderer_->drawCircle(PointF(cx, cy), radius, paint);
    }

    void Canvas::drawOval(float cx, float cy, float rx, float ry, const Color& color) {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setColor(color);
        renderer_->drawEllipse(PointF(cx, cy), rx, ry, paint);
    }

    void Canvas::drawOval(float cx, float cy, float rx, float ry, float stroke_width, const Color& color) {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        renderer_->drawEllipse(PointF(cx, cy), rx, ry, paint);
    }

    void Canvas::fillOval(float cx, float cy, float rx, float ry, const Color& color) {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        renderer_->drawEllipse(PointF(cx, cy), rx, ry, paint);
    }

    void Canvas::drawPath(const Path* path, float stroke_width, const Color& color) {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::STROKE);
        paint.setStrokeWidth(stroke_width);
        paint.setColor(color);
        renderer_->drawPath(path, paint);
    }

    void Canvas::fillPath(const Path* path, const Color& color) {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        renderer_->drawPath(path, paint);
    }

    void Canvas::fillPath(const Path* path, const ImageFrame* img) {
        if (!renderer_ || !img) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::IMAGE);
        paint.setImage(img);
        renderer_->drawPath(path, paint);
    }

    void Canvas::drawImage(const ImageFrame* img) {
        if (!img) {
            return;
        }

        auto size = img->getSize();
        RectF src(0.f, 0.f, size.width, size.height);

        drawImage(src, src, 1.f, img);
    }

    void Canvas::drawImage(float x, float y, const ImageFrame* img) {
        if (!img) {
            return;
        }

        auto size = img->getSize();
        RectF src(0.f, 0.f, size.width, size.height);
        RectF dst(x, y, size.width, size.height);

        drawImage(src, dst, 1.f, img);
    }

    void Canvas::drawImage(float opacity, const ImageFrame* img) {
        if (!img) {
            return;
        }

        auto size = img->getSize();
        RectF src(0.f, 0.f, size.width, size.height);

        drawImage(src, src, opacity, img);
    }

    void Canvas::drawImage(const RectF& dst, float opacity, const ImageFrame* img) {
        if (!img) {
            return;
        }

        auto size = img->getSize();
        RectF src(0.f, 0.f, size.width, size.height);

        drawImage(src, dst, opacity, img);
    }

    void Canvas::drawImage(const RectF& src, const RectF& dst, float opacity, const ImageFrame* img) {
        if (renderer_ && img) {
            renderer_->drawImage(src, dst, opacity, img);
        }
    }

    void Canvas::drawText(
        const std::u16string& text,
        const std::u16string& font_name, float font_size,
        const RectF& rect, const Color& color)
    {
        if (!renderer_) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        renderer_->drawText(text, font_name, font_size, rect, paint);
    }

    void Canvas::drawTextLayout(
        float x, float y, TextLayout* layout, const Color& color)
    {
        if (!renderer_ || !layout) {
            return;
        }

        Paint paint;
        paint.setStyle(Paint::Style::FILL);
        paint.setColor(color);
        renderer_->drawTextLayout(x, y, layout, paint);
    }

}
