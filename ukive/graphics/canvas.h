// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_CANVAS_H_
#define UKIVE_GRAPHICS_CANVAS_H_

#include <memory>

#include "ukive/graphics/color.h"
#include "ukive/graphics/graphics_utils.h"
#include "ukive/graphics/matrix_2x3_f.h"


namespace ukive {

    class View;
    class Path;
    class ImageFrame;
    class LcImageFrame;
    class Window;
    class CyroBuffer;
    class TextLayout;
    class CyroRenderer;
    class OffscreenBuffer;
    class ImageOptions;

    class Canvas {
    public:
        Canvas(int width, int height, const ImageOptions& options);
        explicit Canvas(const std::shared_ptr<CyroRenderer>& renderer);
        ~Canvas();

        ImageFrame* createImage(const LcImageFrame* frame);
        ImageFrame* createImage(int width, int height);
        ImageFrame* createImage(
            int width, int height, const ImageOptions& options);
        ImageFrame* createImage(
            int width, int height,
            const uint8_t* pixel_data, size_t size, size_t stride);
        ImageFrame* createImage(
            int width, int height,
            const uint8_t* pixel_data, size_t size, size_t stride,
            const ImageOptions& options);

        void setOpacity(float opacity);
        float getOpacity() const;

        void clear();
        void clear(const Color& color);

        void beginDraw();
        GRet endDraw();

        void pushClip(const RectF& rect);
        void popClip();

        void save();
        void restore();

        int getWidth() const;
        int getHeight() const;
        CyroRenderer* getRenderer() const;
        CyroBuffer* getBuffer() const;
        ImageFrame* extractImage() const;
        ImageFrame* extractImage(const ImageOptions& options) const;

        void scale(float sx, float sy);
        void scale(float sx, float sy, float cx, float cy);
        void rotate(float angle);
        void rotate(float angle, float cx, float cy);
        void translate(float dx, float dy);
        void concat(const Matrix2x3F& matrix);

        Matrix2x3F getMatrix() const;

        void fillOpacityMask(
            float width, float height,
            const ImageFrame* mask, const ImageFrame* content);
        void fillImageRepeat(const RectF& rect, const ImageFrame* content);

        void drawLine(
            const PointF& start, const PointF& end, const Color& color);
        void drawLine(
            const PointF& start, const PointF& end, float stroke_width, const Color& color);

        void drawRect(const RectF& rect, const Color& color);
        void drawRect(const RectF& rect, float stroke_width, const Color& color);
        void fillRect(const RectF& rect, const Color& color);
        void fillRect(const RectF& rect, const ImageFrame* img);

        void drawRoundRect(
            const RectF& rect, float radius, const Color& color);
        void drawRoundRect(
            const RectF& rect, float stroke_width,
            float radius, const Color& color);
        void fillRoundRect(
            const RectF& rect, float radius, const Color& color);

        void drawCircle(const PointF& cp, float radius, const Color& color);
        void drawCircle(const PointF& cp, float radius, float stroke_width, const Color& color);
        void fillCircle(const PointF& cp, float radius, const Color& color);
        void fillCircle(const PointF& cp, float radius, const ImageFrame* img);

        void drawOval(const PointF& cp, float rx, float ry, const Color& color);
        void drawOval(const PointF& cp, float rx, float ry, float stroke_width, const Color& color);
        void fillOval(const PointF& cp, float rx, float ry, const Color& color);

        void drawPath(const Path* path, float stroke_width, const Color& color);
        void fillPath(const Path* path, const Color& color);
        void fillPath(const Path* path, const ImageFrame* img);

        void drawImage(const ImageFrame* img);
        void drawImage(float x, float y, const ImageFrame* img);
        void drawImage(float opacity, const ImageFrame* img);
        void drawImage(const RectF& dst, float opacity, const ImageFrame* img);
        void drawImage(const RectF& src, const RectF& dst, float opacity, const ImageFrame* img);

        void drawText(
            const std::u16string& text,
            const std::u16string& font_name, float font_size,
            const RectF& rect, const Color& color);
        void drawTextLayout(
            float x, float y, TextLayout* layout, const Color& color);

    private:
        std::shared_ptr<CyroRenderer> renderer_;
    };

}

#endif  // UKIVE_GRAPHICS_CANVAS_H_
