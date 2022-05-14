// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_SCROLL_VIEW_H_
#define UKIVE_VIEWS_SCROLL_VIEW_H_

#include "ukive/animation/scroller.h"
#include "ukive/event/velocity_calculator.h"
#include "ukive/graphics/vsyncable.h"
#include "ukive/views/layout/layout_view.h"


namespace ukive {

    class Scroller;
    class InputEvent;

    class ScrollView : public LayoutView, public VSyncable
    {
    public:
        explicit ScrollView(Context c);
        ScrollView(Context c, AttrsRef attrs);
        ~ScrollView();

        // View
        Size onDetermineSize(const SizeInfo& info) override;
        void onLayout(
            const Rect& new_bounds, const Rect& old_bounds) override;
        void onBoundsChanged(const Rect& new_bounds, const Rect& old_bounds) override;
        void onScrollChanged(
            int scroll_x, int scroll_y, int old_scroll_x, int old_scroll_y) override;

        bool onHookInputEvent(InputEvent* e) override;
        bool onInputEvent(InputEvent* e) override;

        // VSyncCallback
        void onVSync(
            uint64_t start_time, uint32_t display_freq, uint32_t real_interval) override;

    private:
        bool canScroll() const;
        bool canScroll(bool top) const;
        int computeScrollRange() const;
        int computeScrollExtend() const;

        bool processVerticalScroll(int dy);

        Point mouse_pos_cache_{};
        Point mouse_raw_pos_cache_{};
        int saved_pt_;

        Point prev_touch_pos_{};
        Point start_touch_pos_{};
        bool is_touch_down_ = false;

        Scroller scroller_;
        VelocityCalculator velocity_calculator_;
    };

}

#endif  // UKIVE_VIEWS_SCROLL_VIEW_H_