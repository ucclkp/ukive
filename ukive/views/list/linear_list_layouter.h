// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LINEAR_LIST_LAYOUTER_H_
#define UKIVE_VIEWS_LIST_LINEAR_LIST_LAYOUTER_H_

#include "ukive/views/list/list_columns.h"
#include "ukive/views/list/list_layouter.h"


namespace ukive {

    class LinearListLayouter : public ListLayouter {
    public:
        LinearListLayouter();

        void onMeasureAtPosition(bool cur, int width, int height) override;
        int onLayoutAtPosition(bool cur) override;
        int onDataChangedAtPosition(size_t pos, int offset, bool cur) override;
        int onSmoothScrollToPosition(size_t pos, int offset) override;

        int onFillTopChildren(int dy) override;
        int onFillBottomChildren(int dy) override;
        int onFillLeftChildren(int dx) override;
        int onFillRightChildren(int dx) override;

        void onClear() override;

        void recordCurPositionAndOffset() override;
        void computeTotalHeight(int* prev, int* next) override;
        ListItem* findItemFromView(View* v) override;

        bool canScroll(Direction dir) const override;
        void getCurPosition(size_t* pos, int* offset) const override;

    private:
        void recycleTopChildren(int dy);
        void recycleBottomChildren(int dy);

        bool canScrollToTop() const;
        bool canScrollToBottom() const;
        bool canScrollToLeft() const;
        bool canScrollToRight() const;

        Column column_;

        size_t cur_position_;
        int cur_offset_in_position_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LINEAR_LIST_LAYOUTER_H_