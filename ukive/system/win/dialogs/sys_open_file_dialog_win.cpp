// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "sys_open_file_dialog_win.h"

#include <ShlObj.h>

#include "utils/memory/win/com_ptr.hpp"
#include "utils/message/win/message_pump_ui_win.h"

#include "ukive/window/win/window_impl_win.h"
#include "ukive/window/window.h"


namespace ukive {
namespace win {

    SysOpenFileDialogWin::SysOpenFileDialogWin() {}

    int SysOpenFileDialogWin::show(Window* parent, uint32_t flags) {
        utl::win::ComPtr<IFileOpenDialog> pfd;
        HRESULT hr = ::CoCreateInstance(
            CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
        if (FAILED(hr)) {
            return -1;
        }

        FILEOPENDIALOGOPTIONS options = FOS_FORCEFILESYSTEM;
        if (flags & OFD_OPEN_DIRECTORY) {
            options |= FOS_PICKFOLDERS;
        }
        if (flags & OFD_MULTI_SELECT) {
            options |= FOS_ALLOWMULTISELECT;
        }
        if (flags & OFD_SHOW_HIDDEN) {
            options |= FOS_FORCESHOWHIDDEN;
        }

        pfd->SetOptions(options);

        std::vector<COMDLG_FILTERSPEC> exts_raw;
        for (const auto& ext : exts_) {
            exts_raw.push_back({ ext.second.c_str(), ext.first.c_str() });
        }
        if (!exts_.empty()) {
            pfd->SetFileTypes(UINT(exts_raw.size()), exts_raw.data());
        }

        auto mp_ptr = utl::MessagePump::getCurrent();
        auto pump = static_cast<utl::win::MessagePumpUIWin*>(mp_ptr.get());

        auto hwnd = static_cast<WindowImplWin*>(parent->getImpl())->getHandle();
        pump->setInDialogModalLoop(true);
        hr = pfd->Show(hwnd);
        pump->setInDialogModalLoop(false);
        if (FAILED(hr)) {
            if (hr == ERROR_CANCELLED) {
                return 0;
            }
            return -1;
        }

        utl::win::ComPtr<IShellItemArray> item_array;
        hr = pfd->GetResults(&item_array);
        if (FAILED(hr)) {
            return -1;
        }

        DWORD count;
        hr = item_array->GetCount(&count);
        if (FAILED(hr)) {
            return -1;
        }

        sel_files_.clear();

        for (DWORD i = 0; i < count; ++i) {
            utl::win::ComPtr<IShellItem> item;
            if (SUCCEEDED(item_array->GetItemAt(i, &item))) {
                LPWSTR path;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
                    std::u16string file_path(path, path + std::char_traits<WCHAR>::length(path));
                    ::CoTaskMemFree(path);
                    sel_files_.push_back(std::move(file_path));
                }
            }
        }

        return 1;
    }

    void SysOpenFileDialogWin::addType(const std::u16string_view& types, const std::u16string_view& desc) {
        exts_.push_back(
            {
                std::wstring(types.begin(), types.end()),
                std::wstring(desc.begin(), desc.end())
            });
    }

    void SysOpenFileDialogWin::clearTypes() {
        exts_.clear();
    }

    const std::vector<std::u16string>& SysOpenFileDialogWin::getSelectedFiles() const {
        return sel_files_;
    }

}
}