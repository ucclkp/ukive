// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef VEVAH_MAIN_WINDOW_H_
#define VEVAH_MAIN_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/views/list/list_item_event_router.h"


namespace ukive {
    class ListView;
}

namespace vevah {

    class ContainerLayout;
    class ControlListSource;

    class MainWindow :
        public ukive::Window,
        public ukive::ListItemEventListener
    {
    public:
        MainWindow();

    protected:
        // ukive::Window
        void onCreated() override;
        void onDestroy() override;

        // ukive::ListItemEventListener
        void onItemPressed(
            ukive::ListView* list_view,
            ukive::ListItem* item, ukive::View* v) override;

    private:
        using super = Window;

        ukive::View* left_panel_ = nullptr;
        ukive::ListView* ctrl_list_ = nullptr;
        ContainerLayout* container_layout_ = nullptr;
        ControlListSource* ctrl_source_ = nullptr;
    };

}

#endif  // VEVAH_MAIN_WINDOW_H_