// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "tooltip.h"

#include "ukive/elements/element.h"
#include "ukive/views/text_view.h"
#include "ukive/system/ui_utils.h"
#include "ukive/window/window.h"
#include "ukive/animation/view_animator.h"


namespace ukive {

    Tooltip::Tooltip(Context c) {
        auto element = new Element(Element::SHAPE_RRECT, Color::White);
        element->setRadius(c.dp2px(2));

        tv_ = new TextView(c);
        tv_->setText(u"");
        tv_->setTextSize(c.dp2pxi(12));
        tv_->setPadding(c.dp2pxi(6), c.dp2pxi(4), c.dp2pxi(6), c.dp2pxi(4));

        levitator_.setBackground(element);
        levitator_.setOutsideTouchable(true);
        levitator_.setShadowRadius(c.dp2pxi(2));
        levitator_.setContentView(tv_);
        levitator_.setMaximumSize(c.dp2pxi(210), 0);

        timer_.setDuration(ukive::Timer::ns(0));
        timer_.setRunner([this]() {
            close();
        });
    }

    void Tooltip::show(View* anchor, int gravity) {
        levitator_.show(anchor, gravity);
    }

    void Tooltip::show(Window* host, int x, int y) {
        auto cursor_size = getCurrentCursorSize();
        auto c = host->getContext();
        auto cursor_w = c.px2dpci(cursor_size.width());
        auto cursor_h = c.px2dpci(cursor_size.height());

        Levitator::PosInfo info;
        info.corner = GV_TOP | GV_MID_START;
        info.is_evaded = true;
        info.pp.set(1, 1, 0, cursor_h);

        using namespace std::chrono_literals;

        bool is_showing = levitator_.isShowing();

        levitator_.show(host, x, y, info);
        if (!is_showing) {
            levitator_.getFrameView()->animeParams().setAlpha(0);
            levitator_.getFrameView()->animate().alpha(1, 50ms).start();
        }

        if (timer_.getDuration().count() != 0) {
            timer_.start();
        } else {
            timer_.stop();
        }
    }

    void Tooltip::close() {
        auto frame_view = levitator_.getFrameView();
        if (frame_view) {
            using namespace std::chrono_literals;
            frame_view->animate()
                .alpha(0.f, 50ms).setFinishedHandler(
                    [this](AnimationDirector* director)
                    {
                        levitator_.dismiss();
                    }).start();
        }
    }

    void Tooltip::setText(const std::u16string_view& text) {
        tv_->setText(text);
    }

    void Tooltip::setTextSize(int size) {
        tv_->setTextSize(size);
    }

    void Tooltip::setTimeout(utl::TimeUtils::nsp timeout) {
        if (timeout.count() == 0) {
            timer_.stop();
        } else {
            timer_.setDuration(timeout);
            timer_.start();
        }
    }

}