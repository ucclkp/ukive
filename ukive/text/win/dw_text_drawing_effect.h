// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_WIN_DW_TEXT_DRAWING_EFFECT_H_
#define UKIVE_TEXT_WIN_DW_TEXT_DRAWING_EFFECT_H_

#include <Unknwn.h>

#include "ukive/graphics/colors/color.h"


namespace ukive {

    class TextCustomDrawing;

    class DECLSPEC_UUID("06C2F086-4818-4DF9-880D-FF7C9F796AFA") DWTextDrawingEffect
        : public IUnknown
    {
    public:
        DWTextDrawingEffect();
        virtual ~DWTextDrawingEffect();

        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid, void** ppvObject) override;

        bool text_color_enabled = false;
        bool alter_underline_color = false;
        bool alter_strikethrough_color = false;
        bool custom_drawing_enabled = false;

        Color text_color;
        Color underline_color;
        Color strikethrough_color;
        TextCustomDrawing* custom_drawing_cb = nullptr;

    private:
        ULONG ref_count_;
    };

}

#endif  // UKIVE_TEXT_WIN_DW_TEXT_DRAWING_EFFECT_H_