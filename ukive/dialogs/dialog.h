// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_DIALOGS_DIALOG_H_
#define UKIVE_DIALOGS_DIALOG_H_

#include "ukive/basics/inner_window.h"


namespace ukive {

    class Dialog : public OnInnerWindowEventListener {
    public:
        explicit Dialog(Context c);
        virtual ~Dialog();

        void show(Window* parent, int x, int y);
        void show(Window* parent, int gravity);
        void close();

        void setOwnership(bool myself);

    protected:
        virtual View* onCreate(Context c) = 0;
        virtual void onDestroy() {}

        // OnInnerWindowEventListener
        void onRequestDismissByTouchOutside(InnerWindow* iw) override;

    private:
        Window* window_;
        std::shared_ptr<InnerWindow> inner_window_;
        bool is_own_by_myself_ = false;
    };

}

#endif  // UKIVE_DIALOGS_DIALOG_H_