// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_SEEK_VALUE_CHANGED_LISTENER_H_
#define UKIVE_VIEWS_SEEK_VALUE_CHANGED_LISTENER_H_


namespace ukive {

    class SeekBar;

    class OnSeekValueChangedListener {
    public:
        virtual ~OnSeekValueChangedListener() = default;

        virtual void onSeekValueChanged(SeekBar* sb, double value) = 0;
        virtual void onSeekIntegerValueChanged(SeekBar* sb, int value) {}
    };

}

#endif  // UKIVE_VIEWS_SEEK_VALUE_CHANGED_LISTENER_H_