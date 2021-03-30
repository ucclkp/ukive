// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/win/dwrite/text_analysis_source.h"


namespace ukive {

    TextAnalysisSource::TextAnalysisSource()
        : ref_count_(1) {
    }

    TextAnalysisSource::~TextAnalysisSource() {
    }


    STDMETHODIMP TextAnalysisSource::GetTextAtPosition(
        UINT32 textPosition,
        WCHAR const** textString,
        UINT32* textLength) {

        return S_OK;
    }

    STDMETHODIMP TextAnalysisSource::GetTextBeforePosition(
        UINT32 textPosition,
        WCHAR const** textString,
        UINT32* textLength) {

        return S_OK;
    }

    STDMETHODIMP_(DWRITE_READING_DIRECTION) TextAnalysisSource::GetParagraphReadingDirection() {
        return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
    }

    STDMETHODIMP TextAnalysisSource::GetLocaleName(
        UINT32 textPosition,
        UINT32* textLength,
        WCHAR const** localeName) {

        return S_OK;
    }

    STDMETHODIMP TextAnalysisSource::GetNumberSubstitution(
        UINT32 textPosition,
        UINT32* textLength,
        IDWriteNumberSubstitution** numberSubstitution) {

        return S_OK;
    }


    STDMETHODIMP_(ULONG) TextAnalysisSource::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) TextAnalysisSource::Release() {
        auto rc = InterlockedDecrement(&ref_count_);
        if (rc == 0) {
            delete this;
        }

        return rc;
    }

    STDMETHODIMP TextAnalysisSource::QueryInterface(
        REFIID riid, void** ppvObject) {

        if (ppvObject == nullptr) {
            return E_POINTER;
        }

        if (__uuidof(IDWriteTextAnalysisSource) == riid) {
            *ppvObject = static_cast<IDWriteTextAnalysisSource*>(this);
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
