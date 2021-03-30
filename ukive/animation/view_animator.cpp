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
        : duration_(200),
          owner_view_(v),
          listener_(nullptr)
    {
        director_.setListener(this);
    }

    ViewAnimator::~ViewAnimator() {
    }

    void ViewAnimator::start() {
        director_.start();
        owner_view_->requestDraw();
    }

    void ViewAnimator::cancel() {
        director_.stop();
    }

    ViewAnimator* ViewAnimator::setDuration(uint64_t duration) {
        duration_ = duration;
        return this;
    }

    ViewAnimator* ViewAnimator::alpha(double value) {
        director_.addAnimator(VIEW_ANIM_ALPHA);
        director_.setInitValue(VIEW_ANIM_ALPHA, owner_view_->animeParams()->getAlpha());
        director_.setDuration(VIEW_ANIM_ALPHA, duration_);
        director_.setInterpolator(VIEW_ANIM_ALPHA, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::scaleX(double value) {
        director_.addAnimator(VIEW_ANIM_SCALE_X);
        director_.setInitValue(VIEW_ANIM_SCALE_X, owner_view_->animeParams()->getScaleX());
        director_.setDuration(VIEW_ANIM_SCALE_X, duration_);
        director_.setInterpolator(VIEW_ANIM_SCALE_X, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::scaleY(double value) {
        director_.addAnimator(VIEW_ANIM_SCALE_Y);
        director_.setInitValue(VIEW_ANIM_SCALE_Y, owner_view_->animeParams()->getScaleY());
        director_.setDuration(VIEW_ANIM_SCALE_Y, duration_);
        director_.setInterpolator(VIEW_ANIM_SCALE_Y, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::rotate(double value) {
        director_.addAnimator(VIEW_ANIM_ROTATE);
        director_.setInitValue(VIEW_ANIM_ROTATE, owner_view_->animeParams()->getRotate());
        director_.setDuration(VIEW_ANIM_ROTATE, duration_);
        director_.setInterpolator(VIEW_ANIM_ROTATE, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::translateX(double value) {
        director_.addAnimator(VIEW_ANIM_TRANSLATE_X);
        director_.setInitValue(VIEW_ANIM_TRANSLATE_X, owner_view_->animeParams()->getTranslateX());
        director_.setDuration(VIEW_ANIM_TRANSLATE_X, duration_);
        director_.setInterpolator(VIEW_ANIM_TRANSLATE_X, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::translateY(double value) {
        director_.addAnimator(VIEW_ANIM_TRANSLATE_Y);
        director_.setInitValue(VIEW_ANIM_TRANSLATE_Y, owner_view_->animeParams()->getTranslateY());
        director_.setDuration(VIEW_ANIM_TRANSLATE_Y, duration_);
        director_.setInterpolator(VIEW_ANIM_TRANSLATE_Y, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::rectReveal(
        double center_x, double center_y,
        double start_hori_radius, double end_hori_radius,
        double start_vert_radius, double end_vert_radius)
    {
        director_.addAnimator(VIEW_ANIM_RECT_REVEAL_R_X);
        director_.setInitValue(VIEW_ANIM_RECT_REVEAL_R_X, start_hori_radius);
        director_.setDuration(VIEW_ANIM_RECT_REVEAL_R_X, duration_);
        director_.setInterpolator(VIEW_ANIM_RECT_REVEAL_R_X, new LinearInterpolator(end_hori_radius));

        director_.addAnimator(VIEW_ANIM_RECT_REVEAL_R_Y);
        director_.addAnimator(VIEW_ANIM_RECT_REVEAL_R_Y);
        director_.setInitValue(VIEW_ANIM_RECT_REVEAL_R_Y, start_vert_radius);
        director_.setDuration(VIEW_ANIM_RECT_REVEAL_R_Y, duration_);
        director_.setInterpolator(VIEW_ANIM_RECT_REVEAL_R_Y, new LinearInterpolator(end_vert_radius));

        owner_view_->animeParams()->setHasReveal(true);
        owner_view_->animeParams()->setRevealType(REVEAL_RECT);
        owner_view_->animeParams()->setRevealCenterX(center_x);
        owner_view_->animeParams()->setRevealCenterY(center_y);
        owner_view_->animeParams()->setRevealWidthRadius(start_hori_radius);
        owner_view_->animeParams()->setRevealHeightRadius(start_vert_radius);

        return this;
    }

    ViewAnimator* ViewAnimator::circleReveal(
        double center_x, double center_y, double start_radius, double end_radius)
    {
        director_.addAnimator(VIEW_ANIM_CIRCLE_REVEAL_R);
        director_.setInitValue(VIEW_ANIM_CIRCLE_REVEAL_R, start_radius);
        director_.setDuration(VIEW_ANIM_CIRCLE_REVEAL_R, duration_);
        director_.setInterpolator(VIEW_ANIM_CIRCLE_REVEAL_R, new LinearInterpolator(end_radius));

        owner_view_->animeParams()->setHasReveal(true);
        owner_view_->animeParams()->setRevealType(REVEAL_CIRCULE);
        owner_view_->animeParams()->setRevealCenterX(center_x);
        owner_view_->animeParams()->setRevealCenterY(center_y);
        owner_view_->animeParams()->setRevealRadius(start_radius);

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

    void ViewAnimator::onPreViewDraw() {
        director_.update();
    }

    void ViewAnimator::onPostViewDraw() {
        if (director_.isRunning()) {
            owner_view_->requestDraw();
        }
    }

    void ViewAnimator::onDirectorStarted(AnimationDirector* director, const Animator* animator) {
        if (listener_) {
            listener_->onDirectorStarted(director, animator);
        }
    }

    void ViewAnimator::onDirectorProgress(AnimationDirector* director, const Animator* animator) {
        double new_value = animator->getCurValue();

        switch (animator->getId()) {
        case VIEW_ANIM_ALPHA:
            owner_view_->animeParams()->setAlpha(new_value);
            break;
        case VIEW_ANIM_SCALE_X:
            owner_view_->animeParams()->setScaleX(new_value);
            break;
        case VIEW_ANIM_SCALE_Y:
            owner_view_->animeParams()->setScaleY(new_value);
            break;
        case VIEW_ANIM_TRANSLATE_X:
            owner_view_->animeParams()->setTranslateX(new_value);
            break;
        case VIEW_ANIM_TRANSLATE_Y:
            owner_view_->animeParams()->setTranslateY(new_value);
            break;
        case VIEW_ANIM_ROTATE:
            owner_view_->animeParams()->setRotate(new_value);
            break;
        case VIEW_ANIM_RECT_REVEAL_R_X:
            owner_view_->animeParams()->setRevealWidthRadius(new_value);
            break;
        case VIEW_ANIM_RECT_REVEAL_R_Y:
            owner_view_->animeParams()->setRevealHeightRadius(new_value);
            break;
        case VIEW_ANIM_CIRCLE_REVEAL_R:
            owner_view_->animeParams()->setRevealRadius(new_value);
            break;
        default:
            break;
        }

        if (listener_) {
            listener_->onDirectorProgress(director, animator);
        }
    }

    void ViewAnimator::onDirectorStopped(AnimationDirector* director, const Animator* animator) {
        if (listener_) {
            listener_->onDirectorStopped(director, animator);
        }
    }

    void ViewAnimator::onDirectorFinished(AnimationDirector* director, const Animator* animator) {
        if (animator &&
            (animator->getId() == VIEW_ANIM_RECT_REVEAL_R_X ||
                animator->getId() == VIEW_ANIM_RECT_REVEAL_R_Y ||
                animator->getId() == VIEW_ANIM_CIRCLE_REVEAL_R))
        {
            bool has_reval = false;
            auto rev_anim = director->getAnimator(VIEW_ANIM_RECT_REVEAL_R_X);
            if (rev_anim) {
                has_reval |= rev_anim->isRunning();
            }
            if (!has_reval) {
                rev_anim = director->getAnimator(VIEW_ANIM_RECT_REVEAL_R_Y);
                if (rev_anim) {
                    has_reval |= rev_anim->isRunning();
                }
            }
            if (!has_reval) {
                rev_anim = director->getAnimator(VIEW_ANIM_CIRCLE_REVEAL_R);
                if (rev_anim) {
                    has_reval |= rev_anim->isRunning();
                }
            }

            if (!has_reval) {
                owner_view_->animeParams()->setHasReveal(false);
            }
        }

        if (listener_) {
            listener_->onDirectorFinished(director, animator);
        }

        if (!animator && finished_handler_) {
            finished_handler_(director);
        }
    }

    void ViewAnimator::onDirectorReset(AnimationDirector* director, const Animator* animator) {
        if (listener_) {
            listener_->onDirectorReset(director, animator);
        }
    }

}