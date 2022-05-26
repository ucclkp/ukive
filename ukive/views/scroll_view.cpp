// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "scroll_view.h"

#include <algorithm>

#include "utils/log.h"

#include "ukive/diagnostic/input_tracker.h"
#include "ukive/event/input_consts.h"
#include "ukive/event/input_event.h"
#include "ukive/window/window.h"


namespace ukive {

    ScrollView::ScrollView(Context c)
        : ScrollView(c, {}) {}

    ScrollView::ScrollView(Context c, AttrsRef attrs)
        : LayoutView(c, attrs),
          saved_pt_(InputEvent::PT_NONE)
    {
        setTouchCapturable(true);
        setCursor(Cursor::NONE);
    }

    ScrollView::~ScrollView() {
    }

    bool ScrollView::canScroll() const {
        View* child = getChildAt(0);
        if (child && child->getVisibility() != VANISHED) {
            if (getDeterminedSize().height() < child->getDeterminedSize().height()) {
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
            return child->getDeterminedSize().height();
        }
        return 0;
    }

    int ScrollView::computeScrollExtend() const {
        View* child = getChildAt(0);
        if (child && child->getVisibility() != VANISHED) {
            return child->getDeterminedSize().height() - getDeterminedSize().height();
        }
        return 0;
    }

    bool ScrollView::processVerticalScroll(int dy) {
        int final_dy = 0;
        if (dy > 0) {
            int scroll_y = getScrollY();
            final_dy = (std::max)(scroll_y - dy, 0) - scroll_y;
        } else if (dy < 0) {
            int scroll_y = getScrollY();
            int extend = computeScrollExtend();
            if (extend > 0) {
                final_dy = (std::min)(scroll_y - dy, extend) - scroll_y;
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

        // TODO: ScrollView can only have one child.
        ubassert(getChildCount() <= 1);

        auto new_info = info;
        new_info.setHeight(SizeInfo::Value(0, SizeInfo::FREEDOM));
        determineChildrenSize(new_info);

        switch (info.width().mode) {
        case SizeInfo::CONTENT: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                int child_width = child->getDeterminedSize().width() + margin.hori();
                final_width = (std::min)(child_width + getPadding().hori(), info.width().val);
            }
            break;
        }

        case SizeInfo::FREEDOM: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                int child_width = child->getDeterminedSize().width() + margin.hori();
                final_width = child_width + getPadding().hori();
            }
            break;
        }

        case SizeInfo::DEFINED:
        default:
            final_width = info.width().val;
            break;
        }

        switch (info.height().mode) {
        case SizeInfo::CONTENT: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                int child_height = child->getDeterminedSize().height() + margin.vert();
                final_height = (std::min)(child_height + getPadding().vert(), info.height().val);
            }
            break;
        }

        case SizeInfo::FREEDOM: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                int child_height = child->getDeterminedSize().height() + margin.vert();
                final_height = child_height + getPadding().vert();
            }
            break;
        }

        case SizeInfo::DEFINED:
        default:
            final_height = info.height().val;
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

            int child_left = getPadding().start() + margin.start();
            int child_top = getPadding().top() + margin.top();

            child->layout(
                Rect(child_left, child_top, size.width(), size.height()));

            // 在内容高度缩小时填充下部可能的空白区域
            int detect_bottom = getBounds().height() - getPadding().bottom() - margin.bottom();
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

        /**
         * 使用鼠标滚动时，我们想即时反应光标的变化。
         * 于是在这里发送 EVM_MOVE 事件，好让当前鼠标指针下的
         * View 决定光标样式。因为 ScrollView 自己也可能会设置光标样式，
         * 如果设置了，就不要发 EVM_MOVE 了，以防两边都设置光标造成光标跳变。
         */
        if (saved_pt_ == InputEvent::PT_MOUSE &&
            getCursor() == Cursor::NONE &&
            !getWindow()->hasPointerHolder())
        {
            InputEvent e;
            e.setEvent(InputEvent::EVM_MOVE);
            e.setPointerType(InputEvent::PT_MOUSE);
            e.setPos(mouse_pos_cache_);
            e.setRawPos(mouse_raw_pos_cache_);
            e.offsetInputPos(getX(), getY());

            std::weak_ptr<void> wptr = getCanary();
            INPUT_TRACK_PRINT("Scroll", &e);
            if (!dispatchInputEvent(&e) && !wptr.expired()) {
                // 如果没人处理，就设置默认光标
                auto w = getWindow();
                if (w) {
                    w->setDefaultCursor();
                }
            }
        }
    }

    bool ScrollView::onInputEvent(InputEvent* e) {
        bool consumed = LayoutView::onInputEvent(e);
        if (e->isTouchEvent()) {
            velocity_calculator_.onInputEvent(e);
        }

        switch (e->getEvent()) {
        case InputEvent::EVM_MOVE:
            break;

        case InputEvent::EVM_WHEEL:
        {
            mouse_pos_cache_ = e->getPos();
            mouse_raw_pos_cache_ = e->getRawPos();

            int wheel = e->getWheelValue();
            if (wheel == 0 || !canScroll(wheel > 0)) {
                break;
            }
            consumed = true;
            saved_pt_ = e->getPointerType();

            if (e->getWheelGranularity() == InputEvent::WG_HIGH_PRECISE) {
                scroller_.finish();
                scroller_.bezier(
                    0, getContext().dp2px(10 * wheel), true);
            } else {
                scroller_.bezier(
                    0, getContext().dp2px(6 * wheel), false);
            }

            /*DLOG(Log::INFO) << "onInputEvent## isWheel:" << e->isWheel()
                << " wheel:" << wheel;*/
            startVSync();
            break;
        }

        case InputEvent::EVT_DOWN:
            scroller_.finish();
            prev_touch_pos_ = start_touch_pos_ = e->getPos();
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

            scroller_.bezier(0, vy, true);
            startVSync();
            break;
        }

        case InputEvent::EV_LEAVE:
            is_touch_down_ = false;
            break;

        case InputEvent::EVT_MOVE:
            if (is_touch_down_) {
                int dx = e->getX() - prev_touch_pos_.x();
                int dy = e->getY() - prev_touch_pos_.y();
                processVerticalScroll(dy);
                //DLOG(Log::INFO) << "EVT_MOVE: dy: " << dy;
                consumed |= true;

                prev_touch_pos_ = e->getPos();
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
            start_touch_pos_ = e->getPos();
            is_touch_down_ = true;
            saved_pt_ = InputEvent::PT_NONE;
            if (!scroller_.isFinished()) {
                return true;
            }
            break;

        case InputEvent::EVT_UP:
            is_touch_down_ = false;
            break;

        case InputEvent::EVT_MOVE:
            if (is_touch_down_) {
                int dx = e->getX() - start_touch_pos_.x();
                int dy = e->getY() - start_touch_pos_.y();
                int r = getContext().dp2pxi(kTouchScrollingThreshold);

                if (/*dx * dx +*/ dy * dy > r * r) {
                    start_touch_pos_ = e->getPos();
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

    void ScrollView::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        //DLOG(Log::INFO) << "********** onVSync: " << start_time;
        if (scroller_.compute(start_time, display_freq)) {
            auto dy = scroller_.getDelta();

            /*DLOG(Log::INFO) << "onPreDraw: "
                << " dy:" << dy;*/

            if (dy != 0 && !processVerticalScroll(dy)) {
                scroller_.finish();
            }
            requestDraw();
        } else {
            stopVSync();
        }
    }

}
