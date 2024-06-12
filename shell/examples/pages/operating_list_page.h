// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_EXAMPLES_PAGES_OPERATING_LIST_PAGE_H_
#define SHELL_EXAMPLES_PAGES_OPERATING_LIST_PAGE_H_

#include "ukive/animation/timer.h"
#include "ukive/page/page.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_source.h"


namespace ukive {
    class Button;
    class ListView;
    class TextView;
    class ImageView;
    class SeekBar;
}

namespace shell {

    static const int ID_TITLE   = 1;
    static const int ID_SEEKBAR = 3;

    class OperatingListItem : public ukive::ListItem {
    public:
        explicit OperatingListItem(ukive::View* v);

        ukive::TextView* title_label;
        ukive::SeekBar* seek_bar;
    };

    class OperatingListPage :
        public ukive::Page,
        public ukive::ListSource,
        public ukive::OnClickListener
    {
    public:
        OperatingListPage();

        // ukive::Page
        ukive::View* onCreate(ukive::Context c) override;
        void onDestroy() override;

        // ukive::OnClickListener
        void onClick(ukive::View* v) override;

    protected:
        struct BindData {
            std::u16string title;
            float progress;
        };

        ukive::ListItem* onCreateListItem(
            ukive::LayoutView* parent, ukive::ListItemEventRouter* router,
            size_t position) override;
        void onSetListItemData(
            ukive::LayoutView* parent, ukive::ListItemEventRouter* router,
            ukive::ListItem* item) override;
        size_t onGetListDataCount(ukive::LayoutView* parent) const override;

    private:
        ukive::Button* op_button_ = nullptr;
        ukive::ListView* list_view_ = nullptr;
        ukive::TextView* state_label_ = nullptr;

        ukive::Timer timer_;
        std::vector<BindData> data_list_;
    };

}

#endif  // SHELL_EXAMPLES_PAGES_OPERATING_LIST_PAGE_H_