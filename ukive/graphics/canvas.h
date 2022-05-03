// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_CANVAS_H_
#define UKIVE_GRAPHICS_CANVAS_H_

#include "ukive/graphics/byte_data.h"
#include "ukive/graphics/colors/color.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/graphics_utils.h"
#include "ukive/graphics/matrix_2x3.hpp"


namespace ukive {

    class View;
    class Path;
    class ImageFrame;
    class LcImageFrame;
    class Window;
    class CyroBuffer;
    class TextLayout;
    class CyroRenderTarget;
    class OffscreenBuffer;
    class ImageOptions;
    class GPUTexture;

    class Canvas {
    public:
        Canvas(int width, int height, const ImageOptions& options);
        explicit Canvas(CyroRenderTarget* rt);
        ~Canvas();

        GPtr<ImageFrame> createImage(const GPtr<LcImageFrame>& frame);
        GPtr<ImageFrame> createImage(int width, int height);
        GPtr<ImageFrame> createImage(
            int width, int height, const ImageOptions& options);
        GPtr<ImageFrame> createImage(
            int width, int height,
            const GPtr<ByteData>& pixel_data, size_t stride);
        GPtr<ImageFrame> createImage(
            int width, int height,
            const GPtr<ByteData>& pixel_data, size_t stride,
            const ImageOptions& options);
        GPtr<ImageFrame> createImage(const GPtr<GPUTexture>& tex2d);
        GPtr<ImageFrame> createImage(
            const GPtr<GPUTexture>& tex2d, const ImageOptions& options);

        void setOpacity(float opacity);
        float getOpacity() const;

        void clear();
        void clear(const Color& color);

        bool isValid() const;

        void beginDraw();
        GRet endDraw();

        void pushClip(const RectF& rect);
        void popClip();

        void save();
        void restore();

        int getWidth() const;
        int getHeight() const;
        Size getSize() const;
        const ImageOptions& getImageOptions() const;
        CyroRenderTarget* getRT() const;
        CyroBuffer* getBuffer() const;
        GPtr<ImageFrame> extractImage() const;
        GPtr<ImageFrame> extractImage(const ImageOptions& options) const;

        void scale(float sx, float sy);
        void scale(float sx, float sy, float cx, float cy);
        void rotate(float angle);
        void rotate(float angle, float cx, float cy);
        void translate(float dx, float dy);
        void concat(const Matrix2x3F& matrix);

        Matrix2x3F getMatrix() const;

        void fillOpacityMask(
            float width, float height,
            ImageFrame* mask, ImageFrame* content);
        void fillImageRepeat(const RectF& rect, ImageFrame* content);

        void drawLine(
            const PointF& start, const PointF& end, const Color& color);
        void drawLine(
            const PointF& start, const PointF& end, float stroke_width, const Color& color);

        void drawRect(const RectF& rect, const Color& color);
        void drawRect(const RectF& rect, float stroke_width, const Color& color);
        void fillRect(const RectF& rect, const Color& color);
        void fillRect(const RectF& rect, ImageFrame* img);

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
        void fillCircle(const PointF& cp, float radius, ImageFrame* img);

        void drawOval(const PointF& cp, float rx, float ry, const Color& color);
        void drawOval(const PointF& cp, float rx, float ry, float stroke_width, const Color& color);
        void fillOval(const PointF& cp, float rx, float ry, const Color& color);

        void drawPath(const Path* path, float stroke_width, const Color& color);
        void fillPath(const Path* path, const Color& color);
        void fillPath(const Path* path, ImageFrame* img);

        void drawImage(ImageFrame* img);
        void drawImage(float x, float y, ImageFrame* img);
        void drawImage(float opacity, ImageFrame* img);
        void drawImage(const RectF& dst, float opacity, ImageFrame* img);
        void drawImage(const RectF& src, const RectF& dst, float opacity, ImageFrame* img);

        void drawText(
            const std::u16string& text,
            const std::u16string& font_name, float font_size,
            const RectF& rect, const Color& color);
        void drawTextLayout(
            float x, float y, TextLayout* layout, const Color& color);

    private:
        CyroRenderTarget* rt_ = nullptr;
    };

}

#endif  // UKIVE_GRAPHICS_CANVAS_H_
