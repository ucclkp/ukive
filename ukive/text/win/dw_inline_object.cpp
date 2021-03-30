// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "dw_inline_object.h"

#include "utils/log.h"

#include "ukive/text/text_inline_object.h"


namespace ukive {

    DWInlineObject::DWInlineObject()
        : ref_count_(1) {}

    void DWInlineObject::setCallback(TextInlineObject* cb) {
        callback_ = cb;
    }

    STDMETHODIMP_(ULONG) DWInlineObject::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) DWInlineObject::Release() {
        auto nc = InterlockedDecrement(&ref_count_);
        if (nc == 0) {
            delete this;
        }

        return nc;
    }

    STDMETHODIMP DWInlineObject::QueryInterface(
        REFIID riid, void** ppvObject)
    {
        if (!ppvObject) {
            return E_POINTER;
        }

        if (__uuidof(DWInlineObject) == riid) {
            *ppvObject = static_cast<DWInlineObject*>(this);
        } else if (__uuidof(IDWriteInlineObject) == riid) {
            *ppvObject = static_cast<IDWriteInlineObject*>(this);
        } else if (__uuidof(IUnknown) == riid) {
            *ppvObject = static_cast<IUnknown*>(this);
        } else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    STDMETHODIMP DWInlineObject::Draw(
        void* clientDrawingContext,
        IDWriteTextRenderer* renderer,
        FLOAT originX, FLOAT originY,
        BOOL isSideways, BOOL isRightToLeft,
        IUnknown* clientDrawingEffect)
    {
        if (callback_) {
            DCHECK(clientDrawingContext);
            if (!clientDrawingContext) {
                return S_OK;
            }

            auto canvas = static_cast<Canvas*>(clientDrawingContext);
            callback_->onDrawInlineObject(canvas, originX, originY);
        }

        return S_OK;
    }

    STDMETHODIMP DWInlineObject::GetMetrics(
        DWRITE_INLINE_OBJECT_METRICS* metrics)
    {
        if (callback_) {
            callback_->onGetMetrics(
                &metrics->width, &metrics->height, &metrics->baseline);
        } else {
            metrics->width = 0;
            metrics->height = 0;
            metrics->baseline = 0;
        }
        metrics->supportsSideways = FALSE;
        return S_OK;
    }

    STDMETHODIMP DWInlineObject::GetOverhangMetrics(
        DWRITE_OVERHANG_METRICS* overhangs)
    {
        if (!overhangs) {
            return E_INVALIDARG;
        }

        overhangs->left = 0;
        overhangs->top = 0;
        overhangs->right = 0;
        overhangs->bottom = 0;
        return S_OK;
    }

    STDMETHODIMP DWInlineObject::GetBreakConditions(
        DWRITE_BREAK_CONDITION* breakConditionBefore,
        DWRITE_BREAK_CONDITION* breakConditionAfter)
    {
        if (!breakConditionBefore || !breakConditionAfter) {
            return E_INVALIDARG;
        }

        *breakConditionBefore = DWRITE_BREAK_CONDITION_NEUTRAL;
        *breakConditionAfter = DWRITE_BREAK_CONDITION_NEUTRAL;
        return S_OK;
    }

}
