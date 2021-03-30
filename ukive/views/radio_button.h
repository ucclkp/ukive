// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_RADIO_BUTTON_H_
#define UKIVE_VIEWS_RADIO_BUTTON_H_

#include "ukive/animation/animator.h"
#include "ukive/views/text_view.h"


namespace ukive {

    class OnRadioSelectedListener;

    class RadioButton : public TextView {
    public:
        static void StartGroup();
        static void EndGroup();

        explicit RadioButton(Context c);
        RadioButton(Context c, AttrsRef attrs);
        ~RadioButton();

        void setChecked(bool checked);
        void setOnSelectedListener(OnRadioSelectedListener* l);

        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

    private:
        using super = TextView;

        static int g_group_id_;
        static std::map<int, std::vector<RadioButton*>> g_groups_;

        int group_id_;
        bool selected_;
        Animator anim_;
        OnRadioSelectedListener* listener_ = nullptr;
    };

}

#endif  // UKIVE_VIEWS_RADIO_BUTTON_H_