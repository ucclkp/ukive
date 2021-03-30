// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "list_view.h"

#include <algorithm>

#include "utils/number.hpp"

#include "ukive/event/input_consts.h"
#include "ukive/event/input_event.h"
#include "ukive/window/window.h"
#include "ukive/views/list/list_item_recycler.h"
#include "ukive/views/list/overlay_scroll_bar.h"
#include "ukive/views/list/list_layouter.h"
#include "ukive/views/list/list_item.h"


namespace ukive {

    ListView::ListView(Context c)
        : ListView(c, {}) {}

    ListView::ListView(Context c, AttrsRef attrs)
        : LayoutView(c, attrs),
          scroller_(c)
    {
        scroll_bar_ = std::make_unique<OverlayScrollBar>();
        scroll_bar_->registerScrollHandler(std::bind(&ListView::onScrollBarChanged, this, std::placeholders::_1));
        scroll_bar_->setScrollBarWidth(c.dp2pxi(8));
        scroll_bar_->setScrollBarMinHeight(c.dp2pxi(16));

        recycler_ = std::make_unique<ListItemRecycler>(this);

        setTouchCapturable(true);
    }

    Size ListView::onDetermineSize(const SizeInfo& info) {
        int width = std::max(info.width.val, getMinimumSize().width);
        int height = std::max(info.height.val, getMinimumSize().height);

        scroller_.finish();

        if (layouter_) {
            layouter_->onMeasureAtPosition(true, width, height);
        }

        return Size(width, height);
    }

    void ListView::onLayout(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        if (new_bounds.size() != old_bounds.size()) {
            layoutAtPosition(true);
            recordCurPositionAndOffset();

            auto& size = getDeterminedSize();
            scroll_bar_->setBounds(Rect(0, 0, size.width, size.height));
            updateOverlayScrollBar();
        }
    }

