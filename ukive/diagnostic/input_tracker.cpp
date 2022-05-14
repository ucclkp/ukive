// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "input_tracker.h"

#include "ukive/event/input_event.h"
#include "utils/log.h"
#include "utils/time_utils.h"


namespace ukive {

    int InputTracker::indent_ = 0;
    std::string InputTracker::view_str_;
    std::stack<uint64_t> InputTracker::start_times_;

    void InputTracker::initTrack() {
        indent_ = 0;
    }

    void InputTracker::trackStart(
        const char* cls_name, const char* met_name)
    {
        std::string ind(indent_, ' ');

        auto start_time = utl::TimeUtils::upTimeMillis();
        start_times_.push(start_time);

        jour_i("%s%s::%s cur: %d", ind, cls_name, met_name, start_time);
        indent_ += 2;
    }

    void InputTracker::trackEnd() {
        auto end_time = utl::TimeUtils::upTimeMillis();
        ubassert(!start_times_.empty());
        auto start_time = start_times_.top();
        start_times_.pop();

        ubassert(indent_ > 0);
        indent_ -= 2;
        std::string ind(indent_, ' ');

        jour_i(
            "%send cur: %d, dur: %dms",
            ind, start_time, end_time - start_time);
    }

    void InputTracker::trackCur(const char* cls_name) {
        ubassert(!start_times_.empty());
        auto prev_time = start_times_.top();
        auto cur_time = utl::TimeUtils::upTimeMillis();

        std::string ind(indent_, ' ');
        LOG(Log::INFO) << ind << "<Cur (" << cls_name
            << ") duration: " << (cur_time - prev_time) << "ms";
    }

    // static
    void InputTracker::printInputEvent(const char* prefix, InputEvent* e) {
        ubassert(e);
        std::string ind(indent_, ' ');
        jour_i("%s=== %s Event: %s", ind, prefix, e->toString());
    }

}
