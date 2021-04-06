// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "scroll_view.h"

#include <algorithm>

#include "utils/log.h"

#include "ukive/event/input_consts.h"
#include "ukive/event/input_event.h"
#include "ukive/window/window.h"


namespace ukive {

    ScrollView::ScrollView(Context c)
        : ScrollView(c, {}) {}

    ScrollView::ScrollView(Context c, AttrsRef attrs)
        : LayoutView(c, attrs),
          mouse_x_cache_(0),
          mouse_y_cache_(0),
          saved_pointer_type_(InputEvent::PT_NONE),
          scroller_(c)
    {
        setTouchCapturable(true);
    }

    bool ScrollView::canScroll() const {
        View* child = getChildAt(0);
        if (child && child->getVisibility() != VANISHED) {
            if (getDeterminedSize().height < child->getDeterminedSize().height) {
                return true;
            }
        }
        return false;
    }

    bool ScrollView::canScroll(bool top) const {
        int scroll_y = getScrollY();
        if (top) {
            if (scroll_y == 0) {
                return false;
            }
        } else {
            int extend = computeScrollExtend();
            if (extend <= 0 || scroll_y == extend) {
                return false;
            }
        }
        return true;
    }

    int ScrollView::computeScrollRange() const {
        View* child = getChildAt(0);
        if (child && child->getVisibility() != VANISHED) {
            return child->getDeterminedSize().height;
        }
        return 0;
    }

    int ScrollView::computeScrollExtend() const {
        View* child = getChildAt(0);
        if (child && child->getVisibility() != VANISHED) {
            return child->getDeterminedSize().height - getDeterminedSize().height;
        }
        return 0;
    }

    bool ScrollView::processVerticalScroll(int dy) {
        int final_dy = 0;
        if (dy > 0) {
            int scroll_y = getScrollY();
            final_dy = std::max(scroll_y - dy, 0) - scroll_y;
        } else if (dy < 0) {
            int scroll_y = getScrollY();
            int extend = computeScrollExtend();
            if (extend > 0) {
                final_dy = std::min(scroll_y - dy, extend) - scroll_y;
            }
        }

        if (final_dy != 0) {
            scrollBy(0, final_dy);
            return true;
        }
        return false;
    }

    Size ScrollView::onDetermineSize(const SizeInfo& info) {
        int final_width = 0;
        int final_height = 0;

        // TODO:
        DCHECK(getChildCount() <= 1) << "ScrollView can only have one child.";

        auto new_info = info;
        new_info.setHeight(SizeInfo::Value(0, SizeInfo::FREEDOM));
        determineChildrenSize(new_info);

        switch (info.width.mode) {
        case SizeInfo::CONTENT: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                int child_width = child->getDeterminedSize().width + margin.hori();
                final_width = std::min(child_width + getPadding().hori(), info.width.val);
            }
            break;
        }

