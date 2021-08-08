// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/text_action_menu.h"

#include <memory>

#include "utils/message/cycler.h"

#include "ukive/menu/menu_impl.h"
#include "ukive/elements/shape_element.h"
#include "ukive/graphics/color.h"
#include "ukive/text/text_action_menu_callback.h"
#include "ukive/animation/view_animator.h"
#include "ukive/window/window.h"


namespace ukive {

    TextActionMenu::TextActionMenu(
        Window* w, TextActionMenuCallback* callback)
        : is_finished_(true),
          window_(w),
          callback_(callback)
    {
        auto c = w->getContext();
        menu_width_ = c.dp2pxi(92);
        menu_item_height_ = c.dp2pxi(36);

        menu_impl_ = new MenuImpl(c);
        menu_impl_->setCallback(this);
        menu_impl_->setMenuItemHeight(menu_item_height_);

        ShapeElement* shape_element
            = new ShapeElement(ShapeElement::ROUND_RECT);
        shape_element->setRadius(2.f);
        shape_element->setSolidEnable(true);
        shape_element->setSolidColor(Color::White);

        inner_window_ = std::make_shared<InnerWindow>();
        inner_window_->setShadowRadius(c.dp2pxi(2.f));
        inner_window_->setContentView(menu_impl_);
        inner_window_->setOutsideTouchable(true);
        inner_window_->setBackground(shape_element);
        inner_window_->setWidth(menu_width_);
    }

    TextActionMenu::~TextActionMenu() {
    }

    void TextActionMenu::onCreateMenu(Menu* menu) {
    }

    void TextActionMenu::onPrepareMenu(Menu* menu) {
    }

    bool TextActionMenu::onMenuItemClicked(Menu* menu, MenuItem* item) {
        return callback_->onActionItemClicked(this, item);
    }

    Menu* TextActionMenu::getMenu() const {
        return menu_impl_;
    }

    void TextActionMenu::invalidateMenu() {
        callback_->onPrepareActionMode(this, menu_impl_);
    }

    void TextActionMenu::invalidatePosition() {
        int x = 0, y = 0;
        callback_->onGetContentPosition(&x, &y);

        auto bounds = window_->getContentBounds();
        if (x + menu_width_ > bounds.width()) {
            x -= menu_width_;
        }

        inner_window_->update(x, y);
    }

    void TextActionMenu::show() {
        if (!is_finished_) {
            return;
        }

        is_finished_ = false;

        int x = 0, y = 0;
        callback_->onGetContentPosition(&x, &y);

        int center_x = 0, center_y = 0;
        auto bounds = window_->getContentBounds();
        if (x + menu_width_ > bounds.width()) {
            center_x = menu_width_;
            center_y = 0;
            x -= menu_width_;
        }

        inner_window_->dismiss();

        using namespace std::chrono_literals;

        inner_window_->show(window_, x, y);
        inner_window_->getDecorView()->animate()->
            setDuration(2s)->
            circleReveal(
                tval::ofReal(center_x),
                tval::ofReal(center_y),
                tval::ofReal(0),
                tval::ofAuto())->start();
    }

    void TextActionMenu::close() {
        if (is_finished_) {
            return;
        }

        is_finished_ = true;

        menu_impl_->setEnabled(false);
        callback_->onDestroyActionMode(this);
        inner_window_->markDismissing();

        std::weak_ptr<InnerWindow> ptr = inner_window_;
        auto dec_view = inner_window_->getDecorView();
        if (dec_view) {
            using namespace std::chrono_literals;
            dec_view->animate()->
                setDuration(100ms)->alpha(0.f)->setFinishedHandler(
                    [ptr](AnimationDirector* director)
            {
                auto window = ptr.lock();
                if (window) {
                    window->dismiss();
                }
            })->start();
        }
    }

}