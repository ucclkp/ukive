// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/app/application.h"

#import <Cocoa/Cocoa.h>

#include "ukive/app/mac/app_delegate.h"


namespace ukive {

    void Application::initPlatform() {
        [NSApplication sharedApplication];

        auto app_delegate = [[AppDelegate alloc] init];
        [NSApp setDelegate:app_delegate];
        [app_delegate release];
    }

    void Application::cleanPlatform() {
        [NSApp terminate:nil];
    }

}
