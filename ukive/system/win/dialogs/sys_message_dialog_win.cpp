// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/system/dialogs/sys_message_dialog.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include "utils/message/win/message_pump_ui_win.h"

#include "ukive/window/window.h"
#include "ukive/window/win/window_impl_win.h"


namespace ukive {

    // static
    int SysMessageDialog::show(
        Window* parent,
        const std::u16string_view& title,
        const std::u16string_view& text,
        unsigned int flags)
    {
        HWND hWnd = nullptr;
        if (parent) {
            auto impl = static_cast<win::WindowImplWin*>(parent->getImpl());
            if (impl) {
                hWnd = impl->getHandle();
            }
        }

        UINT type = 0;
        switch (flags & 0xFFu) {
        case SMDF_BTN_YES:
            type = MB_OK;
            break;
        case SMDF_BTN_YES_CANCEL:
            type = MB_OKCANCEL;
            break;
        case SMDF_BTN_YES_NO_CANCEL:
            type = MB_YESNOCANCEL;
            break;
        case SMDF_BTN_RETRY_CANCEL:
            type = MB_RETRYCANCEL;
            break;
        default:
            type = MB_OK;
            break;
        }

        switch (flags & 0xFFFFFF00u) {
        case SMDF_ICO_INFO:
            type |= MB_ICONINFORMATION;
            break;
        case SMDF_ICO_WARN:
            type |= MB_ICONWARNING;
            break;
        case SMDF_ICO_ERRO:
            type |= MB_ICONERROR;
            break;
        default:
            break;
        }

        auto mp_ptr = utl::MessagePump::getCurrent();
        auto pump = static_cast<utl::win::MessagePumpUIWin*>(mp_ptr.get());

        std::wstring w_text(text.begin(), text.end());
        std::wstring w_title(title.begin(), title.end());
        pump->setInDialogModalLoop(true);
        ::MessageBoxW(hWnd, w_text.c_str(), w_title.c_str(), type);
        pump->setInDialogModalLoop(false);

        return 0;
    }

}
