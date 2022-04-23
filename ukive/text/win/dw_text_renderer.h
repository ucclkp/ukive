// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_WIN_DW_TEXT_RENDER_H_
#define UKIVE_TEXT_WIN_DW_TEXT_RENDER_H_

#include "ukive/graphics/colors/color.h"

#include <d2d1.h>
#include <dwrite.h>


namespace ukive {
namespace win {

    class TextRenderer : public IDWriteTextRenderer {
    public:
        TextRenderer();
        virtual ~TextRenderer() = default;

        void setTextColor(const Color& color);
        void setUnderlineColor(const Color& color);
        void setStrikethroughColor(const Color& color);

        STDMETHOD(IsPixelSnappingDisabled)(
            void* clientDrawingContext,
            BOOL* isDisabled) override;

        STDMETHOD(GetCurrentTransform)(
            void* clientDrawingContext,
            DWRITE_MATRIX* transform) override;

        STDMETHOD(GetPixelsPerDip)(
            void* clientDrawingContext,
            FLOAT* pixelsPerDip) override;

        STDMETHOD(DrawGlyphRun)(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_MEASURING_MODE measuringMode,
            DWRITE_GLYPH_RUN const* glyphRun,
            DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
            IUnknown* clientDrawingEffect) override;

        STDMETHOD(DrawUnderline)(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_UNDERLINE const* underline,
            IUnknown* clientDrawingEffect) override;

        STDMETHOD(DrawStrikethrough)(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_STRIKETHROUGH const* strikethrough,
            IUnknown* clientDrawingEffect) override;

        STDMETHOD(DrawInlineObject)(
            void* clientDrawingContext,
            FLOAT originX,
            FLOAT originY,
            IDWriteInlineObject* inlineObject,
            BOOL isSideways,
            BOOL isRightToLeft,
            IUnknown* clientDrawingEffect) override;

        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid, void** ppvObject) override;

    private:
        ULONG ref_count_;

        Color def_text_color_;
        Color def_underline_color_;
        Color def_strikethrough_color_;
    };

}
}

#endif  // UKIVE_TEXT_WIN_DW_TEXT_RENDER_H_