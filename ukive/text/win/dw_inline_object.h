// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_WIN_DW_INLINE_OBJECT_H_
#define UKIVE_TEXT_WIN_DW_INLINE_OBJECT_H_

#include <dwrite.h>


namespace ukive {

    class TextInlineObject;

namespace win {

    class DECLSPEC_UUID("89E38A14-5021-4B3E-BE77-2B9AEFF52A6D") DWInlineObject
        : public IDWriteInlineObject
    {
    public:
        DWInlineObject();
        virtual ~DWInlineObject() = default;

        void setCallback(TextInlineObject* cb);

        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid, void** ppvObject) override;

        STDMETHOD(Draw)(
            void* clientDrawingContext,
            IDWriteTextRenderer* renderer,
            FLOAT originX, FLOAT originY,
            BOOL isSideways, BOOL isRightToLeft,
            IUnknown* clientDrawingEffect) override;
        STDMETHOD(GetMetrics)(
            DWRITE_INLINE_OBJECT_METRICS* metrics) override;
        STDMETHOD(GetOverhangMetrics)(
            DWRITE_OVERHANG_METRICS* overhangs) override;
        STDMETHOD(GetBreakConditions)(
            DWRITE_BREAK_CONDITION* breakConditionBefore,
            DWRITE_BREAK_CONDITION* breakConditionAfter) override;

    private:
        ULONG ref_count_;
        TextInlineObject* callback_ = nullptr;
    };

}
}

#endif  // UKIVE_TEXT_WIN_DW_INLINE_OBJECT_H_