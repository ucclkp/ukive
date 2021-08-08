// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_TIMER_H_
#define UKIVE_ANIMATION_TIMER_H_

#include <functional>

#include "utils/message/cycler.h"
#include "utils/weak_ref_nest.hpp"


namespace ukive {

    class Timer {
    public:
        using ns = utl::Cycler::ns;
        using nsp = utl::Cycler::nsp;
        using Runner = std::function<void()>;

        Timer();
        ~Timer();

        void start();
        void stop();

        void setRepeat(bool repeat);
        void setRunner(const Runner& runner);
        void setDuration(nsp duration);

        bool isRepeat() const;
        bool isRunning() const;
        nsp getDuration() const;

    private:
        void onTimer();
        void postToMessageLoop();

        ns duration_;
        bool is_repeat_;
        bool is_running_;
        Runner runner_;

        utl::Cycler cycler_;
        utl::WeakRefNest<Timer> weak_ref_nest_;
    };

}

#endif  // UKIVE_ANIMATION_TIMER_H_