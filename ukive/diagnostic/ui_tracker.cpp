// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ui_tracker.h"

#include "utils/log.h"
#include "utils/time_utils.h"


namespace ukive {

    int UITracker::indent_ = 0;
    std::string UITracker::view_str_;
    std::stack<uint64_t> UITracker::start_times_;

    void UITracker::initTrack() {
        indent_ = 0;
    }

    void UITracker::trackStart(bool is_layout_view, const char* name) {
        std::string ind(indent_, ' ');

        auto start_time = utl::TimeUtils::upTimeMillis();
        start_times_.push(start_time);

        if (is_layout_view) {
            LOG(Log::INFO) << ind << "-Group START (" << name << ") cur: " << start_time;
            indent_ += 2;
        } else {
            view_str_ = ind;
            view_str_.append("[View START (")
                .append(name).append(") cur: ")
                .append(std::to_string(start_time)).append("ms");
        }
    }

    void UITracker::trackEnd(bool is_layout_view, const char* name) {
        auto end_time = utl::TimeUtils::upTimeMillis();
        ubassert(!start_times_.empty());
        auto start_time = start_times_.top();
        start_times_.pop();

        if (is_layout_view) {
            ubassert(indent_ > 0);
            indent_ -= 2;
            std::string ind(indent_, ' ');

            LOG(Log::INFO) << ind << "Group END (" << name << ") cur: " << end_time
                << " duration: " << (end_time - start_time) << "ms";
        } else {
            view_str_.append(" - END ")
                .append(" cur: ")
                .append(std::to_string(end_time)).append("ms")
                .append(" duration: ").append(std::to_string(end_time - start_time)).append("ms]");

            LOG(Log::INFO) << view_str_;
        }
    }

    void UITracker::trackCur(const char* name) {
        ubassert(!start_times_.empty());
        auto prev_time = start_times_.top();
        auto cur_time = utl::TimeUtils::upTimeMillis();

        std::string ind(indent_, ' ');
        LOG(Log::INFO) << ind << "<Cur (" << name
            << ") duration: " << (cur_time - prev_time) << "ms";
    }

}
