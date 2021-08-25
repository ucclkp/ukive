// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/animation_director.h"

#include <assert.h>

#include "animator.h"


namespace ukive {

    AnimationDirector::AnimationDirector()
        : listener_(nullptr) {}

    Anitom* AnimationDirector::add(int id) {
        return add(id, ns(0));
    }

    Anitom* AnimationDirector::add(int id, nsp st) {
        auto animator = std::make_unique<Anitom>();
        animator->setId(id);
        animator->setStartTime(st);

        auto ptr = animator.get();

        bool hit = false;
        auto& vec = channels_[id].anitoms;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if ((*it)->getStartTime() == st) {
                (*it) = std::move(animator);
                hit = true;
                break;
            }

            if ((*it)->getStartTime() > st) {
                vec.insert(it, std::move(animator));
                hit = true;
                break;
            }
        }

        if (!hit) {
            vec.push_back(std::move(animator));
        }

        return ptr;
    }

    void AnimationDirector::remove(int id) {
        auto it = channels_.find(id);
        if (it != channels_.end()) {
            channels_.erase(it);
        }
    }

    void AnimationDirector::remove(int id, nsp st) {
        auto mit = channels_.find(id);
        if (mit == channels_.end()) {
            return;
        }

        auto& vec = mit->second.anitoms;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if ((*it)->getStartTime() == st) {
                vec.erase(it);
                if (vec.empty()) {
                    channels_.erase(mit);
                }
                break;
            }
        }
    }

    void AnimationDirector::clear() {
        channels_.clear();
    }

    bool AnimationDirector::contains(int id) const {
        auto it = channels_.find(id);
        return it != channels_.end();
    }

    bool AnimationDirector::contains(int id, nsp st) const {
        auto mit = channels_.find(id);
        if (mit == channels_.end()) {
            return false;
        }

        auto& vec = mit->second.anitoms;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if ((*it)->getStartTime() == st) {
                return true;
            }
        }

        return false;
    }

    const AnimationDirector::Channel& AnimationDirector::get(int id) const {
        auto mit = channels_.find(id);
        if (mit == channels_.end()) {
            static Channel empty_ch;
            return empty_ch;
        }
        return mit->second;
    }

    Anitom* AnimationDirector::get(int id, nsp st) const {
        auto mit = channels_.find(id);
        if (mit == channels_.end()) {
            return nullptr;
        }

        auto& vec = mit->second.anitoms;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if ((*it)->getStartTime() == st) {
                return it->get();
            }
        }

        return nullptr;
    }

    void AnimationDirector::addLoop(nsp start, nsp duration, uint32_t count) {
        for (auto it = loops_.begin(); it != loops_.end(); ++it) {
            if (it->start == start.count()) {
                it->duration = duration.count();
                it->count = count;
                return;
            }

            if (it->start > start.count()) {
                LoopItem item;
                item.start = start.count();
                item.duration = duration.count();
                item.count = count;
                loops_.insert(it, item);
                return;
            }
        }

        LoopItem item;
        item.start = start.count();
        item.duration = duration.count();
        item.count = count;
        loops_.push_back(item);
    }

    void AnimationDirector::removeLoop(nsp start) {
        for (auto it = loops_.begin(); it != loops_.end(); ++it) {
            if (it->start == start.count()) {
                loops_.erase(it);
                return;
            }
        }
    }

    void AnimationDirector::removeLoops() {
        loops_.clear();
    }

    void AnimationDirector::start() {
        if (is_running_ || is_finished_ || channels_.empty()) {
            return;
        }

        if (!is_started_) {
            for (auto& pair : channels_) {
                pair.second.cur_val = pair.second.init_val;
            }
            is_started_ = true;
        }

        is_running_ = true;
        is_finished_ = false;
        start_time_ = Animator::now() - elapsed_time_;
        looped_time_ = 0;

        if (listener_) {
            listener_->onDirectorStarted(this);
        }
    }

    void AnimationDirector::stop() {
        if (!is_running_) {
            return;
        }

        is_running_ = false;
        elapsed_time_ = Animator::now() - start_time_ - looped_time_;

        for (auto& pair : channels_) {
            auto& channel = pair.second;

            for (auto& anim : channel.anitoms) {
                if (anim->isRunning()) {
                    anim->setRunning(false);
                    if (listener_) {
                        listener_->onDirectorAnitomStopped(this, anim.get());
                    }
                }
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

        for (auto& pair : channels_) {
            ns end_time;
            Anitom* end_a = nullptr;

            auto& vec = pair.second.anitoms;
            for (const auto& a : vec) {
                auto et = a->getEndTime();
                if (et > end_time) {
                    end_time = et;
                    end_a = a.get();
                }

                anitomToFinish(a.get());
            }

            if (end_a) {
                pair.second.cur_val = end_a->getFinalValue();
            }
        }

        if (listener_) {
            listener_->onDirectorFinished(this);
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

        for (auto& pair : channels_) {
            pair.second.cur_val = pair.second.init_val;
            for (auto& anim : pair.second.anitoms) {
                anitomToReset(anim.get());
            }
        }

        if (listener_) {
            listener_->onDirectorReset(this);
        }
    }

    bool AnimationDirector::update(uint64_t cur_time, uint32_t display_freq) {
        if (!is_running_) {
            return false;
        }

        bool in_loop = false;
        auto local_time = cur_time - start_time_ - looped_time_;

        if (cur_loop_.has_value()) {
            auto& loop = cur_loop_.value();
            if (loop.start <= local_time) {
                // 当前有正在进行的循环动画
                in_loop = true;
                auto loop_end_time = loop.start + loop.duration;
                if (local_time > loop_end_time) {
                    if (loop.count > 0) {
                        --loop.count;
                        looped_time_ = cur_time - (start_time_ + loop.start);
                    } else {
                        cur_loop_.reset();
                        in_loop = false;
                    }
                }
            } else {
                cur_loop_.reset();
            }
        }

        if (!in_loop) {
            for (auto& loop : loops_) {
                if (local_time >= loop.start &&
                    local_time < loop.start + loop.duration)
                {
                    cur_loop_ = loop;
                }
            }
        }

        bool finished = updateInternal(local_time);
        if (finished) {
            if (is_repeat_) {
                restart(cur_time);
            } else {
                finish();
            }
        }

        return true;
    }

    bool AnimationDirector::updateInternal(uint64_t local_time) {
        for (auto& pair : channels_) {
            auto& channel = pair.second;

            for (auto& anim : channel.anitoms) {
                updateAnitom(local_time, anim.get(), channel.cur_val);
            }
        }

        if (listener_) {
            listener_->onDirectorProgress(this);
        }

        bool finished = ns(local_time) >= getTotalDuration();
        return finished;
    }

    void AnimationDirector::updateAnitom(
        uint64_t local_time, Anitom* anim, double& cur_val)
    {
        if (anim->getEndTime() <= ns(local_time)) {
            anitomToFinish(anim);
            return;
        }

        if (anim->getStartTime() > ns(local_time)) {
            anitomToReset(anim);
            return;
        }

        if (anim->isFinished()) {
            anim->setStarted(false);
            anim->setRunning(false);
            anim->setFinished(false);

            if (listener_) {
                listener_->onDirectorAnitomReset(this, anim);
            }
        }

        if (!anim->isStarted() || !anim->isRunning()) {
            if (!anim->isStarted()) {
                anim->setInitValue(cur_val);
            }
            anim->setStarted(true);
            anim->setRunning(true);
            anim->setFinished(false);

            if (listener_) {
                listener_->onDirectorAnitomStarted(this, anim);
            }
        }

        anim->setRunning(true);
        cur_val = anim->getCurValue(ns(local_time));

        if (listener_) {
            listener_->onDirectorAnitomProgress(this, anim);
        }
    }

    Anitom* AnimationDirector::findCurAnitom(
        uint64_t local_time, const Anitoms& anitoms) const
    {
        for (auto& anim : anitoms) {
            if (anim->getStartTime() <= ns(local_time) &&
                anim->getEndTime() > ns(local_time))
            {
                return anim.get();
            }
        }
        return nullptr;
    }

    void AnimationDirector::anitomToFinish(Anitom* anim) {
        if (!anim->isFinished() && anim->isStarted()) {
            anim->setRunning(false);
            anim->setFinished(true);
            if (listener_) {
                listener_->onDirectorAnitomFinished(this, anim);
            }
        }
    }

    void AnimationDirector::anitomToReset(Anitom* anim) {
        if (anim->isStarted()) {
            anim->setStarted(false);
            anim->setRunning(false);
            anim->setFinished(false);

            if (listener_) {
                listener_->onDirectorAnitomReset(this, anim);
            }
        }
    }

    void AnimationDirector::restart(uint64_t cur_time) {
        for (auto& pair : channels_) {
            pair.second.cur_val = pair.second.init_val;
        }

        elapsed_time_ = 0;
        start_time_ = cur_time;
        looped_time_ = 0;
    }

    void AnimationDirector::setRepeat(bool repeat) {
        is_repeat_ = repeat;
    }

    void AnimationDirector::setInitValue(int id, double init_val) {
        channels_[id].init_val = init_val;
    }

    void AnimationDirector::setListener(AnimationDirectorListener* l) {
        listener_ = l;
    }

    bool AnimationDirector::isRepeat() const {
        return is_repeat_;
    }

    bool AnimationDirector::isRunning() const {
        return is_running_;
    }

    bool AnimationDirector::isFinished() const {
        return is_finished_;
    }

    double AnimationDirector::getCurValue(int id) const {
        auto mit = channels_.find(id);
        if (mit == channels_.end()) {
            return 0;
        }
        return mit->second.cur_val;
    }

    double AnimationDirector::getInitValue(int id) const {
        auto mit = channels_.find(id);
        if (mit == channels_.end()) {
            return 0;
        }
        return mit->second.init_val;
    }

    AnimationDirector::ns AnimationDirector::getDuration(int id) const {
        auto mit = channels_.find(id);
        if (mit == channels_.end()) {
            return ns(0);
        }

        ns duration(0);
        auto& vec = mit->second.anitoms;
        for (auto& anim : vec) {
            auto cur = anim->getEndTime();
            if (cur > duration) {
                duration = cur;
            }
        }

        return duration;
    }

    AnimationDirector::ns AnimationDirector::getTotalDuration() const {
        ns total(0);
        for (auto& pair : channels_) {
            auto& vec = pair.second.anitoms;
            for (auto& anim : vec) {
                auto cur = anim->getEndTime();
                if (cur > total) {
                    total = cur;
                }
            }
        }

        return total;
    }

}
