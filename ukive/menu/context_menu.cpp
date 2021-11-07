// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "context_menu.h"

#include "utils/weak_bind.hpp"

#include "ukive/animation/view_animator.h"
#include "ukive/elements/color_element.h"
#include "ukive/menu/context_menu_callback.h"
#include "ukive/menu/menu.h"
#include "ukive/menu/menu_impl.h"
#include "ukive/window/window.h"


namespace ukive {

    ContextMenu::ContextMenu(
        Window* w, ContextMenuCallback* callback)
        : is_finished_(true),
          callback_(callback),
          window_(w)
    {
        auto c = w->getContext();
        menu_width_ = c.dp2pxi(92);
        menu_item_height_ = c.dp2pxi(36);

        menu_ = new MenuImpl(c);
        menu_->setCallback(this);
        menu_->setMenuItemHeight(menu_item_height_);

        inner_window_ = std::make_shared<InnerWindow>();
        inner_window_->setShadowRadius(c.dp2pxi(1.5f));
        inner_window_->setContentView(menu_);
        inner_window_->setOutsideTouchable(false);
        inner_window_->setDismissByTouchOutside(true);
        inner_window_->setBackground(new ColorElement(Color::White));
        inner_window_->setWidth(menu_width_);
        inner_window_->setEventListener(this);
    }

    ContextMenu::~ContextMenu() {}

    void ContextMenu::onCreateMenu(Menu* menu) {}

    void ContextMenu::onPrepareMenu(Menu* menu) {}

    bool ContextMenu::onMenuItemClicked(Menu* menu, MenuItem* item) {
        return callback_->onContextMenuItemClicked(this, item);
    }

    Menu* ContextMenu::getMenu() const {
        return menu_;
    }

    void ContextMenu::invalidateMenu() {
        if (is_finished_) return;

        callback_->onPrepareContextMenu(this, menu_);
    }

    void ContextMenu::show(int x, int y) {
        if (!is_finished_) return;

        is_finished_ = false;

        inner_window_->dismiss();
        inner_window_->show(window_, x, y);
    }

    void ContextMenu::show(View* anchor, int gravity) {
        if (!is_finished_) return;

        is_finished_ = false;

        inner_window_->dismiss();
        inner_window_->show(anchor, gravity);
    }

    void ContextMenu::close() {
        if (is_finished_)
            return;

        is_finished_ = true;
        callback_->onDestroyContextMenu(this);
        inner_window_->markDismissing();

        using namespace std::chrono_literals;

        // 异步关闭 ContextMenu，以防止在输入事件处理流程中
        // 关闭菜单时出现问题。
        std::weak_ptr<InnerWindow> ptr = inner_window_;
        inner_window_->getDecorView()->animate()->
            setDuration(100ms)->alpha(0.f)->setFinishedHandler(
                [ptr](AnimationDirector* director)
        {
            auto window = ptr.lock();
            if (window) {
                window->dismiss();
            }
        })->start();
    }

    void ContextMenu::onRequestDismissByTouchOutside(InnerWindow* iw) {
        close();
    }

}