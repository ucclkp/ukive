// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WIN_WINDOW_CLASS_MANAGER_H_
#define UKIVE_WINDOW_WIN_WINDOW_CLASS_MANAGER_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <string>
#include <vector>


namespace ukive {

    struct ClassInfo {
        int style;
        HICON icon;
        HICON icon_small;
        HCURSOR cursor;

        bool operator==(const ClassInfo& rhs) const {
            return (style == rhs.style
                && icon == rhs.icon
                && icon_small == rhs.icon_small);
        }
    };

    class WindowClassManager {
    public:
        ~WindowClassManager();

        struct RegisteredClass {
            ClassInfo info;
            ATOM atom;
        };

        static WindowClassManager* getInstance();

        ATOM retrieveWindowClass(const ClassInfo& info);

    private:
        WindowClassManager()
            : class_counter_(0) {}

        ATOM createWindowClass(const std::wstring& className, const ClassInfo& info);

        static std::unique_ptr<WindowClassManager> instance_;
        static const wchar_t* kWindowClassName;

        size_t class_counter_;
        std::vector<RegisteredClass> class_list_;
    };

}

#endif  // UKIVE_WINDOW_WIN_WINDOW_CLASS_MANAGER_H_