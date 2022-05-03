// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/animation_director.h"

#include "animator.h"


namespace {

    template <typename Ty>
    Ty uns_sub(Ty lhs, Ty rhs) {
        return (lhs >= rhs) ? (lhs - rhs) : 0;
    }

}

namespace ukive {

    AnimationDirector::AnimationDirector()
        : listener_(nullptr) {}

    Anitom* AnimationDirector::add(int id) {
        return add(id, ns(0));
    }

    Anitom* AnimationDirector::add(int id, bool continuous) {
        ns start_time;
        if (continuous && is_started_ && !is_finished_) {
            start_time = ns(Animator::now() - start_time_);
        } else {
            start_time = ns(0);
        }

        return add(id, start_time);
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

        if (is_started_ && !is_finished_ && vec.size() == 1) {
            channels_[id].cur_val = channels_[id].init_val;
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

    void AnimationDirector::removeFinished() {
        for (auto it = channels_.begin(); it != channels_.end();) {
            auto& vec = it->second.anitoms;
            for (auto ia = vec.begin(); ia != vec.end();) {
                if (ia->get()->isFinished()) {
                    ia = vec.erase(ia);
                } else {
                    ++ia;
                }
            }

            if (vec.empty()) {
                it = channels_.erase(it);
            } else {
                ++it;
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
        if (duration.count() <= 0 || count == 0) {
            return;
        }

        for (size_t i = 0; i < loops_.size(); ++i) {
            auto& loop = loops_[i];
            if (loop.start == start.count()) {
                loop.duration = duration.count();
                loop.count = count;
                return;
            }

            if (loop.start > start.count()) {
                LoopItem item;
                item.start = start.count();
                item.duration = duration.count();
                item.count = count;
                item.is_finished = false;
                loops_.insert(loops_.begin() + i, item);

                if (cur_loop_idx_ != npos) {
                    if (i <= cur_loop_idx_) {
                        ++cur_loop_idx_;
                    }
                }
                return;
            }
        }

        LoopItem item;
        item.start = start.count();
        item.duration = duration.count();
        item.count = count;
        item.is_finished = false;
        loops_.push_back(item);
    }

    void AnimationDirector::removeLoop(nsp start) {
        for (size_t i = 0; i < loops_.size(); ++i) {
            if (loops_[i].start == start.count()) {
                loops_.erase(loops_.begin() + i);

                if (cur_loop_idx_ != npos) {
                    if (i == cur_loop_idx_) {
                        cur_loop_idx_ = npos;
                    } else if (i < cur_loop_idx_) {
                        --cur_loop_idx_;
                    }
                }
                return;
            }
        }
    }

    void AnimationDirector::removeLoops() {
        loops_.clear();
        cur_loop_idx_ = npos;
    }

    void AnimationDirector::start() {
        if (is_running_ || channels_.empty()) {
            return;
        }

        if (is_finished_) {
            reset();
        }

        if (!is_started_) {
            for (auto& pair : channels_) {
                pair.second.cur_val = pair.second.init_val;
            }
            is_started_ = true;
        }

        is_running_ = true;
        is_finished_ = false;
        is_preparing_ = true;
        start_time_ = 0;

        if (listener_) {
            listener_->onDirectorStarted(this);
        }
    }

    void AnimationDirector::stop() {
        if (!is_running_) {
            return;
        }

        is_running_ = false;
        if (!is_preparing_) {
            elapsed_time_ = uns_sub(Animator::now(), start_time_);
        }

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
        is_preparing_ = false;

        for (auto& loop : loops_) {
            loop.is_finished = true;
        }
        cur_loop_idx_ = npos;

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

                if (!a->isFinished() && a->isStarted()) {
                    anitomToFinish(a.get());
                }
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
        is_preparing_ = false;
        elapsed_time_ = 0;
        looped_time_ = 0;

        for (auto& loop : loops_) {
            loop.is_finished = false;
        }
        cur_loop_idx_ = npos;

        for (auto& pair : channels_) {
            pair.second.cur_val = pair.second.init_val;
            for (auto& anim : pair.second.anitoms) {
                if (anim->isStarted()) {
                    anitomToReset(anim.get());
                }
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

        if (is_preparing_) {
            is_preparing_ = false;
            start_time_ = uns_sub(cur_time, (std::nano::den / display_freq) + elapsed_time_);
        }

        // 计算已经结束的循环所花的时间
        uint64_t prev_loop_time = 0;
        for (size_t i = 0; i < loops_.size(); ++i) {
            const auto& loop = loops_[i];
            if (loop.is_finished) {
                prev_loop_time += loop.duration * loop.count;
            }
        }

        auto local_time = uns_sub(cur_time, start_time_ + prev_loop_time);
        uint64_t cur_loop_time = 0;

        // 选定下一个循环
        if (cur_loop_idx_ == npos) {
            for (size_t i = 0; i < loops_.size(); ++i) {
                const auto& loop = loops_[i];
                if (local_time >= loop.start && !loop.is_finished) {
                    cur_loop_idx_ = i;
                    break;
                }
            }
        }

        if (cur_loop_idx_ != npos) {
            auto& loop = loops_[cur_loop_idx_];
            auto elapsed_loop_count = uint32_t(uns_sub(local_time, loop.start) / loop.duration);
            if (elapsed_loop_count > loop.count) {
                loop.is_finished = true;
                cur_loop_idx_ = npos;
                cur_loop_time = loop.count * loop.duration;
            } else {
                cur_loop_time = elapsed_loop_count * loop.duration;
            }

            local_time = uns_sub(local_time, cur_loop_time);
        }

        looped_time_ = prev_loop_time + cur_loop_time;

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
        if (anim->isInevitable()) {
            updateAnitomP1(local_time, anim, cur_val);
            updateAnitomP2(local_time, anim, cur_val);
        } else {
            if (!updateAnitomP2(local_time, anim, cur_val)) {
                return;
            }
            updateAnitomP1(local_time, anim, cur_val);
        }
    }

    void AnimationDirector::updateAnitomP1(
        uint64_t local_time, Anitom* anim, double& cur_val)
    {
        if (!anim->isStarted() || !anim->isRunning()) {
            anitomToStart(anim, cur_val);
        }

        anim->setRunning(true);
        cur_val = anim->getCurValue(ns(local_time));

        if (listener_) {
            listener_->onDirectorAnitomProgress(this, anim);
        }
    }

    bool AnimationDirector::updateAnitomP2(
        uint64_t local_time, Anitom* anim, double& cur_val)
    {
        if (anim->getEndTime() <= ns(local_time)) {
            cur_val = anim->getCurValue(ns(local_time));
            if (!anim->isFinished() && anim->isStarted()) {
                anitomToFinish(anim);
            }
            return false;
        }

        if (anim->getStartTime() > ns(local_time)) {
            if (anim->isStarted()) {
                anitomToReset(anim);
            }
            return false;
        }

        if (anim->isFinished()) {
            anitomToReset(anim);
        }
        return true;
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

    void AnimationDirector::anitomToStart(Anitom* anim, double cur_val) {
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

    void AnimationDirector::anitomToFinish(Anitom* anim) {
        anim->setRunning(false);
        anim->setFinished(true);
        if (listener_) {
            listener_->onDirectorAnitomFinished(this, anim);
        }
    }

    void AnimationDirector::anitomToReset(Anitom* anim) {
        anim->setStarted(false);
        anim->setRunning(false);
        anim->setFinished(false);

        if (listener_) {
            listener_->onDirectorAnitomReset(this, anim);
        }
    }

    void AnimationDirector::restart(uint64_t cur_time) {
        for (auto& pair : channels_) {
            pair.second.cur_val = pair.second.init_val;
        }

        for (auto& loop : loops_) {
            loop.is_finished = true;
        }
        cur_loop_idx_ = npos;

        elapsed_time_ = 0;
        looped_time_ = 0;
        start_time_ = cur_time;
    }

    void AnimationDirector::setRepeat(bool repeat) {
        is_repeat_ = repeat;
    }

    void AnimationDirector::setInitValue(int id, double init_val) {
        auto& pair = channels_[id];
        pair.init_val = init_val;

        if (is_started_ && !is_finished_) {
            ns start_time;
            bool first = true;
            auto& vec = pair.anitoms;
            for (auto& a : vec) {
                if (first) {
                    start_time = a->getStartTime();
                    first = false;
                } else {
                    auto st = a->getStartTime();
                    if (st > start_time) {
                        break;
                    }
                }
                a->setInitValue(init_val);
            }

            if (vec.size() == 1 && !vec[0]->isStarted()) {
                pair.cur_val = pair.init_val;
            }
        }
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
