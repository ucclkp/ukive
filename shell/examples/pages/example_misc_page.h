// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_EXAMPLES_PAGES_EXAMPLE_MISC_PAGE_H_
#define SHELL_EXAMPLES_PAGES_EXAMPLE_MISC_PAGE_H_

#include "ukive/page/page.h"
#include "ukive/views/click_listener.h"
#include "ukive/animation/animator.h"


namespace ukive {
    class Button;
    class CheckBox;
    class ImageView;
    class ComboBox;
}

namespace shell {

    class ExampleMiscPage :
        public ukive::Page,
        public ukive::OnClickListener,
        public ukive::AnimationListener
    {
    public:
        explicit ExampleMiscPage(ukive::Window* w);

        // ukive::Page
        ukive::View* onCreate(ukive::LayoutView* parent) override;
        void onDestroy() override;

        // ukive::AnimationListener
        void onAnimationProgress(ukive::Animator* animator) override;

        // ukive::OnClickListener
        void onClick(ukive::View* v) override;

    private:
        ukive::Animator animator_;

        ukive::Button* test_button_ = nullptr;
        ukive::ImageView* image_view_ = nullptr;
        ukive::CheckBox* check_box_ = nullptr;
        ukive::ComboBox* combo_box_ = nullptr;
    };

}

#endif  // SHELL_EXAMPLES_PAGES_EXAMPLE_MISC_PAGE_H_