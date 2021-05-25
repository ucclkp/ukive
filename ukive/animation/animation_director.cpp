// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/animation_director.h"

#include "ukive/animation/interpolator.h"


namespace ukive {

    AnimationDirector::AnimationDirector()
        : listener_(nullptr) {}

    void AnimationDirector::addAnimator(int id) {
        auto animator = std::make_unique<Animator>();
        animator->setId(id);
        animator->setListener(this);
        animators_[id] = std::move(animator);
    }

    void AnimationDirector::removeAnimator(int id) {
        auto it = animators_.find(id);
        if (it != animators_.end()) {
            animators_.erase(it);
        }
    }

    bool AnimationDirector::hasAnimator(int id) const {
        auto it = animators_.find(id);
        return it != animators_.end();
    }

    const Animator* AnimationDirector::getAnimator(int id) const {
        auto it = animators_.find(id);
        if (it == animators_.end()) {
            return nullptr;
        }
        return it->second.get();
    }

    void AnimationDirector::start() {
        for (auto& pair : animators_) {
            pair.second->start();
        }

        if (listener_) {
            listener_->onDirectorStarted(this, nullptr);
        }
    }

    void AnimationDirector::stop() {
        for (auto& pair : animators_) {
            pair.second->stop();
        }

        if (listener_) {
            listener_->onDirectorStopped(this, nullptr);
        }
    }

    void AnimationDirector::finish() {
        for (auto& pair : animators_) {
            pair.second->finish();
        }
    }

    void AnimationDirector::reset() {
        for (auto& pair : animators_) {
            pair.second->reset();
        }

        if (listener_) {
            listener_->onDirectorReset(this, nullptr);
        }
    }

    void AnimationDirector::update(uint64_t cur_time, uint32_t display_freq) {
        for (auto& pair : animators_) {
            pair.second->update(cur_time, display_freq);
        }
    }

    void AnimationDirector::setDuration(int id, Animator::nsp duration) {
        auto it = animators_.find(id);
        if (it != animators_.end()) {
            it->second->setDuration(duration);
        }
    }

    void AnimationDirector::setInitValue(int id, double init_val) {
        auto it = animators_.find(id);
        if (it != animators_.end()) {
            it->second->setInitValue(init_val);
        }
    }

    void AnimationDirector::setInterpolator(int id, Interpolator* ipr) {
        auto it = animators_.find(id);
        if (it != animators_.end()) {
            it->second->setInterpolator(ipr);
        } else {
            delete ipr;
        }
    }

    void AnimationDirector::setListener(AnimationDirectorListener* l) {
        listener_ = l;
    }

    bool AnimationDirector::isRunning() const {
        for (auto& pair : animators_) {
            if (pair.second->isRunning()) {
                return true;
            }
        }
        return false;
    }

    bool AnimationDirector::isFinished() const {
        bool all_finished = true;
        for (auto& pair : animators_) {
            all_finished &= pair.second->isFinished();
        }
        return all_finished;
    }

    Animator::ns AnimationDirector::getDuration(int id) const {
        auto it = animators_.find(id);
        if (it != animators_.end()) {
            return it->second->getDuration();
        }
        return Animator::ns(0);
    }

    double AnimationDirector::getCurValue(int id) const {
        auto it = animators_.find(id);
        if (it != animators_.end()) {
            return it->second->getCurValue();
        }
        return 0;
    }

    double AnimationDirector::getInitValue(int id) const {
        auto it = animators_.find(id);
        if (it != animators_.end()) {
            return it->second->getInitValue();
        }
        return 0;
    }

    Interpolator* AnimationDirector::getInterpolator(int id) const {
        auto it = animators_.find(id);
        if (it != animators_.end()) {
            return it->second->getInterpolator();
        }
        return nullptr;
    }

    void AnimationDirector::onAnimationStarted(Animator* animator) {
        if (listener_) {
            listener_->onDirectorStarted(this, animator);
        }
    }

    void AnimationDirector::onAnimationProgress(Animator* animator) {
        if (listener_) {
            listener_->onDirectorProgress(this, animator);
        }
    }

    void AnimationDirector::onAnimationStopped(Animator* animator) {
        if (listener_) {
            listener_->onDirectorStopped(this, animator);
        }
    }

    void AnimationDirector::onAnimationFinished(Animator* animator) {
        if (listener_) {
            listener_->onDirectorFinished(this, animator);
        }

        if (isFinished()) {
            if (listener_) {
                listener_->onDirectorFinished(this, nullptr);
            }
        }
    }

    void AnimationDirector::onAnimationReset(Animator* animator) {
        if (listener_) {
            listener_->onDirectorReset(this, animator);
        }
    }

}