// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "grid_navigator.h"

#include "ukive/animation/view_animator.h"
#include "ukive/elements/shape_element.h"
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

        auto shape_element = new ShapeElement(ShapeElement::ROUND_RECT);
        shape_element->setRadius(c.dp2px(4.f));
        shape_element->setSolidEnable(true);
        shape_element->setSolidColor(bg_color);

        inner_window_ = std::make_shared<InnerWindow>();
        inner_window_->setSize(View::LS_FREE, View::LS_FREE);
        inner_window_->setShadowRadius(c.dp2pxi(4.f));
        inner_window_->setContentView(view);
        inner_window_->setOutsideTouchable(true);
        inner_window_->setDismissByTouchOutside(false);
        inner_window_->setBackground(shape_element);
        inner_window_->setEventListener(this);
    }

    void GridNavigator::showNav(const Rect& rect) {
        auto bounds = parent_view_->getContentBounds();
        bounds.offset(-bounds.left, -bounds.top);
        if (bounds.intersect(rect)) {
            close();
            return;
        }

        int x, y;
        if (rect.left < bounds.left) {
            x = bounds.left;
        } else if (rect.left > bounds.right) {
            x = bounds.right;
        } else {
            x = rect.left;
        }

        if (rect.top < bounds.top) {
            y = bounds.top;
        } else if (rect.top > bounds.bottom) {
            y = bounds.bottom;
        } else {
            y = rect.top;
        }

        bounds = parent_view_->getBoundsInRoot();
        if (is_finished_) {
            show(x + bounds.left, y + bounds.top);
        } else {
            inner_window_->update(x + bounds.left, y + bounds.top);
        }
    }

    void GridNavigator::closeNav() {
        close();
    }

    void GridNavigator::onBeforeInnerWindowLayout(
        InnerWindow* iw, Rect* new_bounds, const Rect& old_bounds)
    {
        if (iw->isShowing()) {
            auto v = iw->getDecorView();
            int width = new_bounds->width();
            int height = new_bounds->height();

            int padding = v->getContext().dp2pxi(6);
            auto root_bounds = parent_view_->getBoundsInRoot();
            auto content_bounds = parent_view_->getContentBounds();

            Rect bounds;
            bounds.set(
                root_bounds.left + content_bounds.left,
                root_bounds.top + content_bounds.top,
                content_bounds.width(), content_bounds.height());

            auto& margin = v->getLayoutMargin();
            if (margin.start < bounds.left + padding) {
                margin.start = bounds.left + padding;
            }
            if (margin.start > bounds.right - padding - width) {
                margin.start = bounds.right - padding - width;
            }

            if (margin.top < bounds.top + padding) {
                margin.top = bounds.top + padding;
            }
            if (margin.top > bounds.bottom - padding - height) {
                margin.top = bounds.bottom - padding - height;
            }

            new_bounds->setPos(margin.start, margin.top);
        }
    }

    void GridNavigator::show(int x, int y) {
        auto w = parent_view_->getWindow();
        if (!is_finished_ || !w) {
            return;
        }

        is_finished_ = false;

        using namespace std::chrono_literals;

        inner_window_->dismiss();
        inner_window_->show(w, x, y);
        inner_window_->getDecorView()->animeParams()->setAlpha(0);
        inner_window_->getDecorView()->animate()->setDuration(150ms)->
            alpha(1)->start();
        //list_view_->setEnabled(true);
    }

    void GridNavigator::close() {
        if (is_finished_) {
            return;
        }

        is_finished_ = true;

        //list_view_->setEnabled(false);
        inner_window_->markDismissing();

        std::weak_ptr<InnerWindow> ptr = inner_window_;
        auto dec_view = inner_window_->getDecorView();
        if (dec_view) {
            using namespace std::chrono_literals;
            dec_view->animate()->
                setDuration(100ms)->alpha(0)->setFinishedHandler(
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
