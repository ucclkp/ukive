// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_COMBO_BOX_COMBO_LIST_SOURCE_H_
#define UKIVE_VIEWS_COMBO_BOX_COMBO_LIST_SOURCE_H_

#include <vector>

#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_source.h"


namespace ukive {

    class TextView;

    class ComboListSource : public ListSource {
    public:
        class TextViewListItem : public ListItem {
        public:
            explicit TextViewListItem(TextView* v)
                : ListItem(reinterpret_cast<View*>(v)),
                  title_label(v) {}

            TextView* title_label;
        };

        ComboListSource();

        ListItem* onListCreateItem(LayoutView* parent, int position) override;
        void onListSetItemData(ListItem* item, int position) override;
        int onListGetDataCount() override;

        void addItem(const std::u16string& title);
        void addItem(int pos, const std::u16string& title);
        void modifyItem(int pos, const std::u16string& title);
        void removeItem(int pos);
        std::u16string getItemData(int pos) const;

    private:
        std::vector<std::u16string> data_;
    };

}

#endif  // UKIVE_VIEWS_COMBO_BOX_COMBO_LIST_SOURCE_H_