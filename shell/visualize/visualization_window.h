// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_VISUALIZE_VISUALIZATION_WINDOW_H_
#define SHELL_VISUALIZE_VISUALIZATION_WINDOW_H_

#include "ukive/views/space3d_view.h"
#include "ukive/window/window.h"

#include "shell/visualize/visual_layout_scene.h"


namespace vsul {

    class VisualizationWindow : public ukive::Window {
    public:
        VisualizationWindow();

        // ukive::Window
        void onCreated() override;
        void onDestroy() override;

    private:
        using super = ukive::Window;

        VisualLayoutScene* scene_ = nullptr;
        ukive::Space3DView* space_view_ = nullptr;
    };

}

#endif  // SHELL_VISUALIZE_VISUALIZATION_WINDOW_H_