// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_EXAMPLES_EXAMPLE_LIST_SOURCE_H_
#define SHELL_EXAMPLES_EXAMPLE_LIST_SOURCE_H_

#include <vector>

#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_source.h"


namespace ukive {
    class TextView;
    class ImageView;
}

namespace shell {

    static const int ID_TITLE = 1;
    static const int ID_SUMMARY = 2;
    static const int ID_AVATAR = 3;

    class ExampleListItem : public ukive::ListItem {
    public:
        explicit ExampleListItem(ukive::View* v);

        ukive::TextView* title_label;
        ukive::TextView* summary_label;
        ukive::ImageView* avatar_image;
    };

    class ExampleListSource : public ukive::ListSource {
    public:
        struct BindData {
            int image_resource_id;
            std::u16string title;
            std::u16string summary;
        };

        ukive::ListItem* onListCreateItem(ukive::LayoutView* parent, size_t position) override;
        void onListSetItemData(ukive::ListItem* item, size_t position) override;
        size_t onListGetDataCount() const override;

        void addItem(int image_res_id, const std::u16string& title, const std::u16string& summary);
        void addItem(size_t pos, int image_res_id, const std::u16string& title, const std::u16string& summary);
        void modifyItem(int image_res_id, const std::u16string& title, const std::u16string& summary);
        void removeItem(const std::u16string& title);
        void removeItem(size_t pos);

    private:
        std::vector<BindData> data_list_;
    };

}

#endif  // SHELL_EXAMPLES_EXAMPLE_LIST_SOURCE_H_