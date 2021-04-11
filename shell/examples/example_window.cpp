// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "example_window.h"

#include "utils/convert.h"

#include "ukive/views/button.h"
#include "ukive/views/image_view.h"
#include "ukive/elements/color_element.h"
#include "ukive/graphics/color.h"
#include "ukive/views/tab/tab_strip_view.h"
#include "ukive/views/tab/tab_view.h"

#include "shell/resources/necro_resources_id.h"
#include "shell/examples/pages/example_tree_page.h"
#include "shell/examples/pages/example_misc_page.h"
#include "shell/examples/pages/example_list_page.h"


namespace shell {

    ExampleWindow::ExampleWindow()
        : Window() {}

    ExampleWindow::~ExampleWindow() {
    }

    void ExampleWindow::onCreated() {
        Window::onCreated();

        setBackgroundColor(ukive::Color::Transparent);

        showTitleBar();
        inflateTabView();
    }

    void ExampleWindow::onDestroy() {
        Window::onDestroy();
    }

    bool ExampleWindow::onGetWindowIconName(std::u16string* icon_name, std::u16string* small_icon_name) const {
        //*icon_name = IDI_SHELL;
        //*small_icon_name = IDI_SMALL;
        return false;
    }

    void ExampleWindow::inflateTabView() {
        setContentView(Res::Layout::example_window_layout_xml);
        getContentView()->setBackground(new ukive::ColorElement(ukive::Color::White));

        auto strip_view = findView<ukive::TabStripView>(Res::Id::tsv_example_strip);
        strip_view->setBackground(new ukive::ColorElement(ukive::Color::White));
        strip_view->setShadowRadius(getContext().dp2pxi(2));

        auto tab_view = findView<ukive::TabView>(Res::Id::tv_example_table);
        tab_view->setStripView(strip_view);

        tab_view->addPage(new ExampleMiscPage(this), u"Misc");
        tab_view->addPage(new ExampleListPage(this), u"ListView");
        tab_view->addPage(new ExampleTreePage(this), u"TreeView");
    }

}
