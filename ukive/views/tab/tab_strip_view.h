// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TAB_TAB_STRIP_VIEW_H_
#define UKIVE_VIEWS_TAB_TAB_STRIP_VIEW_H_

#include <string>

#include "ukive/animation/animator.h"
#include "ukive/graphics/vsyncable.h"
#include "ukive/views/layout/sequence_layout.h"
#include "ukive/views/click_listener.h"


namespace ukive {

    class TextView;
    class TabStripSelectionListener;

    class TabStripView :
        public SequenceLayout,
        public OnClickListener,
        public AnimationListener,
        public VSyncable
    {
    public:
        explicit TabStripView(Context c);
        TabStripView(Context c, AttrsRef attrs);

        // View
        void onDrawOverChildren(Canvas* canvas) override;

        // OnClickListener
        void onClick(View* v) override;

        // VSyncable
        void onVSync(
            uint64_t start_time, uint32_t display_freq, uint32_t real_interval) override;

        void setSelectedItem(size_t index);
        void setSelectionListener(TabStripSelectionListener* l);

        size_t addItem(const std::u16string& title);
        size_t addItem(size_t index, const std::u16string& title);
        void removeItem(size_t index);
        void clearItems();
        size_t getItemCount() const;

    private:
        Rect prev_rect_;
        Animator animator_;

        TextView* sel_view_ = nullptr;
        TabStripSelectionListener* listener_ = nullptr;
    };

}

#endif
