// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_WIN_DWRITE_TEXT_ANALYSIS_SOURCE_H_
#define UKIVE_TEXT_WIN_DWRITE_TEXT_ANALYSIS_SOURCE_H_

#include <dwrite.h>

#include "ukive/system/win/com_ptr.hpp"


namespace ukive {

    class TextAnalysisSource : public IDWriteTextAnalysisSource {
    public:
        TextAnalysisSource();
        virtual ~TextAnalysisSource();

        STDMETHOD(GetTextAtPosition)(
            UINT32 textPosition,
            WCHAR const** textString,
            UINT32* textLength) override;
        STDMETHOD(GetTextBeforePosition)(
            UINT32 textPosition,
            WCHAR const** textString,
            UINT32* textLength) override;
        STDMETHOD_(DWRITE_READING_DIRECTION, GetParagraphReadingDirection)() override;
        STDMETHOD(GetLocaleName)(
            UINT32 textPosition,
            UINT32* textLength,
            WCHAR const** localeName) override;
        STDMETHOD(GetNumberSubstitution)(
            UINT32 textPosition,
            UINT32* textLength,
            IDWriteNumberSubstitution** numberSubstitution) override;

        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid, void** ppvObject) override;

    private:
        ULONG ref_count_;
    };

}

#endif  // UKIVE_TEXT_WIN_DWRITE_TEXT_ANALYSIS_SOURCE_H_
