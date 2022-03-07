// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_CYRO_RENDERER_D2D_H_
#define UKIVE_GRAPHICS_WIN_CYRO_RENDERER_D2D_H_

#include <stack>

#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/cyro_renderer.h"
#include "ukive/graphics/win/directx_manager.h"
#include "ukive/graphics/matrix_2x3_f.h"


namespace ukive {
namespace win {

    class CyroRendererD2D : public CyroRenderer {
    public:
        CyroRendererD2D();
        ~CyroRendererD2D();

        bool bind(CyroBuffer* buffer, bool owned) override;
        void release() override;

        CyroBuffer* getBuffer() const override;

        GPtr<ImageFrame> createImage(const GPtr<LcImageFrame>& frame) override;
        GPtr<ImageFrame> createImage(
            int width, int height, const ImageOptions& options) override;
        GPtr<ImageFrame> createImage(
            int width, int height,
            const void* pixel_data, size_t size, size_t stride,
            const ImageOptions& options) override;

        void setOpacity(float opacity) override;
        float getOpacity() const override;

        Matrix2x3F getMatrix() const override;

        void clear() override;
        void clear(const Color& c) override;

        void pushClip(const RectF& rect) override;
        void popClip() override;

        void save() override;
        void restore() override;

        void scale(float sx, float sy, const PointF& c) override;
        void rotate(float angle, const PointF& c) override;
        void translate(float dx, float dy) override;
        void concat(const Matrix2x3F& matrix) override;

        void drawPoint(const PointF& p, const Paint& paint) override;
        void drawLine(const PointF& start, const PointF& end, const Paint& paint) override;
        void drawRect(const RectF& rect, const Paint& paint) override;
        void drawRoundRect(const RectF& rect, float radius, const Paint& paint) override;
        void drawCircle(const PointF& c, float radius, const Paint& paint) override;
        void drawEllipse(const PointF& c, float rx, float ry, const Paint& paint) override;
        void drawPath(const Path* path, const Paint& paint) override;
        void drawImage(
            const RectF& src, const RectF& dst, float opacity, const ImageFrame* img) override;

        void fillOpacityMask(
            float width, float height, const ImageFrame* mask, const ImageFrame* content) override;

        void drawText(
            const std::u16string_view& text,
            const std::u16string_view& font_name, float font_size,
            const RectF& rect, const Paint& paint) override;
        void drawTextLayout(
            float x, float y, TextLayout* layout, const Paint& paint) override;

        void nativeDrawGlyphRun(
            float x, float y,
            const DWRITE_GLYPH_RUN* glyph_run,
            DWRITE_MEASURING_MODE measuring_mode, const Color& color);

    private:
        bool initRes();

        float opacity_;
        Matrix2x3F matrix_;

        utl::win::ComPtr<ID2D1RenderTarget> rt_;
        utl::win::ComPtr<ID2D1SolidColorBrush> solid_brush_;
        utl::win::ComPtr<ID2D1BitmapBrush> bitmap_brush_;

        std::stack<float> opacity_stack_;
        std::stack<utl::win::ComPtr<ID2D1DrawingStateBlock>> drawing_state_stack_;

        bool owned_buffer_ = false;
        CyroBuffer* buffer_ = nullptr;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_CYRO_RENDERER_D2D_H_