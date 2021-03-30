// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WINDOW_MANAGER_H_
#define UKIVE_WINDOW_WINDOW_MANAGER_H_

#include <memory>
#include <vector>


namespace ukive {

    class Window;

    class WindowManageListener {
    public:
        virtual ~WindowManageListener() = default;

        virtual void onWindowAdded(Window* w) {}
        virtual void onWindowRemoved(Window* w) {}
    };

    class WindowManager {
    public:
        static WindowManager* getInstance();

        void addWindow(Window *window);
        size_t getWindowCount();
        Window* getWindow(size_t index);
        void removeWindow(Window *window);

        void addListener(WindowManageListener* l);
        void removeListener(WindowManageListener* l);

    private:
        WindowManager() {}

        static std::unique_ptr<WindowManager> instance_;

        std::vector<Window*> window_list_;
        std::vector<WindowManageListener*> listeners_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_MANAGER_H_
