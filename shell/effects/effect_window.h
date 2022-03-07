// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_EFFECTS_EFFECT_WINDOW_H_
#define SHELL_EFFECTS_EFFECT_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/animation/animator.h"
#include "ukive/graphics/vsyncable.h"
#include "ukive/graphics/win/effects/image_effect_dx.h"


namespace ukive {
    class Button;
    class Canvas;
    class ShadowEffect;
    class ImageFrame;
}

namespace shell {

    class EffectWindow :
        public ukive::Window,
        public ukive::AnimationListener,
        public ukive::VSyncable
    {
    public:
        EffectWindow();
        ~EffectWindow();

        void onCreated() override;
        void onPreDrawCanvas(ukive::Canvas* canvas) override;
        void onDestroy() override;
        bool onInputEvent(ukive::InputEvent* e) override;

        void onContextChanged(const ukive::Context& context) override;

        // ukive::AnimationListener
        void onAnimationProgress(ukive::Animator* animator) override;

        // ukive::VSyncable
        void onVSync(
            uint64_t start_time, uint32_t display_freq, uint32_t real_interval) override;

    private:
        void createShadowImages();

        ukive::Button* ce_button_;
        ukive::Animator animator_;

        ukive::GPtr<ukive::ImageFrame> shadow_img_;
        ukive::GPtr<ukive::ImageFrame> image_img_;
        ukive::GPtr<ukive::ImageFrame> content_img_;
        std::unique_ptr<ukive::ShadowEffect> shadow_effect_;
        std::unique_ptr<ukive::win::ImageEffectGPU> image_effect_;
    };

}

#endif  // SHELL_EFFECTS_EFFECT_WINDOW_H_