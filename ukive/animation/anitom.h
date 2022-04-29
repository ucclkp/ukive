// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_ANITOM_H_
#define UKIVE_ANIMATION_ANITOM_H_

#include <memory>

#include "utils/time_utils.h"


namespace ukive {

    class Interpolator;
    class AnimationDirector;

    class Anitom {
    public:
        using ns = utl::TimeUtils::ns;
        using nsp = utl::TimeUtils::nsp;

        Anitom();
        ~Anitom();

        void setInevitable(bool inevitable);
        void setDuration(nsp duration);
        void setFinalValue(double val);
        void setInterpolator(Interpolator* i);

        int getId() const;
        ns getStartTime() const;
        ns getEndTime() const;
        ns getDuration() const;
        double getInitValue() const;
        double getFinalValue() const;
        const Interpolator* getInterpolator() const;

        bool isStarted() const;
        bool isRunning() const;
        bool isFinished() const;
        bool isInevitable() const;

        double getCurValue(nsp cur_time) const;

    private:
        friend class AnimationDirector;

        void setId(int id);
        void setStartTime(nsp start);
        void setStarted(bool started);
        void setRunning(bool running);
        void setFinished(bool finished);
        void setInitValue(double val);

        bool is_started_ = false;
        bool is_running_ = false;
        bool is_finished_ = false;

        int id_ = 0;
        ns start_time_;
        ns duration_;
        double init_val_ = 0;
        double final_val_ = 1;
        bool is_inevitable_ = true;
        std::unique_ptr<Interpolator> interpolator_;
    };

}

#endif  // UKIVE_ANIMATION_ANITOM_H_