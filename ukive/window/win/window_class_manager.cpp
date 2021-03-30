// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/window/win/window_class_manager.h"

#include "utils/log.h"
#include "utils/convert.h"

#include "ukive/app/application.h"
#include "ukive/window/win/window_impl_win.h"


namespace ukive {

    std::unique_ptr<WindowClassManager> WindowClassManager::instance_;
    const wchar_t* WindowClassManager::kWindowClassName = L"Ukive_WindowClass_";

    WindowClassManager* WindowClassManager::getInstance() {
        if (instance_ == nullptr) {
            instance_.reset(new WindowClassManager());
        }

        return instance_.get();
    }

    WindowClassManager::~WindowClassManager() {
        for (auto it = class_list_.begin(); it != class_list_.end(); ++it) {
            wchar_t *class_name = reinterpret_cast<wchar_t*>(it->atom);
            if (::UnregisterClass(class_name, ::GetModuleHandle(nullptr)) == 0) {
                LOG(Log::ERR) << "Failed to unregister class: " << class_name;
            }
        }
    }

    ATOM WindowClassManager::retrieveWindowClass(const ClassInfo& info) {
        for (auto it = class_list_.begin(); it != class_list_.end(); ++it) {
            if (it->info == info) {
                return it->atom;
            }
        }

        std::wstring class_name(kWindowClassName);
        class_name.append(std::to_wstring(class_counter_));

        ATOM atom = createWindowClass(class_name, info);
        if (atom != 0) {
            RegisteredClass reg;
            reg.info = info;
            reg.atom = atom;
            class_list_.push_back(reg);

            ++class_counter_;
        } else {
            LOG(Log::ERR) << "Cannot create new window class: "
                << utl::WideToUTF8(class_name);
        }

        return atom;
    }

    ATOM WindowClassManager::createWindowClass(const std::wstring& className, const ClassInfo& info) {
        WNDCLASSEX wcex;
        wcex.style = info.style;
        wcex.lpfnWndProc = WindowImplWin::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = ::GetModuleHandle(nullptr);
        wcex.hIcon = info.icon;
        wcex.hIconSm = info.icon_small;
        wcex.hCursor = info.cursor;
        wcex.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = className.c_str();
        wcex.cbSize = sizeof(WNDCLASSEXW);

        return ::RegisterClassEx(&wcex);
    }

}
