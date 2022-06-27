// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "example_list_page.h"

#include "ukive/views/button.h"
#include "ukive/views/list/flow_list_layouter.h"
#include "ukive/views/list/grid_list_layouter.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/views/list/list_view.h"
#include "ukive/window/window.h"
#include "ukive/resources/layout_parser.h"
#include "ukive/system/dialogs/sys_message_dialog.h"
#include "ukive/dialogs/message_dialog.h"

#include "shell/examples/example_list_source.h"
#include "shell/resources/necro_resources_id.h"


namespace shell {

    ExampleListPage::ExampleListPage()
        : Page() {}

    ukive::View* ExampleListPage::onCreate(ukive::LayoutView* parent) {
        auto v = ukive::LayoutParser::from(
            parent->getContext(), parent, Res::Layout::example_list_page_layout_xml);

        // Buttons
        dwm_button_ = findView<ukive::Button>(v, Res::Id::bt_dwm_button);
        dwm_button_->setOnClickListener(this);

        // ListView
        list_source_ = new ExampleListSource();
        for (int i = 0; i < 136; ++i) {
            list_source_->addItem(0, u"test", u"test test");
        }

        list_view_ = findView<ukive::ListView>(v, Res::Id::lv_list_page_list);
        list_view_->setLayouter(new ukive::GridListLayouter(4));
        //list_view_->setLayouter(new ukive::LinearListLayouter());
        //list_view_->setLayouter(new ukive::FlowListLayouter(4));
        list_view_->setSource(list_source_);

        return v;
    }

    void ExampleListPage::onDestroy() {
        delete list_source_;
        list_source_ = nullptr;
    }

    void ExampleListPage::onClick(ukive::View* v) {
        if (v == dwm_button_) {
            /*if (getWindow()->isFullscreen()) {
                getWindow()->setFullscreen(false);
            } else {
                getWindow()->setFullscreen(true);
            }*/

            list_view_->scrollToPosition(list_source_->onGetListDataCount(list_view_), 0, false);
            //list_source_->removeItem(0);

            /*if (isTitleBarShowing()) {
                hideTitleBar();
            } else {
                showTitleBar();
            }*/

            ukive::MessageDialog::showDialog(
                getWindow(), u"提示", u"这是一行提示文本", u"确定");
        }
    }

}
