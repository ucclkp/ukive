// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_
#define UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_

#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "ukive/animation/anitom.h"


namespace ukive {

    class AnimationDirector;

    class AnimationDirectorListener {
    public:
        virtual ~AnimationDirectorListener() = default;

        virtual void onDirectorStarted(AnimationDirector* director) {}
        virtual void onDirectorProgress(AnimationDirector* director) {}
        virtual void onDirectorStopped(AnimationDirector* director) {}
        virtual void onDirectorFinished(AnimationDirector* director) {}
        virtual void onDirectorReset(AnimationDirector* director) {}

        virtual void onDirectorAnitomStarted(
            AnimationDirector* director, const Anitom* anitom) {}
        virtual void onDirectorAnitomProgress(
            AnimationDirector* director, const Anitom* anitom) {}
        virtual void onDirectorAnitomStopped(
            AnimationDirector* director, const Anitom* anitom) {}
        virtual void onDirectorAnitomFinished(
            AnimationDirector* director, const Anitom* anitom) {}
        virtual void onDirectorAnitomReset(
            AnimationDirector* director, const Anitom* anitom) {}
    };

    class AnimationDirector {
    public:
        using ns = Anitom::ns;
        using nsp = Anitom::nsp;
        using Anitoms = std::vector<std::unique_ptr<Anitom>>;

        struct Channel {
            Anitoms anitoms;
            double init_val = 0;
            double cur_val = 0;
        };

        AnimationDirector();

        Anitom* add(int id);
        Anitom* add(int id, bool continuous);
        Anitom* add(int id, nsp st);
        void remove(int id);
        void remove(int id, nsp st);
        void removeFinished();
        void clear();
        bool contains(int id) const;
        bool contains(int id, nsp st) const;
        const Channel& get(int id) const;
        Anitom* get(int id, nsp st) const;

        void addLoop(nsp start, nsp duration, uint32_t count);
        void removeLoop(nsp start);
        void removeLoops();

        void start();
        void stop();
        void finish();
        void reset();
        bool update(uint64_t cur_time, uint32_t display_freq);

        void setRepeat(bool repeat);
        void setInitValue(int id, double init_val);
        void setListener(AnimationDirectorListener* l);

        bool isRepeat() const;
        bool isRunning() const;
        bool isFinished() const;

        double getCurValue(int id) const;
        double getInitValue(int id) const;
        ns getDuration(int id) const;
        ns getTotalDuration() const;

    private:
        static constexpr auto npos = (std::numeric_limits<size_t>::max)();

        struct LoopItem {
            uint64_t start;
            uint64_t duration;
            uint32_t count;
            bool is_finished;
        };

        Anitom* findCurAnitom(
            uint64_t local_time, const Anitoms& anitoms) const;
        void anitomToStart(Anitom* anim, double cur_val);
        void anitomToFinish(Anitom* anim);
        void anitomToReset(Anitom* anim);

        bool updateInternal(uint64_t local_time);
        void updateAnitom(uint64_t local_time, Anitom* anim, double& cur_val);
        void updateAnitomP1(uint64_t local_time, Anitom* anim, double& cur_val);
        bool updateAnitomP2(uint64_t local_time, Anitom* anim, double& cur_val);
        void restart(uint64_t cur_time);

        uint64_t start_time_ = 0;
        uint64_t elapsed_time_ = 0;
        uint64_t looped_time_ = 0;
        size_t cur_loop_idx_ = npos;

        bool is_repeat_ = false;
        bool is_started_ = false;
        bool is_running_ = false;
        bool is_finished_ = false;
        bool is_preparing_ = false;
        AnimationDirectorListener* listener_;
        std::map<int, Channel> channels_;
        std::vector<LoopItem> loops_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_