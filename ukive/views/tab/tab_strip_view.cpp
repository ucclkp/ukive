// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/tab/tab_strip_view.h"

#include <cmath>

#include "ukive/animation/interpolator.h"
#include "ukive/graphics/canvas.h"
#include "ukive/views/text_view.h"
#include "ukive/views/tab/tab_strip_selection_listener.h"
#include "ukive/window/window.h"
#include "ukive/elements/ripple_element.h"


namespace ukive {

    TabStripView::TabStripView(Context c)
        : TabStripView(c, {}) {}

    TabStripView::TabStripView(Context c, AttrsRef attrs)
        : SequenceLayout(c, attrs)
    {
        setOrientation(HORIZONTAL);

        using namespace std::chrono_literals;
        animator_.setListener(this);
        animator_.setDuration(150ms);
        animator_.setValueRange(0, 1);
        animator_.setInterpolator(new LinearInterpolator());
        animator_.setRepeat(false);
    }

    void TabStripView::setSelectedItem(size_t index) {
        if (index >= getChildCount()) {
            return;
        }

        auto view = static_cast<TextView*>(getChildAt(index));
        if (sel_view_ != view) {
            if (sel_view_) {
                prev_rect_ = sel_view_->getBounds();
                animator_.reset();
                animator_.start();
                startVSync();
            }

            sel_view_ = view;
            requestDraw();
        }
    }

    void TabStripView::setSelectionListener(TabStripSelectionListener* l) {
        listener_ = l;
    }

    void TabStripView::onDrawOverChildren(Canvas* canvas) {
        SequenceLayout::onDrawOverChildren(canvas);

        if (sel_view_) {
            auto item = sel_view_;
            int nav_height = getContext().dp2pxri(3);
            Rect rect(item->getBounds());
            if (!prev_rect_.empty()) {
                int width = int((rect.width() - prev_rect_.width()) * animator_.getCurValue()) + prev_rect_.width();
                int left = int((rect.x() - prev_rect_.x()) * animator_.getCurValue()) + prev_rect_.x();
                Rect cur_rect(left, rect.bottom() - nav_height, width, nav_height);
                canvas->fillRect(RectF(cur_rect), Color::Blue400);
            } else {
                rect.y(rect.bottom() - nav_height);
                rect.height(nav_height);
                canvas->fillRect(RectF(rect), Color::Blue400);
            }
        }
    }

    void TabStripView::onClick(View* v) {
        if (v == sel_view_) {
            return;
        }

        if (sel_view_) {
            prev_rect_ = sel_view_->getBounds();
            animator_.reset();
            animator_.start();
            startVSync();
        }
        sel_view_ = static_cast<TextView*>(v);

        if (listener_) {
            for (size_t i = 0; i < getChildCount(); ++i) {
                if (getChildAt(i) == v) {
                    listener_->onTabStripSelectionChanged(i);
                    break;
                }
            }
        }

        requestDraw();
    }

    void TabStripView::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        if (!animator_.update(start_time, display_freq)) {
            stopVSync();
        }
        requestDraw();
    }

    size_t TabStripView::addItem(const std::u16string& title) {
        int padding = getContext().dp2pxi(8);

        auto view = new TextView(getContext());
        view->setText(title);
        view->setTextSize(getContext().dp2pxi(12));
        view->setPadding(padding, 0, padding, 0);
        view->setVertAlignment(TextLayout::Alignment::CENTER);
        view->setBackground(new RippleElement());
        view->setClickable(true);
        view->setOnClickListener(this);
        view->setLayoutSize(LS_AUTO, LS_FILL);
        addView(view);

        size_t index = getChildCount() - 1;
        if (index == 0 && !sel_view_) {
            sel_view_ = view;
        }

        return index;
    }

    size_t TabStripView::addItem(size_t index, const std::u16string& title) {
        if (index > getChildCount()) {
            index = getChildCount();
        }

        int padding = getContext().dp2pxi(8);

        auto view = new TextView(getContext());
        view->setText(title);
        view->setTextSize(getContext().dp2pxi(12));
        view->setPadding(padding, 0, padding, 0);
        view->setVertAlignment(TextLayout::Alignment::CENTER);
        view->setBackground(new RippleElement());
        view->setClickable(true);
        view->setOnClickListener(this);
        view->setLayoutSize(LS_AUTO, LS_FILL);
        addView(index, view);

        if (index == 0 && !sel_view_) {
            sel_view_ = view;
        }

        return index;
    }

    void TabStripView::removeItem(size_t index) {
        if (index >= getChildCount()) {
            return;
        }

        auto item = getChildAt(index);
        bool sel_removed = sel_view_ == item;

        removeView(item);

        if (sel_removed) {
            sel_view_ = nullptr;
        }

        if (getChildCount() > index) {
            setSelectedItem(index);
        } else {
            if (index > 0 && getChildCount() > index - 1) {
                setSelectedItem(index - 1);
            } else if (getChildCount() > 0) {
                setSelectedItem(0);
            }
        }
    }

    void TabStripView::clearItems() {
        removeAllViews();
        sel_view_ = nullptr;
    }

    size_t TabStripView::getItemCount() const {
        return getChildCount();
    }

}
