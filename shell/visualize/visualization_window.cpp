// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "visualization_window.h"


namespace vsul {

    VisualizationWindow::VisualizationWindow() {}

    void VisualizationWindow::onCreated() {
        super::onCreated();
        showTitleBar();

        scene_ = new VisualLayoutScene();
        space_view_ = new ukive::Space3DView(getContext(), scene_);
        setContentView(space_view_);
    }

    void VisualizationWindow::onDestroy() {
        super::onDestroy();

        delete scene_;
    }

}
