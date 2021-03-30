// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_CONTEXT_CHANGED_LISTENER_H_
#define UKIVE_WINDOW_CONTEXT_CHANGED_LISTENER_H_


namespace ukive {

    class Context;

    class ContextChangedListener {
    public:
        virtual ~ContextChangedListener() = default;

        virtual void onContextChanged(Context c) = 0;
    };

}

#endif  // UKIVE_WINDOW_CONTEXT_CHANGED_LISTENER_H_