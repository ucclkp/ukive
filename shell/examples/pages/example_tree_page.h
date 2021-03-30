// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_EXAMPLES_PAGES_EXAMPLE_TREE_PAGE_H_
#define SHELL_EXAMPLES_PAGES_EXAMPLE_TREE_PAGE_H_

#include "ukive/page/page.h"
#include "ukive/views/list/list_view.h"


namespace ukive {
    class TreeSource;
    class LinearListLayouter;
}

namespace shell {

    class ExampleTreePage : public ukive::Page, public ukive::ListItemSelectedListener {
    public:
        explicit ExampleTreePage(ukive::Window* w);

        // ukive::Page
        ukive::View* onCreate(ukive::LayoutView* parent) override;

        // ukive::ListItemSelectedListener
        void onItemClicked(ukive::ListView* lv, ukive::ListItem* item) override;

    private:
        ukive::TreeSource* tree_source_ = nullptr;
        ukive::LinearListLayouter* layouter_ = nullptr;
    };

}

#endif  // SHELL_EXAMPLES_PAGES_EXAMPLE_TREE_PAGE_H_