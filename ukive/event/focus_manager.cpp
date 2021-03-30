// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "focus_manager.h"

#include "utils/log.h"

#include "ukive/event/keyboard.h"
#include "ukive/views/layout/root_layout.h"
#include "ukive/window/window.h"


namespace ukive {

    // static
    std::unique_ptr<FocusManager> FocusManager::instance_;

    // static
    FocusManager* FocusManager::focusOn(Window* w) {
        if (!instance_) {
            instance_.reset(new FocusManager());
        }
        if (instance_) {
            instance_->setWindow(w);
        }

        return instance_.get();
    }

    FocusManager::FocusManager() {}

    void FocusManager::setWindow(Window* w) {
        cur_window_ = w;
    }

    bool FocusManager::onInputEvent(InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVK_DOWN:
        {
            int key = e->getKeyboardKey();
            if (key == Keyboard::KEY_TAB) {
                next();
                return true;
            }
            break;
        }
        case InputEvent::EVK_UP:
        {
            int key = e->getKeyboardKey();
            if (key == Keyboard::KEY_TAB) {
                return true;
            }
            break;
        }

        default:
            break;
        }

        return false;
    }

    bool FocusManager::next() {
        if (!cur_window_) {
            return false;
        }

        auto cur = cur_window_->getKeyboardHolder();
        if (!cur) {
            return first();
        }

        auto next = findNextFocusable(cur, true);
        if (next) {
            next->requestFocus();
            return true;
        }

        return first();
    }

    bool FocusManager::prev() {
        if (!cur_window_) {
            return false;
        }

        auto cur = cur_window_->getKeyboardHolder();
        if (!cur) {
            return last();
        }

        auto prev = findPrevFocusable(cur);
        if (prev) {
            prev->requestFocus();
            return true;
        }

        return last();
    }

    bool FocusManager::first() {
        if (!cur_window_) {
            return false;
        }

        auto root = cur_window_->getRootLayout();
        if (root->canGetFocus()) {
            root->requestFocus();
            return true;
        }

        auto next = findNextFocusable(root, true);
        if (next) {
            next->requestFocus();
            return true;
        }

        return false;
    }

    bool FocusManager::last() {
        if (!cur_window_) {
            return false;
        }

        auto root = cur_window_->getRootLayout();
        LayoutView* cur = root;
        for (;;) {
            if (cur->getChildCount() == 0) {
                break;
            }

            auto cur_last = cur->getChildAt(cur->getChildCount() - 1);
            if (!cur_last->isLayoutView()) {
                break;
            }

            cur = static_cast<LayoutView*>(cur_last);
        }

        if (cur->canGetFocus()) {
            cur->requestFocus();
            return true;
        }

        auto prev = findPrevFocusable(cur);
        if (prev) {
            prev->requestFocus();
            return true;
        }

        return false;
    }

    void FocusManager::clear() {
        if (!cur_window_) {
            return;
        }

        auto holder = cur_window_->getKeyboardHolder();
        if (!holder) {
            return;
        }

        holder->discardFocus();
    }

    View* FocusManager::findNextFocusable(View* cur, bool check_cur) {
        // 先看看 cur 的子 View 里有没有合适的
        if (check_cur) {
            auto target = findNextFocusableIn(cur, false);
            if (target) {
                return target;
            }
        }

        // 再看看后面的兄弟合不合适
        auto parent = cur->getParent();
        if (!parent || parent->getVisibility() != View::SHOW) {
            return nullptr;
        }

        bool start = false;
        for (auto child : *parent) {
            if (start) {
                auto target = findNextFocusableIn(child, true);
                if (target) {
                    return target;
                }
            }
            if (child == cur) {
                start = true;
            }
        }

        // 没合适的，跳出父 View 继续找
        return findNextFocusable(parent, false);
    }

    View* FocusManager::findNextFocusableIn(View* cur, bool check_cur) {
        if (!cur) {
            return nullptr;
        }
        // 先看看自己合不合适
        if (check_cur && cur->canGetFocus()) {
            return cur;
        }
        if (!cur->isLayoutView()) {
            return nullptr;
        }

        // 自己不合适，再遍历子 View
        auto cur_lv = static_cast<LayoutView*>(cur);
        if (cur_lv->getChildCount() == 0 ||
            cur_lv->getVisibility() != View::SHOW)
        {
            return nullptr;
        }

        for (auto child : *cur_lv) {
            auto target = findNextFocusableIn(child, true);
            if (target) {
                return target;
            }
        }
        return nullptr;
    }

    View* FocusManager::findPrevFocusable(View* cur) {
        if (!cur) {
            return nullptr;
        }

        // 从位于自己前面的兄弟里找
        auto parent = cur->getParent();
        if (!parent || parent->getVisibility() != View::SHOW) {
            return nullptr;
        }

        bool start = false;
        for (int i = parent->getChildCount() - 1; i >= 0; --i) {
            auto child = parent->getChildAt(i);
            if (start) {
                auto target = findPrevFocusableOut(child, true);
                if (target) {
                    return target;
                }
            }
            if (child == cur) {
                start = true;
            }
        }

        // 兄弟都不合适，继续往上找
        return findPrevFocusable(parent);
    }

    View* FocusManager::findPrevFocusableOut(View* cur, bool check_cur) {
        if (!cur) {
            return nullptr;
        }

        // 先从自己的子 View 里从后往前找
        if (cur->isLayoutView()) {
            auto cur_lv = static_cast<LayoutView*>(cur);
            if (cur_lv->getChildCount() != 0 &&
                cur_lv->getVisibility() == View::SHOW)
            {
                for (int i = cur_lv->getChildCount() - 1; i >= 0; --i) {
                    auto target = findPrevFocusableOut(cur_lv->getChildAt(i), true);
                    if (target) {
                        return target;
                    }
                }
            }
        }

        // 子 View 都不合适再看自己
        if (check_cur && cur->canGetFocus()) {
            return cur;
        }
        return nullptr;
    }

}
