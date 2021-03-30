// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_WIN_DWRITE_TEXT_ANALYSIS_SINK_H_
#define UKIVE_TEXT_WIN_DWRITE_TEXT_ANALYSIS_SINK_H_

#include <dwrite.h>

#include "ukive/system/win/com_ptr.hpp"


namespace ukive {

    class TextAnalysisSink : public IDWriteTextAnalysisSink {
    public:
        TextAnalysisSink();
        virtual ~TextAnalysisSink();

        STDMETHOD(SetScriptAnalysis)(
            UINT32 textPosition,
            UINT32 textLength,
            DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis) override;
        STDMETHOD(SetLineBreakpoints)(
            UINT32 textPosition,
            UINT32 textLength,
            DWRITE_LINE_BREAKPOINT const* lineBreakpoints) override;
        STDMETHOD(SetBidiLevel)(
            UINT32 textPosition,
            UINT32 textLength,
            UINT8 explicitLevel,
            UINT8 resolvedLevel) override;
        STDMETHOD(SetNumberSubstitution)(
            UINT32 textPosition,
            UINT32 textLength,
            IDWriteNumberSubstitution* numberSubstitution) override;

        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid, void** ppvObject) override;

    private:
        ULONG ref_count_;
    };

}

#endif  // UKIVE_TEXT_WIN_DWRITE_TEXT_ANALYSIS_SINK_H_
