// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_VIEW_STATUS_LISTENER_H_
#define UKIVE_VIEWS_VIEW_STATUS_LISTENER_H_


namespace ukive {

    class OnViewStatusListener {
    public:
        virtual ~OnViewStatusListener() = default;

        virtual void onViewEnableChanged(bool enabled) {}
        virtual void onViewVisibilityChanged(int visibility) {}
    };

}

#endif  // UKIVE_VIEWS_VIEW_STATUS_LISTENER_H_