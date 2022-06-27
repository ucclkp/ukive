// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_CYRO_RENDER_TARGET_D2D_H_
#define UKIVE_GRAPHICS_WIN_CYRO_RENDER_TARGET_D2D_H_

#include <stack>

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/canvas_stack.hpp"
#include "ukive/graphics/cyro_render_target.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/rebuildable.h"

#include <d2d1.h>


namespace ukive {
namespace win {

    class CyroRenderTargetD2D :
        public CyroRenderTarget,
        public Rebuildable
    {
    public:
        CyroRenderTargetD2D();
        ~CyroRenderTargetD2D();

        bool onCreate(CyroBuffer* buffer) override;
        void onDestroy() override;

        CyroBuffer* getBuffer() const override;

        GPtr<ImageFrame> createImage(const GPtr<LcImageFrame>& frame) override;
        GPtr<ImageFrame> createImage(
            int width, int height, const ImageOptions& options) override;
        GPtr<ImageFrame> createImage(
            int width, int height,
            const GPtr<ByteData>& pixel_data, size_t stride,
            const ImageOptions& options) override;
        GPtr<ImageFrame> createImage(
            const GPtr<GPUTexture>& tex2d, const ImageOptions& options) override;

        void setOpacity(float opacity) override;
        float getOpacity() const override;

        Size getSize() const override;
        Size getPixelSize() const override;
        Matrix2x3F getMatrix() const override;

        void onBeginDraw() override;
        GRet onEndDraw() override;
        GRet onResize(int width, int height) override;

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
            const RectF& src, const RectF& dst, float opacity, ImageFrame* img) override;

        void fillOpacityMask(
            float width, float height, ImageFrame* mask, ImageFrame* content) override;

        void drawText(
            const std::u16string_view& text,
            const std::u16string_view& font_name, float font_size,
            const RectF& rect, const Paint& paint) override;
        void drawTextLayout(
            float x, float y, TextLayout* layout, const Paint& paint) override;

        utl::win::ComPtr<ID2D1RenderTarget> getNativeRT() const { return rt_; }

        void nativeDrawGlyphRun(
            float x, float y,
            const DWRITE_GLYPH_RUN* glyph_run,
            DWRITE_MEASURING_MODE measuring_mode, const Color& color);

    protected:
        void onDemolish() override;
        void onRebuild(bool succeeded) override;

    private:
        struct StackData {
            float opacity;
            utl::win::ComPtr<ID2D1DrawingStateBlock> state_block;
        };

        bool initialize();
        void uninitialize();

        float opacity_;
        Matrix2x3F matrix_;

        utl::win::ComPtr<ID2D1RenderTarget> rt_;
        utl::win::ComPtr<ID2D1SolidColorBrush> solid_brush_;
        utl::win::ComPtr<ID2D1BitmapBrush> bitmap_brush_;

        std::mutex rt_sync_;
        CanvasStack<StackData> save_stack_;

        CyroBuffer* buffer_ = nullptr;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_CYRO_RENDER_TARGET_D2D_H_