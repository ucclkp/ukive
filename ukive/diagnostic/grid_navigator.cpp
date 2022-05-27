// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "grid_navigator.h"

#include "ukive/animation/view_animator.h"
#include "ukive/elements/element.h"
#include "ukive/views/view.h"
#include "ukive/views/text_view.h"


namespace ukive {

    GridNavigator::GridNavigator(View* v)
        : parent_view_(v)
    {}

    void GridNavigator::create(
        const std::u16string_view& text,
        const Color& text_color,
        const Color& bg_color)
    {
        auto c = parent_view_->getContext();

        int padding_hori = c.dp2pxi(4.f);
        int padding_vert = c.dp2pxi(2.f);

        auto view = new TextView(c);
        view->setText(text);
        view->setTextWeight(TextLayout::FontWeight::BOLD);
        view->setTextSize(c.dp2pxi(12.f));
        view->setTextColor(text_color);
        view->setPadding(padding_hori, padding_vert, padding_hori, padding_vert);

        auto shape_element = new Element(Element::SHAPE_RRECT, bg_color);
        shape_element->setRadius(c.dp2px(4.f));

        levitator_ = std::make_shared<Levitator>();
        levitator_->setLayoutSize(View::LS_FREE, View::LS_FREE);
        levitator_->setShadowRadius(c.dp2pxi(4.f));
        levitator_->setContentView(view);
        levitator_->setOutsideTouchable(true);
        levitator_->setDismissByTouchOutside(false);
        levitator_->setBackground(shape_element);
        levitator_->setEventListener(this);
        levitator_->setLayoutMargin(
            { c.dp2pxi(4), c.dp2pxi(4), c.dp2pxi(4), c.dp2pxi(4) });
    }

    void GridNavigator::showNav(const Point& center_pos) {
        auto bounds = parent_view_->getContentBounds();
        if (bounds.empty()) {
            return;
        }

        Point cp(center_pos);
        cp.x() += bounds.x();
        cp.y() += bounds.y();

        if (bounds.hit(cp)) {
            close();
            return;
        }

        auto root = parent_view_->getBoundsInRoot();
        if (is_finished_) {
            show(cp.x() + root.x(), cp.y() + root.y());
        } else {
            levitator_->update(cp.x() + root.x(), cp.y() + root.y());
        }
    }

    void GridNavigator::closeNav() {
        close();
    }

    void GridNavigator::onBeforeInnerWindowLayout(
        Levitator* lev, Rect* new_bounds, const Rect& old_bounds) {}

    void GridNavigator::show(int x, int y) {
        auto w = parent_view_->getWindow();
        if (!is_finished_ || !w) {
            return;
        }

        is_finished_ = false;

        using namespace std::chrono_literals;

        auto root = parent_view_->getBoundsInRoot();

        Levitator::PosInfo info;
        info.corner = GV_MID_HORI | GV_MID_VERT;
        info.limit_view = parent_view_;
        info.limit_range = parent_view_->getContentBoundsInRoot();

        levitator_->dismiss();
        levitator_->show(w, x, y, info);
        levitator_->getFrameView()->animeParams().setAlpha(0);
        levitator_->getFrameView()->animate()
            .alpha(1, 150ms).start();
        //list_view_->setEnabled(true);
    }

    void GridNavigator::close() {
        if (is_finished_) {
            return;
        }

        is_finished_ = true;

        //list_view_->setEnabled(false);
        levitator_->dismissing();

        std::weak_ptr<Levitator> ptr = levitator_;
        auto frame_view = levitator_->getFrameView();
        if (frame_view) {
            using namespace std::chrono_literals;
            frame_view->animate()
                .alpha(0, 150ms).setFinishedHandler(
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
