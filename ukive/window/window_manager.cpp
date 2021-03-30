// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "window_manager.h"

#include "utils/log.h"

#include "ukive/window/window.h"


namespace ukive {

    std::unique_ptr<WindowManager> WindowManager::instance_;

    WindowManager* WindowManager::getInstance() {
        if (!instance_) {
            instance_.reset(new WindowManager());
        }
        return instance_.get();
    }

    void WindowManager::addWindow(Window *window) {
        for (auto w : window_list_) {
            if (w == window) {
                DCHECK(false);
                return;
            }
        }

        if (window_list_.empty() && !window->isStartupWindow()) {
            window->setStartupWindow(true);
        } else if (!window_list_.empty() && window->isStartupWindow()) {
            for (auto w : window_list_) {
                if (w->isStartupWindow()) {
                    w->setStartupWindow(false);
                }
            }
        }

        window_list_.push_back(window);

        for (auto listener : listeners_) {
            listener->onWindowAdded(window);
        }
    }

    size_t WindowManager::getWindowCount() {
        return window_list_.size();
    }

    Window* WindowManager::getWindow(size_t index) {
        if (index >= window_list_.size()) {
            LOG(Log::ERR) << "Out of bounds.";
            return nullptr;
        }
        return window_list_.at(index);
    }

    void WindowManager::removeWindow(Window *window) {
        for (auto it = window_list_.begin(); it != window_list_.end(); ++it) {
            if (*it == window) {
                window_list_.erase(it);

                for (auto listener : listeners_) {
                    listener->onWindowRemoved(window);
                }
                return;
            }
        }
    }

    void WindowManager::addListener(WindowManageListener* l) {
        for (auto listener : listeners_) {
            if (listener == l) {
                return;
            }
        }
        listeners_.push_back(l);
    }

    void WindowManager::removeListener(WindowManageListener* l) {
        for (auto it = listeners_.begin(); it != listeners_.end(); ++it) {
            if (*it == l) {
                listeners_.erase(it);
                return;
            }
        }
    }

}
