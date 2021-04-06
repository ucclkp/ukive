// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_GRID_GRID_WINDOW_H_
#define SHELL_GRID_GRID_WINDOW_H_

#include "ukive/window/window.h"


namespace ukive {
    class GridView;
}

namespace shell {


    class GridWindow : public ukive::Window {
    public:
        void onCreated() override;
        bool onDataCopy(unsigned id, unsigned size, void* data);

    private:
        using super = Window;

        ukive::GridView* grid_view_ = nullptr;
    };

}

#endif  // SHELL_GRID_GRID_WINDOW_H_