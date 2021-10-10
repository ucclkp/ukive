// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_VSYNCABLE_H_
#define UKIVE_GRAPHICS_VSYNCABLE_H_

#include "ukive/graphics/vsync_provider.h"


namespace ukive {

    class VSyncable : public VSyncCallback {
    public:
        VSyncable();
        ~VSyncable();

        void startVSync();
        void stopVSync();

    private:
        bool is_started_ = false;
    };

}

#endif  // UKIVE_GRAPHICS_VSYNCABLE_H_