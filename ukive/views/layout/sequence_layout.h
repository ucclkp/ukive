// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_SEQUENCE_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_SEQUENCE_LAYOUT_H_

#include "ukive/views/layout/layout_view.h"


namespace ukive {

    class LayoutInfo;

    class SequenceLayout : public LayoutView {
    public:
        enum Orientation {
            VERTICAL = 1,
            HORIZONTAL = 2
        };

        explicit SequenceLayout(Context c);
        SequenceLayout(Context c, AttrsRef attrs);

        void setOrientation(int orientation);
        int getOrientation() const;

        Size onDetermineSize(const SizeInfo& info) override;
        void onLayout(
            const Rect& new_bounds, const Rect& old_bounds) override;

    protected:
        bool isValidExtraLayoutInfo(LayoutInfo* lp) const override;
        LayoutInfo* makeExtraLayoutInfo() const override;
        LayoutInfo* makeExtraLayoutInfo(AttrsRef attrs) const override;

    private:
        void determineWeightedChildrenSize(
            int total_weight, const SizeInfo& parent_info);

        int getVerticalFinalHeight(const SizeInfo& info);

        Size getVerticalSize(const SizeInfo& info);
        Size getHorizontalSize(const SizeInfo& info);

        void layoutVertical(const Rect& parent_bounds);
        void layoutHorizontal(const Rect& parent_bounds);

        int orientation_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_SEQUENCE_LAYOUT_H_