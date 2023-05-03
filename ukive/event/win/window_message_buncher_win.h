// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_EVENT_WIN_WINDOW_MESSAGE_BUNCHER_WIN_H_
#define UKIVE_EVENT_WIN_WINDOW_MESSAGE_BUNCHER_WIN_H_

#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>


namespace ukive {

    class WindowMessageBuncherListener {
    public:
        virtual ~WindowMessageBuncherListener() = default;

        virtual void onWMBPowerBroadcast() {}
        virtual void onWMBDisplayChanged() {}
        virtual void onWMBSettingChanged() {}
    };

    /**
     * Windows 的某些全局性质的消息会发送给每一个窗口，
     * 但我们有时只想要一个应用程序级的通知机制，即是说，
     * 不管该应用有几个窗口接到了消息，仅触发一次回调。
     * 本类就是用于将应用的所有窗口接到的指定消息收束成仅触发一次的回调。
     */
    class WindowMessageBuncherWin {
    public:
        WindowMessageBuncherWin();

        bool notifyPowerBroadcast(HWND hWnd);
        bool notifyDisplayChanged(HWND hWnd);
        bool notifySettingChanged(HWND hWnd);

        void addListener(WindowMessageBuncherListener* l);
        void removeListener(WindowMessageBuncherListener* l);

    private:
        bool canNotify(HWND hWnd) const;

        HWND cur_source_ = nullptr;
        std::vector<WindowMessageBuncherListener*> listeners_;
    };

}

#endif  // UKIVE_EVENT_WIN_WINDOW_MESSAGE_BUNCHER_WIN_H_