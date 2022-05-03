// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_INPUT_METHOD_MANAGER_H_
#define UKIVE_TEXT_INPUT_METHOD_MANAGER_H_


namespace ukive {

    class InputMethodManager {
    public:
        static InputMethodManager* create();

        virtual ~InputMethodManager() = default;

        virtual bool initialize() = 0;
        virtual void destroy() = 0;

        virtual bool updateIMEStatus() = 0;
    };

}

#endif  // UKIVE_TEXT_INPUT_METHOD_MANAGER_H_