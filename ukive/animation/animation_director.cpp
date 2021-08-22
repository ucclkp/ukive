// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/animation_director.h"


namespace ukive {

    AnimationDirector::AnimationDirector()
        : listener_(nullptr) {}

    Animator* AnimationDirector::add(int id) {
        return add(id, ns(0));
    }

    Animator* AnimationDirector::add(int id, nsp st) {
        auto animator = std::make_unique<Animator>();
        animator->setId(id);
        animator->setListener(this);

        auto ptr = animator.get();

        bool hit = false;
        auto& vec = animators_[id];
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if (it->st == st) {
                it->ptr = std::move(animator);
                hit = true;
                break;
            }

            if (it->st > st) {
                vec.insert(it, { st, std::move(animator) });
                hit = true;
                break;
            }
        }

        if (!hit) {
            vec.push_back({ st, std::move(animator) });
        }

        return ptr;
    }

    void AnimationDirector::remove(int id) {
        auto it = animators_.find(id);
        if (it != animators_.end()) {
            animators_.erase(it);
        }
    }

    void AnimationDirector::remove(int id, nsp st) {
        auto mit = animators_.find(id);
        if (mit == animators_.end()) {
            return;
        }

        auto& vec = mit->second;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if (it->st == st) {
                vec.erase(it);
                if (vec.empty()) {
                    animators_.erase(mit);
                }
                break;
            }
        }
    }

    void AnimationDirector::clear() {
        animators_.clear();
    }

    bool AnimationDirector::contains(int id) const {
        auto it = animators_.find(id);
        return it != animators_.end();
    }

    bool AnimationDirector::contains(int id, nsp st) const {
        auto mit = animators_.find(id);
        if (mit == animators_.end()) {
            return false;
        }

        auto& vec = mit->second;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if (it->st == st) {
                return true;
            }
        }

        return false;
    }

    const AnimationDirector::APtrVec& AnimationDirector::get(int id) const {
        auto mit = animators_.find(id);
        if (mit == animators_.end()) {
            static APtrVec empty_vec;
            return empty_vec;
        }
        return mit->second;
    }

    Animator* AnimationDirector::get(int id, nsp st) const {
        auto mit = animators_.find(id);
        if (mit == animators_.end()) {
            return nullptr;
        }

        auto& vec = mit->second;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if (it->st == st) {
                return it->ptr.get();
            }
        }

        return nullptr;
    }

    void AnimationDirector::start() {
        if (is_running_ || is_finished_ || animators_.empty()) {
            return;
        }

        is_started_ = true;
        is_running_ = true;
        is_finished_ = false;
        start_time_ = Animator::now();

        if (listener_) {
            listener_->onDirectorStarted(this);
        }

        for (auto& pair : animators_) {
            auto& vec = pair.second;
            for (auto& anim : vec) {
                if (anim.st <= ns(elapsed_time_)) {
                    anim.ptr->start();
                }
            }
        }
    }

    void AnimationDirector::stop() {
        if (!is_running_) {
            return;
        }

        is_running_ = false;
        elapsed_time_ = Animator::now() - start_time_;

        for (auto& pair : animators_) {
            auto& vec = pair.second;
            for (auto& anim : vec) {
                anim.ptr->stop();
            }
        }

        if (listener_) {
            listener_->onDirectorStopped(this);
        }
    }

    void AnimationDirector::finish() {
        if (!is_started_ || is_finished_) {
            return;
        }

        is_running_ = false;
        is_finished_ = true;

        for (auto& pair : animators_) {
            auto& vec = pair.second;
            for (auto& anim : vec) {
                anim.ptr->finish();
            }
        }
    }

    void AnimationDirector::reset() {
        if (!is_started_) {
            return;
        }

        is_started_ = false;
        is_running_ = false;
        is_finished_ = false;

        elapsed_time_ = 0;

        for (auto& pair : animators_) {
            auto& vec = pair.second;
            for (auto& anim : vec) {
                anim.ptr->reset();
            }
        }

        if (listener_) {
            listener_->onDirectorReset(this);
        }
    }

    void AnimationDirector::update(uint64_t cur_time, uint32_t display_freq) {
        if (!is_running_) {
            return;
        }

        auto elapsed = cur_time - start_time_;

        bool finished = true;
        for (auto& pair : animators_) {
            bool is_finished = true;
            double cur_val = 0;
            auto& vec = pair.second;
            for (auto& anim : vec) {
                if (anim.st <= ns(elapsed)) {
                    if (!anim.ptr->isStarted()) {
                        anim.ptr->setInitValue(cur_val);
                        anim.ptr->start();
                    } else {
                        cur_val = anim.ptr->getCurValue();
                    }
                }
                if (anim.ptr->update(cur_time, display_freq) || anim.st >= ns(elapsed)) {
                    is_finished = false;
                }
            }
            finished &= is_finished;
        }

        if (listener_) {
            listener_->onDirectorProgress(this);
        }

        if (finished) {
            if (is_repeat_) {
                restart(cur_time);
            } else {
                finish();
            }
        }
    }

    void AnimationDirector::restart(uint64_t cur_time) {
        elapsed_time_ = 0;
        start_time_ = cur_time;
    }

    void AnimationDirector::setListener(AnimationDirectorListener* l) {
        listener_ = l;
    }

    bool AnimationDirector::isRunning() const {
        return is_running_;
    }

    bool AnimationDirector::isFinished() const {
        return is_finished_;
    }

    double AnimationDirector::getCurValue(int id) const {
        auto mit = animators_.find(id);
        if (mit == animators_.end()) {
            return 0;
        }

        double val = 0;
        auto& vec = mit->second;
        for (auto& anim : vec) {
            if (anim.ptr->isStarted()) {
                val = anim.ptr->getCurValue();
            }
        }

        return val;
    }

    double AnimationDirector::getInitValue(int id) const {
        auto mit = animators_.find(id);
        if (mit == animators_.end()) {
            return 0;
        }

        auto& vec = mit->second;
        if (vec.empty()) {
            return 0;
        }

        return vec.front().ptr->getInitValue();
    }

    AnimationDirector::ns AnimationDirector::getDuration(int id) const {
        auto mit = animators_.find(id);
        if (mit == animators_.end()) {
            return ns(0);
        }

        ns duration(0);
        auto& vec = mit->second;
        for (auto& anim : vec) {
            auto cur = anim.st + anim.ptr->getDuration();
            if (cur > duration) {
                duration = cur;
            }
        }

        return duration;
    }

    AnimationDirector::ns AnimationDirector::getTotalDuration() const {
        ns total(0);
        for (auto& pair : animators_) {
            auto& vec = pair.second;
            for (auto& anim : vec) {
                auto cur = anim.st + anim.ptr->getDuration();
                if (cur > total) {
                    total = cur;
                }
            }
        }

        return total;
    }

    void AnimationDirector::onAnimationStarted(Animator* animator) {
        if (listener_) {
            listener_->onDirectorAnimationStarted(this, animator);
        }
    }

    void AnimationDirector::onAnimationProgress(Animator* animator) {
        if (listener_) {
            listener_->onDirectorAnimationProgress(this, animator);
        }
    }

    void AnimationDirector::onAnimationStopped(Animator* animator) {
        if (listener_) {
            listener_->onDirectorAnimationStopped(this, animator);
        }
    }

    void AnimationDirector::onAnimationFinished(Animator* animator) {
        if (listener_) {
            listener_->onDirectorAnimationFinished(this, animator);
        }

        if (isFinished()) {
            if (listener_) {
                listener_->onDirectorFinished(this);
            }
        }
    }

    void AnimationDirector::onAnimationReset(Animator* animator) {
        if (listener_) {
            listener_->onDirectorAnimationReset(this, animator);
        }
    }

}