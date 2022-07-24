// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "main_window.h"

#include "utils/strings/string_utils.hpp"

#include "ukive/elements/element.h"
#include "ukive/resources/layout_parser.h"
#include "ukive/views/button.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/views/list/list_item.h"
#include "ukive/window/haul_source.h"

#include "vevah/container_layout.h"
#include "vevah/control_list_source.h"
#include "vevah/properties/property_page.h"
#include "vevah/resources/necro_resources_id.h"


namespace {

    bool needFauxView(const std::string_view& real_name) {
        return (real_name == "ImageView" ||
            real_name == "ChartView" ||
            real_name == "GridView" ||
            real_name == "MediaView" ||
            real_name == "TitleBarLayout" ||
            real_name == "SimpleLayout" ||
            real_name == "SequenceLayout" ||
            real_name == "ListView" ||
            real_name == "RestraintLayout" ||
            real_name == "ScrollView" ||
            real_name == "TabStripView" ||
            real_name == "TabView");
    }

}

namespace vevah {

    MainWindow::MainWindow() {}

    void MainWindow::onCreated() {
        super::onCreated();

        showTitleBar();
        setContentView(Res::Layout::main_window_layout_xml);

        auto c = getContext();

        left_panel_ = findView<ukive::View>(Res::Id::sl_main_wnd_left_panel);
        left_panel_->setBackground(new ukive::Element(ukive::Color::Grey100));

        container_layout_ = findView<ContainerLayout>(Res::Id::cl_main_wnd_cur_view);
        container_layout_->setBackground(new ukive::Element(ukive::Color::Grey100));
        container_layout_->setSelectedListener(this);

        ctrl_list_ = findView<ukive::ListView>(Res::Id::lv_main_wnd_controls);

        ctrl_source_ = new ControlListSource();
        ctrl_list_->setSource(ctrl_source_);
        ctrl_list_->setLayouter(new ukive::LinearListLayouter());
        ctrl_list_->setItemEventRouter(new ukive::ListItemEventRouter(this));

        haul_src_ = std::make_unique<ukive::HaulSource>(0, this);

        levitator_.setDismissByTouchOutside(true);
        levitator_.setOutsideTouchable(true);
        levitator_.setInputEnabled(false);

        prop_page_ = std::make_unique<PropertyPage>();
        auto prop_v = prop_page_->create(c);
        prop_page_->initialize();

        auto ph_v = findView<ukive::SimpleLayout>(Res::Id::v_placeholder);
        auto root_v = findView<ukive::LayoutView>(Res::Id::rl_main_wnd_root);
        ph_v->addView(prop_v);
    }

    void MainWindow::onDestroy() {
        prop_page_->destroy();
        delete ctrl_source_;
        ctrl_source_ = nullptr;

        super::onDestroy();
    }

    void MainWindow::onItemPressed(
        ukive::ListView* list_view,
        ukive::ListItem* item, ukive::View* v)
    {
        auto c = getContext();
        int margin = c.dp2pxi(4);

        std::string faux_name;
        auto& real_name = ctrl_source_->getName(item->data_pos);
        if (needFauxView(real_name)) {
            faux_name = "TextView";
        } else {
            faux_name = real_name;
        }

        auto view = ukive::LayoutParser::createView(faux_name, c, {});
        if (faux_name == "TextView") {
            static_cast<ukive::TextView*>(view)->setText(utl::u8to16(real_name));
        }

        view->animeParams().setAlpha(0.75);
        view->setLayoutMargin({ margin, margin, margin, margin });
        levitator_.setContentView(view);

        haul_src_->setExData("view", real_name);

        v->setHaulSource(haul_src_.get());
    }

    void MainWindow::onHaulStarted(
        ukive::HaulSource* src,
        ukive::View* v, ukive::InputEvent* e)
    {
        ukive::Levitator::PosInfo info;
        info.corner = ukive::GV_MID_HORI | ukive::GV_MID_VERT;
        levitator_.show(this, e->getRawX(), e->getRawY(), info);
    }

    void MainWindow::onHaulStopped(ukive::HaulSource* src) {
        levitator_.dismiss();
    }

    void MainWindow::onHaulCancelled(ukive::HaulSource* src) {
        levitator_.dismiss();
    }

    bool MainWindow::onHauling(ukive::HaulSource* src, ukive::InputEvent* e) {
        levitator_.update(e->getRawX(), e->getRawY());
        return true;
    }

    void MainWindow::onViewSelected(ukive::View* v) {
        prop_page_->showProps(v);
    }

}
