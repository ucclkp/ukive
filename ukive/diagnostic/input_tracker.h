// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_DIAGNOSTIC_INPUT_TRACKER_H_
#define UKIVE_DIAGNOSTIC_INPUT_TRACKER_H_

#include <stack>
#include <string>

//#define INPUT_TRACK_ENABLED
#ifdef INPUT_TRACK_ENABLED

#define INPUT_TRACK_RT_START(mn)  \
    InputTracker::trackStart(typeid(*this).name(), mn);

#define INPUT_TRACK_RT_END()   \
    InputTracker::trackEnd();

#define INPUT_TRACK_RT_CUR(name)  \
    InputTracker::trackCur(name);

#define INPUT_TRACK_PRINT(prefix, e)  \
    InputTracker::printInputEvent(prefix, e);

#else

#define INPUT_TRACK_RT_START(mn)
#define INPUT_TRACK_RT_END()
#define INPUT_TRACK_RT_CUR(name)
#define INPUT_TRACK_PRINT(prefix, e)

#endif


namespace ukive {

    class InputEvent;

    class InputTracker {
    public:
        static void initTrack();
        static void trackStart(const char* cls_name, const char* met_name);
        static void trackEnd();
        static void trackCur(const char* cls_name);

        static void printInputEvent(const char* prefix, InputEvent* e);

    private:
        static int indent_;
        static std::string view_str_;
        static std::stack<uint64_t> start_times_;
    };

}

#endif  // UKIVE_DIAGNOSTIC_INPUT_TRACKER_H_