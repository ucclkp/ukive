// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_EXAMPLES_EXAMPLE_WINDOW_H_
#define SHELL_EXAMPLES_EXAMPLE_WINDOW_H_

#include "ukive/window/window.h"


namespace shell {

    class ExampleListSource;

    class ExampleWindow :
        public ukive::Window
    {
    public:
        ExampleWindow();
        ~ExampleWindow();

        // ukive::Window
        void onCreated() override;
        void onDestroy() override;
        bool onGetWindowIconName(std::u16string* icon_name, std::u16string* small_icon_name) const override;

    private:
        void inflateTabView();
    };

}

#endif  // SHELL_EXAMPLES_EXAMPLE_WINDOW_H_