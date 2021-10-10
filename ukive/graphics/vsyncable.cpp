// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "vsyncable.h"

#include "ukive/app/application.h"


namespace ukive {

    VSyncable::VSyncable() {}

    VSyncable::~VSyncable() {
        stopVSync();
    }

    void VSyncable::startVSync() {
        if (!is_started_) {
            is_started_ = true;
            Application::getVSyncProvider()->addCallback(this);
            Application::getVSyncProvider()->startVSync();
        }
    }

    void VSyncable::stopVSync() {
        if (is_started_) {
            Application::getVSyncProvider()->stopVSync();
            Application::getVSyncProvider()->removeCallback(this);
            is_started_ = false;
        }
    }

}
