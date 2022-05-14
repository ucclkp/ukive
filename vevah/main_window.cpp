// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "main_window.h"

#include "ukive/elements/color_element.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/list/linear_list_layouter.h"

#include "vevah/container_layout.h"
#include "vevah/control_list_source.h"
#include "vevah/resources/necro_resources_id.h"


namespace vevah {

    MainWindow::MainWindow() {}

    void MainWindow::onCreated() {
        super::onCreated();

        showTitleBar();
        setContentView(Res::Layout::main_window_layout_xml);

        left_panel_ = findView<ukive::View>(Res::Id::sl_main_wnd_left_panel);
        left_panel_->setBackground(new ukive::ColorElement(ukive::Color::Grey100));

        container_layout_ = findView<ContainerLayout>(Res::Id::cl_main_wnd_cur_view);
        container_layout_->setBackground(new ukive::ColorElement(ukive::Color::Green100));

        ctrl_list_ = findView<ukive::ListView>(Res::Id::lv_main_wnd_controls);

        ctrl_source_ = new ControlListSource();
        ctrl_list_->setSource(ctrl_source_);
        ctrl_list_->setLayouter(new ukive::LinearListLayouter());
        ctrl_list_->setItemEventRouter(new ukive::ListItemEventRouter(this));
    }

    void MainWindow::onDestroy() {
        super::onDestroy();

        delete ctrl_source_;
        ctrl_source_ = nullptr;
    }

    void MainWindow::onItemPressed(
        ukive::ListView* list_view,
        ukive::ListItem* item, ukive::View* v)
    {

    }

}
