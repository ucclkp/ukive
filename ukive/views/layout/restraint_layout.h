// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_H_

#include "ukive/views/layout/layout_view.h"


namespace ukive {

    class RestraintLayoutInfo;

    class RestraintLayout : public LayoutView {
    public:
        explicit RestraintLayout(Context c);
        RestraintLayout(Context c, AttrsRef attrs);

        // LayoutView
        Size onDetermineSize(const SizeInfo& info) override;
        void onLayout(
            const Rect& new_bounds, const Rect& old_bounds) override;

    protected:
        // LayoutView
        LayoutInfo* makeExtraLayoutInfo() const override;
        LayoutInfo* makeExtraLayoutInfo(AttrsRef attrs) const override;
        bool checkExtraLayoutInfo(LayoutInfo* lp) const override;

    private:
        void clearMeasureFlag();

        bool isAttended(View* v) const;

        void measureRestrainedChildren(const SizeInfo& parent_info);

        void getRestrainedChildWidth(
            View* child, RestraintLayoutInfo* rli,
            const SizeInfo::Value& parent_width, SizeInfo::Value* width);

        void getRestrainedChildHeight(
            View* child, RestraintLayoutInfo* rli,
            const SizeInfo::Value& parent_height, SizeInfo::Value* height);

        int getLeftSpacing(View* child, RestraintLayoutInfo* rli);
        int getTopSpacing(View* child, RestraintLayoutInfo* rli);
        int getRightSpacing(View* child, RestraintLayoutInfo* rli);
        int getBottomSpacing(View* child, RestraintLayoutInfo* rli);

        int measureWrappedWidth();
        int measureWrappedHeight();

        void layoutChild(
            View* child, RestraintLayoutInfo* rli,
            int left, int top, int right, int bottom);

        void layoutChildVertical(
            View* child, RestraintLayoutInfo* rli,
            int top, int bottom);
        void layoutChildHorizontal(
            View* child, RestraintLayoutInfo* rli,
            int left, int right);
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_H_