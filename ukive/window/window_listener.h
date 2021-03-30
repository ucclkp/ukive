// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WINDOW_LISTENER_H_
#define UKIVE_WINDOW_WINDOW_LISTENER_H_

#include <string>


namespace ukive {

    enum class WindowButton {
        Min,
        Max,
        Close,
    };

    class OnWindowStatusChangedListener {
    public:
        virtual ~OnWindowStatusChangedListener() = default;

        virtual void onWindowTextChanged(const std::u16string& text) {}
        virtual void onWindowIconChanged() {}
        virtual void onWindowStatusChanged() {}
        virtual void onWindowButtonChanged(WindowButton button) {}
    };

}

#endif  // UKIVE_WINDOW_WINDOW_LISTENER_H_