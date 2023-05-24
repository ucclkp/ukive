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

    bool ClipboardManager::saveToClipboard(const std::u16string_view& text) {
#ifdef OS_WINDOWS
        bool succeeded = false;
        if (::OpenClipboard(nullptr)) {
            ::EmptyClipboard();

            size_t space = (text.length() + 1) * sizeof(wchar_t);
            HANDLE handle = ::GlobalAlloc(GMEM_FIXED, space);
            if (handle) {
                wchar_t* pData = static_cast<wchar_t*>(::GlobalLock(handle));
                if (pData) {
                    std::copy(text.begin(), text.end(), pData);
                    pData[text.length()] = L'\0';
                    ::GlobalUnlock(handle);

                    if (::SetClipboardData(CF_UNICODETEXT, handle)) {
                        succeeded = true;
                    }
                }

                if (!succeeded) {
                    ::GlobalFree(handle);
                }
            }
            ::CloseClipboard();
        }
        return succeeded;
#elif defined OS_MAC
        return false;
#endif
    }

    bool ClipboardManager::getFromClipboard(std::u16string* out) {
#ifdef OS_WINDOWS
        bool succeeded = false;
        if (::OpenClipboard(nullptr)) {
            HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT);
            if (hMem) {
                wchar_t* lpStr = static_cast<wchar_t*>(::GlobalLock(hMem));
                if (lpStr) {
                    *out = std::u16string(lpStr, lpStr + std::char_traits<wchar_t>::length(lpStr));
                    ::GlobalUnlock(hMem);
                    succeeded = true;
                }
            }

            ::CloseClipboard();
        }
        return succeeded;
#elif defined OS_MAC
        return false;
#endif
    }

}
