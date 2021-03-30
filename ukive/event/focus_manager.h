// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_EVENT_FOCUS_MANAGER_H_
#define UKIVE_EVENT_FOCUS_MANAGER_H_

#include <memory>


namespace ukive {

    class InputEvent;
    class View;
    class LayoutView;
    class Window;

    class FocusManager {
    public:
        static FocusManager* focusOn(Window* w);

        void setWindow(Window* w);

        bool onInputEvent(InputEvent* e);

        bool next();
        bool prev();
        bool first();
        bool last();
        void clear();

    private:
        FocusManager();

        static std::unique_ptr<FocusManager> instance_;

        View* findNextFocusable(View* cur, bool check_cur);
        View* findNextFocusableIn(View* cur, bool check_cur);
        View* findPrevFocusable(View* cur);
        View* findPrevFocusableOut(View* cur, bool check_cur);

        Window* cur_window_ = nullptr;
    };

}

#endif  // UKIVE_EVENT_FOCUS_MANAGER_H_