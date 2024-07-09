// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_VIEW_STATUS_LISTENER_H_
#define UKIVE_VIEWS_VIEW_STATUS_LISTENER_H_


namespace ukive {

    class View;

    class OnViewStatusListener {
    public:
        virtual ~OnViewStatusListener() = default;

        virtual void onViewFocusChanged(View* v, bool focus) {}
        virtual void onViewEnableChanged(View* v, bool enabled) {}
        virtual void onViewVisibilityChanged(View* v, int visibility) {}
    };

}

#endif  // UKIVE_VIEWS_VIEW_STATUS_LISTENER_H_