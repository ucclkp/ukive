// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_DIAGNOSTIC_UI_TRACKER_H_
#define UKIVE_DIAGNOSTIC_UI_TRACKER_H_

#include <stack>
#include <string>

//#define UI_TRACK_ENABLED
#ifdef UI_TRACK_ENABLED

#define UI_TRACK_RT_START  \
    UITracker::trackStart(isLayoutView(), typeid(*this).name());

#define UI_TRACK_RT_END  \
    UITracker::trackEnd(isLayoutView(), typeid(*this).name());

#define UI_TRACK_RT_CUR(name)  \
    UITracker::trackCur(name);

#else

#define UI_TRACK_RT_START
#define UI_TRACK_RT_END
#define UI_TRACK_RT_CUR(name)

#endif


namespace ukive {

    class UITracker {
    public:
        static void initTrack();
        static void trackStart(bool is_layout_view, const char* name);
        static void trackEnd(bool is_layout_view, const char* name);
        static void trackCur(const char* name);

    private:
        static int indent_;
        static std::string view_str_;
        static std::stack<uint64_t> start_times_;
    };

}

#endif  // UKIVE_DIAGNOSTIC_UI_TRACKER_H_