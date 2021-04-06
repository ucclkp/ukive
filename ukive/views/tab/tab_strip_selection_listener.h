// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TAB_TAB_STRIP_SELECTION_LISTENER_H_
#define UKIVE_VIEWS_TAB_TAB_STRIP_SELECTION_LISTENER_H_


namespace ukive {

    class TabStripSelectionListener {
    public:
        virtual ~TabStripSelectionListener() = default;

        virtual void onTabStripSelectionChanged(size_t index) = 0;
    };

}

#endif  // UKIVE_VIEWS_TAB_TAB_STRIP_SELECTION_LISTENER_H_