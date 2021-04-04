// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LIST_VIEW_H_
#define UKIVE_VIEWS_LIST_LIST_VIEW_H_

#include <memory>

#include "ukive/animation/scroller.h"
#include "ukive/event/velocity_calculator.h"
#include "ukive/views/layout/layout_view.h"
#include "ukive/views/list/list_source.h"


namespace ukive {

    class ListView;
    class ListLayouter;
    class OverlayScrollBar;
    class ListItemRecycler;
    class ListItemEventRouter;

    class ListItemRecycledListener {
    public:
        virtual ~ListItemRecycledListener() = default;

        virtual void onChildRecycled(ListView* lv, ListItem* item) = 0;
    };


    class ListView :
        public LayoutView,
        public ListItemChangedNotifier
    {
    public:
        explicit ListView(Context c);
        ListView(Context c, AttrsRef attrs);

        void setSource(ListSource* src);
        void setLayouter(ListLayouter* layouter);
        void setSecDimUnknown(bool unknown);
        void scrollToPosition(size_t pos, int offset, bool smooth);
        void setChildRecycledListener(ListItemRecycledListener* l);
        void setItemEventRouter(ListItemEventRouter* router);

        ListItem* findItemFromView(View* v) const;

        ListLayouter* getLayouter() const;
        void getCurPosition(size_t* pos, int* offset) const;

        // View
        void requestLayout() override;

    protected:
        // LayoutView
        LayoutInfo* makeExtraLayoutInfo() const override;
        LayoutInfo* makeExtraLayoutInfo(AttrsRef attrs) const override;
        bool isValidExtraLayoutInfo(LayoutInfo* lp) const override;
        Size onDetermineSize(const SizeInfo& info) override;
        void onLayout(
            const Rect& new_bounds, const Rect& old_bounds) override;
        bool onHookInputEvent(InputEvent* e) override;
        bool onInputEvent(InputEvent* e) override;
        void onDraw(Canvas* canvas) override;
        void onDrawOverChildren(Canvas* canvas) override;
        void onPreDraw() override;

    private:
        struct SizeCache {
            bool available = false;
            int width = 0;
            int height = 0;
        };

        void freezeLayout();
        void unfreezeLayout();

        int processVerticalScroll(int dy);
        int determineVerticalScroll(int dy);
        void offsetChildrenVertical(int dy);

        ListItem* makeNewItem(size_t data_pos, size_t view_index);
        void setItemData(ListItem* item, size_t data_pos);
        void recycleItem(ListItem* item);

        bool findViewIndexFromStart(ListItem* item, size_t* index) const;
        bool findViewIndexFromEnd(ListItem* item, size_t* index) const;

        void measureItem(ListItem* item, int max_width, int* width, int* height);
        void layoutItem(ListItem* item, int left, int top, int width, int height);

        void updateOverlayScrollBar();
        void recordCurPositionAndOffset();

        int fillTopChildViews(int dy);
        int fillBottomChildViews(int dy);

        void layoutAtPosition(bool cur);
        void refreshAtPosition(size_t pos, int offset, bool cur);
        void smoothScrollToPosition(size_t pos, int offset);

        void onScrollBarChanged(int dy);

        // ListItemChangedNotifier
        void onDataChanged() override;
        void onItemInserted(size_t start_pos, size_t count) override;
        void onItemChanged(size_t start_pos, size_t count) override;
        void onItemRemoved(size_t start_pos, size_t count) override;

        bool is_mouse_down_ = false;

        int prev_touch_x_ = 0;
        int prev_touch_y_ = 0;
        int start_touch_x_ = 0;
        int start_touch_y_ = 0;
        bool is_touch_down_ = false;

        Scroller scroller_;
        VelocityCalculator velocity_calculator_;

        ListSource* source_ = nullptr;
        std::unique_ptr<ListLayouter> layouter_;
        std::unique_ptr<OverlayScrollBar> scroll_bar_;
        std::unique_ptr<ListItemRecycler> recycler_;
        std::unique_ptr<ListItemEventRouter> event_router_;
        ListItemRecycledListener* recycled_listener_ = nullptr;

        bool is_layout_frozen_ = false;
        bool is_sec_dim_unknown_ = false;

        friend class FlowListLayouter;
        friend class GridListLayouter;
        friend class LinearListLayouter;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_VIEW_H_