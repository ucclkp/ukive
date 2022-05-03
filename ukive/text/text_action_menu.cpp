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

        levitator_ = std::make_shared<Levitator>();
        levitator_->setLayoutSize(View::LS_FREE, View::LS_FREE);
        levitator_->setShadowRadius(c.dp2pxi(2.f));
        levitator_->setContentView(menu_impl_);
        levitator_->setOutsideTouchable(true);
        levitator_->setBackground(shape_element);
        levitator_->setLayoutWidth(menu_width_);
        levitator_->setLayoutMargin(
            { c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8) });
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
        View* rel;
        int x = 0, y = 0;
        callback_->onGetContentPosition(&rel, &x, &y);
        levitator_->update(x, y);
    }

    void TextActionMenu::show() {
        if (!is_finished_) {
            return;
        }

        is_finished_ = false;

        View* rel;
        int x = 0, y = 0;
        callback_->onGetContentPosition(&rel, &x, &y);

        levitator_->dismiss();

        using namespace std::chrono_literals;

        Levitator::PosInfo info;
        info.is_evaded = true;
        info.pp = Padding(1, 1, 1, 1);
        info.rel_view = rel;

        levitator_->show(window_, x, y, info);
        levitator_->getFrameView()->animate()
            .circleReveal(
                tval::ofAuto(),
                tval::ofAuto(),
                tval::ofReal(0),
                tval::ofAuto(), 100ms).start();
    }

    void TextActionMenu::close() {
        if (is_finished_) {
            return;
        }

        is_finished_ = true;

        menu_impl_->setEnabled(false);
        callback_->onDestroyActionMode(this);
        levitator_->dismissing();

        std::weak_ptr<Levitator> ptr = levitator_;
        auto frame_view = levitator_->getFrameView();
        if (frame_view) {
            using namespace std::chrono_literals;
            frame_view->animate()
                .alpha(0.f, 100ms).setFinishedHandler(
                    [ptr](AnimationDirector* director)
            {
                auto window = ptr.lock();
                if (window) {
                    window->dismiss();
                }
            }).start();
        }
    }

}