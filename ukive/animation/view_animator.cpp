// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/view_animator.h"

#include "ukive/views/view.h"
#include "ukive/window/window.h"

#include "ukive/animation/interpolator.h"


namespace ukive {

    ViewAnimator::ViewAnimator(View* v)
        : owner_view_(v),
          listener_(nullptr)
    {
        using namespace std::chrono_literals;
        duration_ = 200ms;
        director_.setListener(this);
    }

    ViewAnimator::~ViewAnimator() {
    }

    void ViewAnimator::start() {
        director_.start();
        startVSync();
    }

    void ViewAnimator::cancel() {
        director_.stop();
    }

    ViewAnimator* ViewAnimator::setDuration(Animator::nsp duration) {
        duration_ = duration;
        return this;
    }

    ViewAnimator* ViewAnimator::alpha(double value) {
        auto anim = director_.add(VIEW_ANIM_ALPHA);
        anim->setInitValue(owner_view_->animeParams()->getAlpha());
        anim->setDuration(duration_);
        anim->setInterpolator(new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::scaleX(double value) {
        auto anim = director_.add(VIEW_ANIM_SCALE_X);
        anim->setInitValue(owner_view_->animeParams()->getScaleX());
        anim->setDuration(duration_);
        anim->setInterpolator(new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::scaleY(double value) {
        auto anim = director_.add(VIEW_ANIM_SCALE_Y);
        anim->setInitValue(owner_view_->animeParams()->getScaleY());
        anim->setDuration(duration_);
        anim->setInterpolator(new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::rotate(double value) {
        auto anim = director_.add(VIEW_ANIM_ROTATE);
        anim->setInitValue(owner_view_->animeParams()->getRotate());
        anim->setDuration(duration_);
        anim->setInterpolator(new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::translateX(double value) {
        auto anim = director_.add(VIEW_ANIM_TRANSLATE_X);
        anim->setInitValue(owner_view_->animeParams()->getTranslateX());
        anim->setDuration(duration_);
        anim->setInterpolator(new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::translateY(double value) {
        auto anim = director_.add(VIEW_ANIM_TRANSLATE_Y);
        anim->setInitValue(owner_view_->animeParams()->getTranslateY());
        anim->setDuration(duration_);
        anim->setInterpolator(new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::rectReveal(
        tvalcr x, tvalcr y,
        tvalcr start_width, tvalcr end_width,
        tvalcr start_height, tvalcr end_height)
    {
        auto anim = director_.add(VIEW_ANIM_RECT_REVEAL_R_X);
        anim->setInitValue(0);
        anim->setDuration(duration_);
        anim->setInterpolator(new LinearInterpolator(1));

        anim = director_.add(VIEW_ANIM_RECT_REVEAL_R_Y);
        anim->setInitValue(0);
        anim->setDuration(duration_);
        anim->setInterpolator(new LinearInterpolator(1));

        auto p = owner_view_->animeParams();
        p->setHasReveal(true);
        p->reveal().setRect(
            x, y,
            start_width, end_width,
            start_height, end_height);

        return this;
    }

    ViewAnimator* ViewAnimator::circleReveal(
        tvalcr center_x, tvalcr center_y, tvalcr start_radius, tvalcr end_radius)
    {
        auto anim = director_.add(VIEW_ANIM_CIRCLE_REVEAL_R);
        anim->setInitValue(0);
        anim->setDuration(duration_);
        anim->setInterpolator(new LinearInterpolator(1));

        auto p = owner_view_->animeParams();
        p->setHasReveal(true);
        p->reveal().setCircle(
            center_x, center_y,
            start_radius, end_radius);

        return this;
    }

    ViewAnimator* ViewAnimator::setListener(AnimationDirectorListener* l) {
        listener_ = l;
        return this;
    }

    ViewAnimator* ViewAnimator::setFinishedHandler(const FinishedHandler& h) {
        finished_handler_ = h;
        return this;
    }

    void ViewAnimator::onPreViewDraw() {}

    void ViewAnimator::onPostViewDraw() {}

    void ViewAnimator::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        director_.update(start_time, display_freq);
        if (director_.isRunning()) {
            requestVSync();
        } else {
            stopVSync();
        }
        owner_view_->requestDraw();
    }

    void ViewAnimator::onDirectorStarted(AnimationDirector* director) {
        if (listener_) {
            listener_->onDirectorStarted(director);
        }
    }

    void ViewAnimator::onDirectorProgress(AnimationDirector* director) {
        if (listener_) {
            listener_->onDirectorProgress(director);
        }
    }

    void ViewAnimator::onDirectorStopped(AnimationDirector* director) {
        if (listener_) {
            listener_->onDirectorStopped(director);
        }
    }

    void ViewAnimator::onDirectorFinished(AnimationDirector* director) {
        if (finished_handler_) {
            finished_handler_(director);
        }

        if (listener_) {
            listener_->onDirectorFinished(director);
        }
    }

    void ViewAnimator::onDirectorReset(AnimationDirector* director) {
        if (listener_) {
            listener_->onDirectorReset(director);
        }
    }

    void ViewAnimator::onDirectorAnimationStarted(
        AnimationDirector* director, const Animator* animator)
    {
    }

    void ViewAnimator::onDirectorAnimationProgress(
        AnimationDirector* director, const Animator* animator)
    {
        auto p = owner_view_->animeParams();
        double new_value = animator->getCurValue();

        switch (animator->getId()) {
        case VIEW_ANIM_ALPHA:
            p->setAlpha(new_value);
            break;
        case VIEW_ANIM_SCALE_X:
            p->setScaleX(new_value);
            break;
        case VIEW_ANIM_SCALE_Y:
            p->setScaleY(new_value);
            break;
        case VIEW_ANIM_TRANSLATE_X:
            p->setTranslateX(new_value);
            break;
        case VIEW_ANIM_TRANSLATE_Y:
            p->setTranslateY(new_value);
            break;
        case VIEW_ANIM_ROTATE:
            p->setRotate(new_value);
            break;
        case VIEW_ANIM_RECT_REVEAL_R_X:
            p->reveal().calculate(owner_view_, new_value);
            break;
        case VIEW_ANIM_RECT_REVEAL_R_Y:
            p->reveal().calculate(owner_view_, new_value);
            break;
        case VIEW_ANIM_CIRCLE_REVEAL_R:
            p->reveal().calculate(owner_view_, new_value);
            break;
        default:
            break;
        }

        if (listener_) {
            listener_->onDirectorAnimationProgress(director, animator);
        }
    }

    void ViewAnimator::onDirectorAnimationStopped(
        AnimationDirector* director, const Animator* animator)
    {

    }

    void ViewAnimator::onDirectorAnimationFinished(
        AnimationDirector* director, const Animator* animator)
    {
        using namespace std::chrono_literals;
        if (animator->getId() == VIEW_ANIM_RECT_REVEAL_R_X ||
            animator->getId() == VIEW_ANIM_RECT_REVEAL_R_Y ||
            animator->getId() == VIEW_ANIM_CIRCLE_REVEAL_R)
        {
            bool has_reval = false;
            auto rev_anim = director->get(VIEW_ANIM_RECT_REVEAL_R_X, 0s);
            if (rev_anim) {
                has_reval |= rev_anim->isRunning();
            }
            if (!has_reval) {
                rev_anim = director->get(VIEW_ANIM_RECT_REVEAL_R_Y, 0s);
                if (rev_anim) {
                    has_reval |= rev_anim->isRunning();
                }
            }
            if (!has_reval) {
                rev_anim = director->get(VIEW_ANIM_CIRCLE_REVEAL_R, 0s);
                if (rev_anim) {
                    has_reval |= rev_anim->isRunning();
                }
            }

            if (!has_reval) {
                owner_view_->animeParams()->setHasReveal(false);
            }
        }
    }

    void ViewAnimator::onDirectorAnimationReset(
        AnimationDirector* director, const Animator* animator)
    {

    }

}