// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_CHECK_BOX_H_
#define UKIVE_VIEWS_CHECK_BOX_H_

#include "ukive/animation/animator.h"
#include "ukive/graphics/vsyncable.h"
#include "ukive/views/text_view.h"


namespace ukive {

    class OnCheckListener;

    class CheckBox : public TextView, public VSyncable {
    public:
        explicit CheckBox(Context c);
        CheckBox(Context c, AttrsRef attrs);

        void setChecked(bool checked);
        void setOnCheckListener(OnCheckListener* l);

        bool isChecked() const;

    protected:
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

        // VSyncable
        void onVSync(
            uint64_t start_time, uint32_t display_freq, uint32_t real_interval) override;

    private:
        using super = TextView;

        bool checked_;
        Animator anim_;
        OnCheckListener* listener_ = nullptr;
    };

}

#endif  // UKIVE_VIEWS_CHECK_BOX_H_