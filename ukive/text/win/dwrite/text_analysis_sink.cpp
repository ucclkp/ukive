// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/win/dwrite/text_analysis_sink.h"


namespace ukive {

    TextAnalysisSink::TextAnalysisSink()
        : ref_count_(1) {
    }

    TextAnalysisSink::~TextAnalysisSink() {
    }


    STDMETHODIMP TextAnalysisSink::SetScriptAnalysis(
        UINT32 textPosition,
        UINT32 textLength,
        DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis) {

        return S_OK;
    }

    STDMETHODIMP TextAnalysisSink::SetLineBreakpoints(
        UINT32 textPosition,
        UINT32 textLength,
        DWRITE_LINE_BREAKPOINT const* lineBreakpoints) {

        return S_OK;
    }

    STDMETHODIMP TextAnalysisSink::SetBidiLevel(
        UINT32 textPosition,
        UINT32 textLength,
        UINT8 explicitLevel,
        UINT8 resolvedLevel) {

        return S_OK;
    }

    STDMETHODIMP TextAnalysisSink::SetNumberSubstitution(
        UINT32 textPosition,
        UINT32 textLength,
        IDWriteNumberSubstitution* numberSubstitution) {

        return S_OK;
    }


    STDMETHODIMP_(ULONG) TextAnalysisSink::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) TextAnalysisSink::Release() {
        auto rc = InterlockedDecrement(&ref_count_);
        if (rc == 0) {
            delete this;
        }

        return rc;
    }

    STDMETHODIMP TextAnalysisSink::QueryInterface(
        REFIID riid, void** ppvObject) {

        if (ppvObject == nullptr) {
            return E_POINTER;
        }

        if (__uuidof(IDWriteTextAnalysisSink) == riid) {
            *ppvObject = static_cast<IDWriteTextAnalysisSink*>(this);
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
