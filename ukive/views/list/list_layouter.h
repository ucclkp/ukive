// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LIST_LAYOUTER_H_
#define UKIVE_VIEWS_LIST_LIST_LAYOUTER_H_

#include "ukive/graphics/size.hpp"
#include "ukive/views/list/list_source.h"
#include "ukive/views/size_info.h"


namespace ukive {

    class ListView;
    class ListSource;
    class ListItemRecycler;

    class ListLayouter {
    public:
        enum Direction {
            TOP = 1 << 0,
            BOTTOM = 1 << 1,
            LEFT = 1 << 2,
            RIGHT = 1 << 3,

            HORIZONTAL = LEFT | RIGHT,
            VERTICAL = TOP | BOTTOM,
            ALL = HORIZONTAL | VERTICAL,
        };

        ListLayouter();
        virtual ~ListLayouter() = default;

        void bind(ListView* parent, ListSource* source);

        virtual Size onDetermineSize(
            int cw, int ch, SizeInfo::Mode wm, SizeInfo::Mode hm) = 0;
        virtual int onLayoutAtPosition(bool cur) = 0;
        virtual int onDataChangedAtPosition(size_t pos, int offset, bool cur) = 0;
        virtual int onSmoothScrollToPosition(size_t pos, int offset) = 0;

        virtual int onFillTopChildren(int dy) = 0;
        virtual int onFillBottomChildren(int dy) = 0;
        virtual int onFillLeftChildren(int dx) = 0;
        virtual int onFillRightChildren(int dx) = 0;

        virtual void onClear() = 0;

        virtual void recordCurPositionAndOffset() = 0;
        virtual void computeTotalHeight(int* prev, int* next) = 0;
        virtual ListItem* findItemFromView(View* v) = 0;

        virtual bool canScroll(Direction dir) const = 0;
        virtual void getCurPosition(size_t* pos, int* offset) const = 0;

    protected:
        bool isAvailable() const;

        ListView* parent_ = nullptr;
        ListSource* source_ = nullptr;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_LAYOUTER_H_