        case SizeInfo::FREEDOM: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                int child_width = child->getDeterminedSize().width + margin.hori();
                final_width = child_width + getPadding().hori();
            }
            break;
        }

        case SizeInfo::DEFINED:
        default:
            final_width = info.width.val;
            break;
        }

        switch (info.height.mode) {
        case SizeInfo::CONTENT: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                int child_height = child->getDeterminedSize().height + margin.vert();
                final_height = std::min(child_height + getPadding().vert(), info.height.val);
            }
            break;
        }

        case SizeInfo::FREEDOM: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                int child_height = child->getDeterminedSize().height + margin.vert();
                final_height = child_height + getPadding().vert();
            }
            break;
        }

        case SizeInfo::DEFINED:
        default:
            final_height = info.height.val;
            break;
        }

        return Size(final_width, final_height);
    }

    void ScrollView::onLayout(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        auto child = getChildAt(0);
        if (child && child->getVisibility() != VANISHED) {
            auto& margin = child->getLayoutMargin();

            auto& size = child->getDeterminedSize();

            int child_left = getPadding().start + margin.start;
            int child_top = getPadding().top + margin.top;

            child->layout(
                Rect(child_left, child_top, size.width, size.height));

            // 在内容高度缩小时填充下部可能的空白区域
            int detect_bottom = getBounds().height() - getPadding().bottom - margin.bottom;
            if (child->getBottom() - getScrollY() < detect_bottom && canScroll()) {
                scrollBy(0, child->getBottom() - getScrollY() - detect_bottom);
            }
        }
    }

    void ScrollView::onBoundsChanged(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        if (new_bounds.size() == old_bounds.size()) {
            return;
        }

        if (!new_bounds.empty() &&
            !old_bounds.empty())
        {
            int changed = 0;
            if (canScroll()) {
                if (getScrollY() < 0) {
                    changed = -getScrollY();
                }

                int extend = computeScrollExtend();
                if (getScrollY() > extend) {
                    changed = extend - getScrollY();
                }
            } else {
                if (getScrollY() != 0) {
                    changed = -getScrollY();
                }
            }

            if (changed != 0) {
                scrollBy(0, changed);
            }
        }
    }

    void ScrollView::onScrollChanged(
        int scroll_x, int scroll_y, int old_scroll_x, int old_scroll_y)
    {
        LayoutView::onScrollChanged(scroll_x, scroll_y, old_scroll_x, old_scroll_y);

        // 当前窗口存在有鼠标/触摸焦点的 View 时，不应产生
        // EVM_SCROLL_ENTER 消息
        if (saved_pointer_type_ == InputEvent::PT_MOUSE &&
            getWindow()->getMouseHolderRef() == 0 &&
            getWindow()->getTouchHolderRef() == 0)
        {
            InputEvent e;
            e.setEvent(InputEvent::EVM_SCROLL_ENTER);
            e.setPointerType(saved_pointer_type_);
            e.setX(mouse_x_cache_);
            e.setY(mouse_y_cache_);
            dispatchInputEvent(&e);
        }
    }

    bool ScrollView::onInputEvent(InputEvent* e) {
        bool consumed = LayoutView::onInputEvent(e);
        if (e->isTouchEvent()) {
            velocity_calculator_.onInputEvent(e);
        }

        switch (e->getEvent()) {
        case InputEvent::EVM_MOVE:
            consumed = true;
            break;

        case InputEvent::EVM_WHEEL:
        {
            mouse_x_cache_ = e->getX();
            mouse_y_cache_ = e->getY();
            saved_pointer_type_ = e->getPointerType();

            int wheel = e->getWheelValue();
            if (wheel == 0 || !canScroll(wheel > 0)) {
                break;
            }
            consumed = true;

            if (e->getWheelGranularity() == InputEvent::WG_HIGH_PRECISE) {
                saved_pointer_type_ = InputEvent::PT_NONE;
                scroller_.bezier(
                    0, getContext().dp2px(10 * wheel),
                    Scroller::Continuity::Time, true);
            } else {
                scroller_.bezier(
                    0, getContext().dp2px(6 * wheel),
                    Scroller::Continuity::VelocityAndTime, false);
            }

            /*DLOG(Log::INFO) << "onInputEvent## isWheel:" << e->isWheel()
                << " wheel:" << wheel;*/
            requestDraw();
            break;
        }

        case InputEvent::EVT_DOWN:
            scroller_.finish();
            prev_touch_x_ = start_touch_x_ = e->getX();
            prev_touch_y_ = start_touch_y_ = e->getY();
            is_touch_down_ = true;
            consumed |= true;
            break;

        case InputEvent::EVT_UP:
        {
            is_touch_down_ = false;

            float vy = velocity_calculator_.getVelocityY();
            if (vy == 0 || !canScroll(vy > 0)) {
                break;
            }
            consumed = true;

            //DLOG(Log::INFO) << "EVT_UP | vy=" << vy;

            scroller_.bezier(
                0, vy, Scroller::Continuity::VelocityAndTime, true);
            requestDraw();
            break;
        }

        case InputEvent::EV_LEAVE_VIEW:
            is_touch_down_ = false;
            saved_pointer_type_ = InputEvent::PT_NONE;
            break;

        case InputEvent::EVT_MOVE:
            if (is_touch_down_) {
                int dx = e->getX() - prev_touch_x_;
                int dy = e->getY() - prev_touch_y_;
                mouse_x_cache_ = e->getX();
                mouse_y_cache_ = e->getY();
                saved_pointer_type_ = e->getPointerType();
                processVerticalScroll(dy);
                //DLOG(Log::INFO) << "EVT_MOVE: dy: " << dy;
                consumed |= true;

                prev_touch_x_ = e->getX();
                prev_touch_y_ = e->getY();
            }
            break;

        default:
            break;
        }

        return consumed;
    }

    bool ScrollView::onHookInputEvent(InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVT_DOWN:
            start_touch_x_ = e->getX();
            start_touch_y_ = e->getY();
            is_touch_down_ = true;
            if (!scroller_.isFinished()) {
                return true;
            }
            break;

        case InputEvent::EVT_UP:
            is_touch_down_ = false;
            saved_pointer_type_ = InputEvent::PT_NONE;
            break;

        case InputEvent::EVT_MOVE:
            if (is_touch_down_) {
                int dx = e->getX() - start_touch_x_;
                int dy = e->getY() - start_touch_y_;
                int r = getContext().dp2pxi(kTouchScrollingThreshold);

                if (/*dx * dx +*/ dy * dy > r * r) {
                    start_touch_x_ = e->getX();
                    start_touch_y_ = e->getY();
                    return true;
                }
            }
            break;

        case InputEvent::EVM_MOVE:
            break;

        default:
            break;
        }

        return false;
    }

    void ScrollView::onPreDraw() {
        if (scroller_.compute()) {
            auto dy = scroller_.getDelta();

            /*DLOG(Log::INFO) << "onPreDraw: "
                << " dy:" << dy;*/

            if (dy != 0 && !processVerticalScroll(dy)) {
                scroller_.finish();
            }
            requestDraw();
        }
    }

}
