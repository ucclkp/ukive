// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_CYRO_RENDERER_H_
#define UKIVE_GRAPHICS_CYRO_RENDERER_H_

#include <string>

#include "ukive/graphics/byte_data.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/graphics_utils.h"
#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"


namespace ukive {

    class Path;
    class View;
    class Color;
    class Paint;
    class ImageFrame;
    class LcImageFrame;
    class TextLayout;
    class Matrix2x3F;
    class CyroBuffer;
    class ImageOptions;

    class CyroRenderer {
    public:
        static CyroRenderer* create();

        virtual ~CyroRenderer() = default;

        virtual bool bind(CyroBuffer* buffer, bool owned) = 0;
        virtual void unbind() = 0;

        virtual CyroBuffer* getBuffer() const = 0;

        virtual GPtr<ImageFrame> createImage(const GPtr<LcImageFrame>& frame) = 0;
        virtual GPtr<ImageFrame> createImage(
            int width, int height, const ImageOptions& options) = 0;
        virtual GPtr<ImageFrame> createImage(
            int width, int height,
            const GPtr<ByteData>& pixel_data, size_t stride,
            const ImageOptions& options) = 0;

        virtual void setOpacity(float opacity) = 0;
        virtual float getOpacity() const = 0;

        virtual Matrix2x3F getMatrix() const = 0;

        virtual void onBeginDraw() = 0;
        virtual GRet onEndDraw() = 0;

        virtual void clear() = 0;
        virtual void clear(const Color& c) = 0;

        virtual void pushClip(const RectF& rect) = 0;
        virtual void popClip() = 0;

        virtual void save() = 0;
        virtual void restore() = 0;

        virtual void scale(float sx, float sy, const PointF& c) = 0;
        virtual void rotate(float angle, const PointF& c) = 0;
        virtual void translate(float dx, float dy) = 0;
        virtual void concat(const Matrix2x3F& matrix) = 0;

        virtual void drawPoint(const PointF& p, const Paint& paint) = 0;
        virtual void drawLine(
            const PointF& start, const PointF& end, const Paint& paint) = 0;
        virtual void drawRect(const RectF& rect, const Paint& paint) = 0;
        virtual void drawRoundRect(
            const RectF& rect, float radius, const Paint& paint) = 0;
        virtual void drawCircle(const PointF& c, float radius, const Paint& paint) = 0;
        virtual void drawEllipse(
            const PointF& c, float rx, float ry, const Paint& paint) = 0;
        virtual void drawPath(const Path* path, const Paint& paint) = 0;
        virtual void drawImage(
            const RectF& src, const RectF& dst, float opacity, ImageFrame* img) = 0;

        virtual void fillOpacityMask(
            float width, float height, ImageFrame* mask, ImageFrame* content) = 0;

        virtual void drawText(
            const std::u16string_view& text,
            const std::u16string_view& font_name, float font_size,
            const RectF& rect, const Paint& paint) = 0;
        virtual void drawTextLayout(
            float x, float y,
            TextLayout* layout, const Paint& paint) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_CYRO_RENDERER_H_
