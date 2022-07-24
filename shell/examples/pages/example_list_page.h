// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_EXAMPLES_PAGES_EXAPMLE_LIST_PAGE_H_
#define SHELL_EXAMPLES_PAGES_EXAPMLE_LIST_PAGE_H_

#include "ukive/page/page.h"
#include "ukive/views/click_listener.h"


namespace ukive {
    class Button;
    class ListView;
}

namespace shell {

    class ExampleListSource;

    class ExampleListPage :
        public ukive::Page,
        public ukive::OnClickListener
    {
    public:
        ExampleListPage();

        // ukive::Page
        ukive::View* onCreate(ukive::Context c) override;
        void onDestroy() override;

        // ukive::OnClickListener
        void onClick(ukive::View* v) override;

    private:
        ukive::Button* dwm_button_ = nullptr;
        ukive::ListView* list_view_ = nullptr;
        ExampleListSource* list_source_ = nullptr;
    };

}

#endif  // SHELL_EXAMPLES_PAGES_EXAPMLE_LIST_PAGE_H_