// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "dw_text_renderer.h"

#include "utils/log.h"

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/win/cyro_renderer_d2d.h"
#include "ukive/graphics/win/cyro_render_target_d2d.h"
#include "ukive/graphics/colors/color.h"
#include "ukive/text/text_custom_drawing.h"
#include "ukive/text/win/dw_text_drawing_effect.h"


namespace ukive {

    TextRenderer::TextRenderer()
        : ref_count_(1),
          def_text_color_(Color::Black),
          def_underline_color_(Color::Black),
          def_strikethrough_color_(Color::Black) {}

    void TextRenderer::setTextColor(const Color& color) {
        def_text_color_ = color;
    }

    void TextRenderer::setUnderlineColor(const Color& color) {
        def_underline_color_ = color;
    }

    void TextRenderer::setStrikethroughColor(const Color& color) {
        def_strikethrough_color_ = color;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::DrawGlyphRun                               *
    *                                                                 *
    *  Gets GlyphRun outlines via IDWriteFontFace::GetGlyphRunOutline *
    *  and then draws and fills them using Direct2D path geometries   *
    *                                                                 *
    ******************************************************************/
    STDMETHODIMP TextRenderer::DrawGlyphRun(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        DWRITE_GLYPH_RUN const* glyphRun,
        DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        IUnknown* clientDrawingEffect)
    {
        DCHECK(clientDrawingContext);
        if (!clientDrawingContext) {
            return S_OK;
        }

        auto canvas = static_cast<Canvas*>(clientDrawingContext);
        auto renderer = static_cast<CyroRendererD2D*>(canvas->getRenderer());

        if (clientDrawingEffect) {
            ComPtr<DWTextDrawingEffect> drawing_effect;
            HRESULT hr = clientDrawingEffect->QueryInterface(&drawing_effect);
            if (SUCCEEDED(hr)) {
                if (drawing_effect->custom_drawing_enabled &&
                    drawing_effect->custom_drawing_cb)
                {
                    drawing_effect->custom_drawing_cb->onTextCustomDrawing(
                        canvas, baselineOriginX, baselineOriginY);
                    return S_OK;
                }

                if (drawing_effect->text_color_enabled) {
                    renderer->nativeDrawGlyphRun(
                        baselineOriginX, baselineOriginY,
                        glyphRun, measuringMode, drawing_effect->text_color);
                    return S_OK;
                }
            }
        }

        renderer->nativeDrawGlyphRun(
            baselineOriginX, baselineOriginY,
            glyphRun, measuringMode, def_text_color_);

        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::DrawUnderline                              *
    *                                                                 *
    *  Draws underlines below the text using a Direct2D recatangle    *
    *  geometry                                                       *
    *                                                                 *
    ******************************************************************/
    STDMETHODIMP TextRenderer::DrawUnderline(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_UNDERLINE const* underline,
        IUnknown* clientDrawingEffect)
    {
        DCHECK(clientDrawingContext);
        if (!clientDrawingContext) {
            return S_OK;
        }

        auto canvas = static_cast<Canvas*>(clientDrawingContext);
        RectF rect(
            0 + baselineOriginX,
            underline->offset + baselineOriginY,
            underline->width, underline->thickness);

        if (clientDrawingEffect) {
            ComPtr<DWTextDrawingEffect> drawing_effect;
            HRESULT hr = clientDrawingEffect->QueryInterface(&drawing_effect);
            if (SUCCEEDED(hr)) {
                if (drawing_effect->alter_underline_color) {
                    canvas->fillRect(rect, drawing_effect->underline_color);
                    return S_OK;
                }
                if (drawing_effect->text_color_enabled) {
                    canvas->fillRect(rect, drawing_effect->text_color);
                    return S_OK;
                }
            }
        }

        canvas->fillRect(rect, def_text_color_);
        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::DrawStrikethrough                          *
    *                                                                 *
    *  Draws strikethroughs below the text using a Direct2D           *
    *  recatangle geometry                                            *
    *                                                                 *
    ******************************************************************/
    STDMETHODIMP TextRenderer::DrawStrikethrough(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_STRIKETHROUGH const* strikethrough,
        IUnknown* clientDrawingEffect)
    {
        DCHECK(clientDrawingContext);
        if (!clientDrawingContext) {
            return S_OK;
        }

        auto canvas = static_cast<Canvas*>(clientDrawingContext);
        RectF rect(
            0 + baselineOriginX,
            strikethrough->offset + baselineOriginY,
            strikethrough->width, strikethrough->thickness);

        if (clientDrawingEffect) {
            ComPtr<DWTextDrawingEffect> drawing_effect;
            HRESULT hr = clientDrawingEffect->QueryInterface(&drawing_effect);
            if (SUCCEEDED(hr)) {
                if (drawing_effect->alter_strikethrough_color) {
                    canvas->fillRect(rect, drawing_effect->strikethrough_color);
                    return S_OK;
                }
                if (drawing_effect->text_color_enabled) {
                    canvas->fillRect(rect, drawing_effect->text_color);
                    return S_OK;
                }
            }
        }

        canvas->fillRect(rect, def_text_color_);
        return S_OK;
    }

    STDMETHODIMP TextRenderer::DrawInlineObject(
        void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        IUnknown* clientDrawingEffect)
    {
        if (!inlineObject) {
            return E_INVALIDARG;
        }

        return inlineObject->Draw(
            clientDrawingContext, this,
            originX, originY, isSideways, isRightToLeft, clientDrawingEffect);
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::IsPixelSnappingDisabled                    *
    *                                                                 *
    *  Determines whether pixel snapping is disabled. The recommended *
    *  default is FALSE, unless doing animation that requires         *
    *  subpixel vertical placement.                                   *
    *                                                                 *
    ******************************************************************/
    STDMETHODIMP TextRenderer::IsPixelSnappingDisabled(
        void* clientDrawingContext,
        BOOL* isDisabled)
    {
        *isDisabled = FALSE;
        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::GetCurrentTransform                        *
    *                                                                 *
    *  Returns the current transform applied to the render target..   *
    *                                                                 *
    ******************************************************************/
    STDMETHODIMP TextRenderer::GetCurrentTransform(
        void* clientDrawingContext,
        DWRITE_MATRIX* transform)
    {
        DCHECK(clientDrawingContext);
        if (!clientDrawingContext) {
            auto identity = D2D1::Matrix3x2F::Identity();
            transform->m11 = identity.m11;
            transform->m12 = identity.m12;
            transform->m21 = identity.m21;
            transform->m22 = identity.m22;
            transform->dx = identity.dx;
            transform->dy = identity.dy;
            return S_OK;
        }

        auto canvas = static_cast<Canvas*>(clientDrawingContext);
        auto rt = static_cast<CyroRenderTargetD2D*>(canvas->getBuffer()->getRT())->getNative();
        rt->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::GetPixelsPerDip                            *
    *                                                                 *
    *  This returns the number of pixels per DIP.                     *
    *                                                                 *
    ******************************************************************/
    STDMETHODIMP TextRenderer::GetPixelsPerDip(
        void* clientDrawingContext,
        FLOAT* pixelsPerDip)
    {
        float x, yUnused;

        DCHECK(clientDrawingContext);
        if (!clientDrawingContext) {
            *pixelsPerDip = 1;
            return S_OK;
        }

        auto canvas = static_cast<Canvas*>(clientDrawingContext);
        auto rt = static_cast<CyroRenderTargetD2D*>(canvas->getBuffer()->getRT())->getNative();

        rt->GetDpi(&x, &yUnused);
        *pixelsPerDip = x / USER_DEFAULT_SCREEN_DPI;

        return S_OK;
    }

    STDMETHODIMP_(ULONG) TextRenderer::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) TextRenderer::Release() {
        auto rc = InterlockedDecrement(&ref_count_);
        if (rc == 0) {
            delete this;
        }

        return rc;
    }

    STDMETHODIMP TextRenderer::QueryInterface(
        REFIID riid, void** ppvObject)
    {
        if (ppvObject == nullptr) {
            return E_POINTER;
        }

        if (__uuidof(IDWriteTextRenderer) == riid) {
            *ppvObject = static_cast<IDWriteTextRenderer*>(this);
        } else if (__uuidof(IDWritePixelSnapping) == riid) {
            *ppvObject = static_cast<IDWritePixelSnapping*>(this);
        } else if (__uuidof(IUnknown) == riid) {
            *ppvObject = reinterpret_cast<IUnknown*>(this);
        } else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}