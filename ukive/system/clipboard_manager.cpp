// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "clipboard_manager.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined OS_MAC
#endif


namespace ukive {

    ClipboardManager::ClipboardManager() {
    }

    ClipboardManager::~ClipboardManager() {
    }


    void ClipboardManager::saveToClipboard(std::u16string text) {
#ifdef OS_WINDOWS
        if (::OpenClipboard(nullptr)) {
            ::EmptyClipboard();

            std::size_t space = (text.length() + 1) * sizeof(wchar_t);

            HANDLE hHandle = ::GlobalAlloc(GMEM_FIXED, space);
            wchar_t* pData = static_cast<wchar_t*>(::GlobalLock(hHandle));

            text._Copy_s(reinterpret_cast<char16_t*>(pData), text.length(), text.length());
            pData[text.length()] = L'\0';

            ::SetClipboardData(CF_UNICODETEXT, hHandle);
            ::GlobalUnlock(hHandle);
            ::CloseClipboard();
        }
#elif defined OS_MAC
#endif
    }

    std::u16string ClipboardManager::getFromClipboard() {
        std::u16string content;
#ifdef OS_WINDOWS
        if (::OpenClipboard(nullptr)) {
            HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT);
            if (hMem != nullptr) {
                wchar_t* lpStr = static_cast<wchar_t*>(::GlobalLock(hMem));
                if (lpStr != nullptr) {
                    content = std::u16string(reinterpret_cast<char16_t*>(lpStr));
                    ::GlobalUnlock(hMem);
                }
            }

            ::CloseClipboard();
        }
#elif defined OS_MAC
#endif
        return content;
    }

}