    bool ListView::onHookInputEvent(InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
            if (scroll_bar_->onMousePressed({ e->getX(), e->getY() })) {
                return true;
            }
            break;

        case InputEvent::EVM_MOVE:
            if (scroll_bar_->isInScrollBar({ e->getX(), e->getY() })) {
                return true;
            }
            break;

        case InputEvent::EVT_DOWN:
            start_touch_x_ = e->getX();
            start_touch_y_ = e->getY();
            is_touch_down_ = true;
            if (!scroller_.isFinished()) {
                return true;
            }
            break;

        case InputEvent::EVT_UP:
        case InputEvent::EV_LEAVE_VIEW:
            is_touch_down_ = false;
            break;

        case InputEvent::EVT_MOVE:
            if (is_touch_down_) {
                int dx = e->getX() - start_touch_x_;
                int dy = e->getY() - start_touch_y_;
                int r = getContext().dp2px(kTouchScrollingThreshold);

                if (dx * dx + dy * dy > r * r) {
                    start_touch_x_ = e->getX();
                    start_touch_y_ = e->getY();
                    return true;
                }
            }
            break;

        default:
            break;
        }
        return false;
    }

    bool ListView::onInputEvent(InputEvent* e) {
        bool result = LayoutView::onInputEvent(e);
        if (!layouter_) {
            return result;
        }

        if (e->isTouchEvent()) {
            velocity_calculator_.onInputEvent(e);
        }

        switch (e->getEvent()) {
        case InputEvent::EVM_WHEEL:
        {
            int wheel = e->getWheelValue();
            if (wheel == 0 ||
                !layouter_->canScroll(wheel > 0 ? ListLayouter::TOP : ListLayouter::BOTTOM))
            {
                break;
            }

            result = true;
            if (e->getWheelGranularity() == InputEvent::WG_HIGH_PRECISE) {
                scroller_.bezier(
                    0, getContext().dp2px(10 * wheel),
                    Scroller::Continuity::Time, true);
            } else {
                scroller_.bezier(
                    0, getContext().dp2px(6 * wheel),
                    Scroller::Continuity::VelocityAndTime, false);
            }
            requestDraw();
            break;
        }

        case InputEvent::EVM_DOWN:
            result = is_mouse_down_ = scroll_bar_->onMousePressed({ e->getX(), e->getY() });
            if (result) {
                requestDraw();
            }
            break;

        case InputEvent::EVM_MOVE:
            if (is_mouse_down_) {
                result = true;
                scroll_bar_->onMouseDragged({ e->getX(), e->getY() });
                requestDraw();
            } else if (scroll_bar_->isInScrollBar({ e->getX(), e->getY() })) {
                result = true;
            } else {
                invalidateHookStatus();
            }
            break;

        case InputEvent::EVM_UP:
            is_mouse_down_ = false;
            break;

        case InputEvent::EVT_DOWN:
            scroller_.finish();
            prev_touch_x_ = start_touch_x_ = e->getX();
            prev_touch_y_ = start_touch_y_ = e->getY();
            is_touch_down_ = true;
            result = true;
            break;

        case InputEvent::EVT_UP:
        {
            is_touch_down_ = false;

            float vy = velocity_calculator_.getVelocityY();
            if (vy == 0 || !layouter_->canScroll(vy > 0 ? ListLayouter::TOP : ListLayouter::BOTTOM)) {
                break;
            }
            result = true;

            /*DLOG(Log::INFO) << "EVT_UP | vx=" << velocity_calculator_.getVelocityX()
                << " vy=" << velocity_calculator_.getVelocityY();*/

            scroller_.bezier(
                0, vy, Scroller::Continuity::VelocityAndTime, true);
            requestDraw();
            break;
        }

        case InputEvent::EV_LEAVE_VIEW:
            is_touch_down_ = false;
            break;

        case InputEvent::EVT_MOVE:
            if (is_touch_down_) {
                int dx = e->getX() - prev_touch_x_;
                int dy = e->getY() - prev_touch_y_;
                processVerticalScroll(dy);
                result = true;

                prev_touch_x_ = e->getX();
                prev_touch_y_ = e->getY();
                requestDraw();
            }
            break;

        default:
            break;
        }

        return result;
    }

    void ListView::onDraw(Canvas* canvas) {
        LayoutView::onDraw(canvas);
    }

    void ListView::onDrawOverChildren(Canvas* canvas) {
        LayoutView::onDrawOverChildren(canvas);
        scroll_bar_->onDraw(canvas);
    }

    void ListView::onPreDraw() {
        if (scroller_.compute()) {
            auto dy = scroller_.getDelta();

            if (dy != 0 && processVerticalScroll(dy) == 0) {
                scroller_.finish();
            }
            requestDraw();
        }
    }

    void ListView::requestLayout() {
        if (is_layout_frozen_) {
            return;
        }
        LayoutView::requestLayout();
    }

    void ListView::onClick(View* v) {
        if (selected_listener_) {
            auto item = layouter_->findItemFromView(v);
            if (item) {
                selected_listener_->onItemClicked(this, item);
            }
        }
    }

    bool ListView::onInputReceived(View* v, InputEvent* e, bool* ret) {
        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
        case InputEvent::EVT_DOWN:
            if (selected_listener_) {
                auto item = layouter_->findItemFromView(v);
                if (item) {
                    selected_listener_->onItemPressed(this, item);
                }
            }
            break;

        default:
            break;
        }
        return false;
    }

    void ListView::setSource(ListSource* src) {
        if (source_.get() == src) {
            return;
        }

        scroller_.finish();

        if (source_) {
            recycler_->clear();
            removeAllViews();

            source_->setNotifier(nullptr);
        }

        if (layouter_) {
            layouter_->bind(this, src);
            layouter_->onClear();
        }

        if (src) {
            source_.reset(src);
            source_->setNotifier(this);
            layoutAtPosition(false);
        }

        requestDraw();
    }

    void ListView::setLayouter(ListLayouter* layouter) {
        layouter_.reset(layouter);
        if (layouter_) {
            layouter_->bind(this, source_.get());
        }
    }

    void ListView::setSecDimUnknown(bool unknown) {
        if (is_sec_dim_unknown_ != unknown) {
            is_sec_dim_unknown_ = unknown;
            requestLayout();
            requestDraw();
        }
    }

    void ListView::scrollToPosition(int pos, int offset, bool smooth) {
        if (smooth) {
            smoothScrollToPosition(pos, offset);
        } else {
            directScrollToPosition(pos, offset, false);
        }
    }

    void ListView::setItemSelectedListener(ListItemSelectedListener* l) {
        selected_listener_ = l;
    }

    void ListView::setChildRecycledListener(ListItemRecycledListener* l) {
        recycled_listener_ = l;
    }

    void ListView::getCurPosition(int* pos, int* offset) const {
        if (layouter_) {
            layouter_->getCurPosition(pos, offset);
        } else {
            *pos = 0;
            if (offset) *offset = 0;
        }
    }

    void ListView::freezeLayout() {
        is_layout_frozen_ = true;
    }

    void ListView::unfreezeLayout() {
        is_layout_frozen_ = false;
    }

    int ListView::processVerticalScroll(int dy) {
        int res_dy = determineVerticalScroll(dy);
        if (res_dy == 0) {
            return 0;
        }
        offsetChildViewTopAndBottom(res_dy);
        recordCurPositionAndOffset();
        updateOverlayScrollBar();
        return res_dy;
    }

    int ListView::determineVerticalScroll(int dy) {
        if (dy > 0) {  // 向上滚动，当前页面内容下沉
            dy = fillTopChildViews(dy);
        } else if (dy < 0) {  // 向下滚动，当前页面内容上升
            dy = fillBottomChildViews(dy);
        }

        return dy;
    }

    void ListView::offsetChildViewTopAndBottom(int dy) {
        int size = getChildCount();
        for (int i = 0; i < size; ++i) {
            auto child = getChildAt(i);
            child->offsetVertical(dy);
        }
    }

    ListItem* ListView::makeNewItem(int data_pos, int view_index) {
        int item_id = source_->onListGetItemId(data_pos);
        auto new_item = recycler_->reuse(item_id, view_index);
        if (!new_item) {
            new_item = source_->onListCreateItem(this, data_pos);
            recycler_->addToParent(new_item, view_index);
        }

        new_item->item_id = item_id;
        new_item->data_pos = data_pos;
        source_->onListSetItemData(new_item, data_pos);
        new_item->item_view->setOnClickListener(this);
        new_item->item_view->setOnInputEventDelegate(this);
        return new_item;
    }

    void ListView::recycleItem(ListItem* item) {
        if (recycled_listener_) {
            recycled_listener_->onChildRecycled(this, item);
        }
        recycler_->recycleFromParent(item);
    }

    int ListView::findViewIndexFromStart(ListItem* item) const {
        for (size_t i = 0; i < getChildCount(); ++i) {
            if (getChildAt(i) == item->item_view) {
                return utl::num_cast<int>(i);
            }
        }
        return -1;
    }

    int ListView::findViewIndexFromEnd(ListItem* item) const {
        for (size_t i = getChildCount(); i-- > 0;) {
            if (getChildAt(i) == item->item_view) {
                return utl::num_cast<int>(i);
            }
        }
        return -1;
    }

    void ListView::measureItem(ListItem* item, int max_width, int* width, int* height) {
        auto& child_size = item->item_view->getLayoutSize();
        int width_margin = item->getHoriMargins();
        int height_margin = item->getVertMargins();

        auto child_width = SizeInfo::getChildSizeInfo(
            SizeInfo::Value(max_width, is_sec_dim_unknown_ ? SizeInfo::FREEDOM : SizeInfo::DEFINED),
            width_margin,
            child_size.width);

        auto child_height = SizeInfo::getChildSizeInfo(
            SizeInfo::Value(0, SizeInfo::FREEDOM), 0,
            child_size.height);

        item->item_view->measure(SizeInfo(child_width, child_height));

        int t_width = item->item_view->getDeterminedSize().width + width_margin;
        int t_height = item->item_view->getDeterminedSize().height + height_margin;

        *width = t_width;
        *height = t_height;
    }

    void ListView::layoutItem(ListItem* item, int left, int top, int width, int height) {
        auto& margin = item->item_view->getLayoutMargin();

        Rect bounds;
        bounds.left = left + margin.start + item->ex_margins.left;
        bounds.top = top + margin.top + item->ex_margins.top;
        bounds.right = left + width - margin.end - item->ex_margins.right;
        bounds.bottom = top + height - margin.bottom - item->ex_margins.bottom;
        item->item_view->layout(bounds);
    }

    void ListView::updateOverlayScrollBar() {
        if (!layouter_) {
            return;
        }

        int prev, next;
        layouter_->computeTotalHeight(&prev, &next);
        int total_height = prev + next;

        float percent = static_cast<float>(prev) / (total_height - getHeight());
        percent = std::max(0.f, percent);
        percent = std::min(1.f, percent);

        scroll_bar_->update(total_height, percent);
    }

    void ListView::recordCurPositionAndOffset() {
        if (layouter_) {
            layouter_->recordCurPositionAndOffset();
        }
    }

    int ListView::fillTopChildViews(int dy) {
        if (!layouter_) {
            return 0;
        }
        return layouter_->onFillTopChildren(dy);
    }

    int ListView::fillBottomChildViews(int dy) {
        if (!layouter_) {
            return 0;
        }
        return layouter_->onFillBottomChildren(dy);
    }

    void ListView::layoutAtPosition(bool cur) {
        if (!layouter_) {
            return;
        }

        auto bounds = getContentBounds();
        if (bounds.empty()) {
            return;
        }

        if (!cur) {
            scroller_.finish();
        }

        int diff = layouter_->onLayoutAtPosition(cur);
        if (diff > 0) {
            diff = determineVerticalScroll(diff);
            if (diff != 0) {
                offsetChildViewTopAndBottom(diff);
            }
        }
    }

    void ListView::directScrollToPosition(int pos, int offset, bool cur) {
        if (!layouter_) {
            return;
        }
        auto bounds = getContentBounds();
        if (bounds.empty()) {
            return;
        }

        if (!cur) {
            scroller_.finish();
        }

        int diff = layouter_->onScrollToPosition(pos, offset, cur);
        if (diff != 0) {
            diff = fillTopChildViews(diff);
            if (diff != 0) {
                offsetChildViewTopAndBottom(diff);
            }
        }

        recordCurPositionAndOffset();
        updateOverlayScrollBar();
        requestDraw();
    }

    void ListView::smoothScrollToPosition(int pos, int offset) {
        if (!layouter_) {
            return;
        }
        auto bounds = getContentBounds();
        if (bounds.empty()) {
            return;
        }

        int total_height = layouter_->onSmoothScrollToPosition(pos, offset);
        if (total_height != 0) {
            scroller_.finish();
            recordCurPositionAndOffset();
        }
    }

    void ListView::onScrollBarChanged(int dy) {
        int prev, next;
        layouter_->computeTotalHeight(&prev, &next);
        int final_dy = determineVerticalScroll(prev - dy);
        if (final_dy == 0) {
            return;
        }

        offsetChildViewTopAndBottom(final_dy);
        recordCurPositionAndOffset();
        updateOverlayScrollBar();
        requestDraw();
    }

    void ListView::onDataChanged() {
        recordCurPositionAndOffset();
        directScrollToPosition(0, 0, true);
        requestDraw();
    }

    void ListView::onItemInserted(size_t start_pos, size_t count) {
        if (count > 0) {
            //TODO: post insert op.
        }
    }

    void ListView::onItemChanged(size_t start_pos, size_t count) {
        if (count > 0) {
            //TODO: post change op.
        }
    }

    void ListView::onItemRemoved(size_t start_pos, size_t count) {
        if (count > 0) {
            //TODO: post remove op.
        }
    }

}