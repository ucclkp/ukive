// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_CLICK_LISTENER_H_
#define UKIVE_VIEWS_CLICK_LISTENER_H_


namespace ukive {

    class View;

    class OnClickListener {
    public:
        virtual ~OnClickListener() = default;

        virtual void onClick(View* v) {}
        virtual void onDoubleClick(View* v) {}
        virtual void onTripleClick(View* v) {}

        virtual void onSecClick(View* v) {}
        virtual void onSecDoubleClick(View* v) {}
        virtual void onSecTripleClick(View* v) {}

        virtual void onMidClick(View* v) {}
        virtual void onMidDoubleClick(View* v) {}
        virtual void onMidTripleClick(View* v) {}

        virtual void onXB1Click(View* v) {}
        virtual void onXB2Click(View* v) {}
    };

}

#endif  // UKIVE_VIEWS_CLICK_LISTENER_H_