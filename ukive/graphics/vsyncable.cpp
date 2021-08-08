// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "vsyncable.h"

#include "utils/log.h"

#include "ukive/app/application.h"


namespace ukive {

    VSyncable::VSyncable() {}

    VSyncable::~VSyncable() {
        stopListenVSync();
    }

    void VSyncable::startVSync() {
        startListenVSync();
        requestVSync();
    }

    void VSyncable::stopVSync() {
        stopListenVSync();
    }

    void VSyncable::startListenVSync() {
        if (!is_start_listening_) {
            is_start_listening_ = true;
            Application::getVSyncProvider()->addCallback(this);
        }
    }

    void VSyncable::stopListenVSync() {
        if (is_start_listening_) {
            is_start_listening_ = false;
            Application::getVSyncProvider()->removeCallback(this);
        }
    }

    void VSyncable::requestVSync() {
        Application::getVSyncProvider()->requestVSync();
    }

}
