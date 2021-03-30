// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "dw_text_drawing_effect.h"


namespace ukive {

    DWTextDrawingEffect::DWTextDrawingEffect()
        : ref_count_(1) {
    }

    DWTextDrawingEffect::~DWTextDrawingEffect() {
    }

    STDMETHODIMP_(ULONG) DWTextDrawingEffect::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) DWTextDrawingEffect::Release() {
        auto nc = InterlockedDecrement(&ref_count_);
        if (nc == 0) {
            delete this;
        }

        return nc;
    }

    STDMETHODIMP DWTextDrawingEffect::QueryInterface(
        REFIID riid, void** ppvObject)
    {
        if (!ppvObject) {
            return E_POINTER;
        }

        if (__uuidof(DWTextDrawingEffect) == riid) {
            *ppvObject = static_cast<DWTextDrawingEffect*>(this);
        } else if (__uuidof(IUnknown) == riid) {
            *ppvObject = static_cast<IUnknown*>(this);
        } else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}