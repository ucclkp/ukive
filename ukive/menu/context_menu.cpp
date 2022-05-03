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

        levitator_ = std::make_shared<Levitator>();
        levitator_->setShadowRadius(c.dp2pxi(1.5f));
        levitator_->setContentView(menu_);
        levitator_->setOutsideTouchable(false);
        levitator_->setDismissByTouchOutside(true);
        levitator_->setBackground(new ColorElement(Color::White));
        levitator_->setLayoutWidth(menu_width_);
        levitator_->setEventListener(this);
        levitator_->setLayoutMargin(
            { c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8) });
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

        levitator_->dismiss();
        levitator_->show(window_, x, y);
    }

    void ContextMenu::show(View* anchor, int gravity) {
        if (!is_finished_) return;

        is_finished_ = false;

        levitator_->dismiss();
        levitator_->show(anchor, gravity);
    }

    void ContextMenu::close() {
        if (is_finished_)
            return;

        is_finished_ = true;
        callback_->onDestroyContextMenu(this);
        levitator_->dismissing();

        using namespace std::chrono_literals;

        // 异步关闭 ContextMenu，以防止在输入事件处理流程中
        // 关闭菜单时出现问题。
        std::weak_ptr<Levitator> ptr = levitator_;
        levitator_->getFrameView()->animate()
            .alpha(0.f, 100ms).setFinishedHandler(
                [ptr](AnimationDirector* director)
        {
            auto window = ptr.lock();
            if (window) {
                window->dismiss();
            }
        }).start();
    }

    void ContextMenu::onRequestDismissByTouchOutside(Levitator* lev) {
        close();
    }

}