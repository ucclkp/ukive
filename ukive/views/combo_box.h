// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_COMBO_BOX_H_
#define UKIVE_VIEWS_COMBO_BOX_H_

#include "utils/var.hpp"
#include "utils/weak_ref_nest.hpp"

#include "ukive/basics/levitator.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/layout/layout_view.h"
#include "ukive/views/list/list_item_event_router.h"
#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/text_view.h"


namespace ukive {

    class TextView;
    class DropdownButton;
    class ComboBoxSelectedListener;

    class ComboBox :
        public LayoutView,
        public OnClickListener,
        public OnInnerWindowEventListener,
        public ListItemEventListener,
        public ListSource
    {
    public:
        explicit ComboBox(Context c);
        ComboBox(Context c, AttrsRef attrs);
        ~ComboBox();

        void addItem(const std::u16string_view& title);
        void addItem(size_t index, const std::u16string_view& title);
        bool modifyItem(size_t index, const std::u16string_view& title);
        void removeItem(size_t index);
        void clearItems();

        void setSelectedIndex(int index);
        void setDefaultItem(const std::u16string_view& title);
        void setDropdownMinWidth(int min_width);
        void setListener(ComboBoxSelectedListener* l);

        size_t getItemCount() const;
        const std::u16string& getItemTitle(size_t index) const;
        const std::u16string& getDefaultTitle(size_t index) const;
        int getSelectedIndex() const;

        bool hasItemData(size_t index) const;
        bool hasDefaultData() const;

        template <typename Ty>
        void addItem(const std::u16string_view& title, const Ty& data) {
            addItemItl(title, std::make_shared<Ty>(data));
        }

        template <typename Ty>
        void addItem(size_t index, const std::u16string_view& title, const Ty& data) {
            addItemItl(index, title, std::make_shared<Ty>(data));
        }

        template <typename Ty>
        bool modifyItemData(size_t index, const Ty& data) {
            if (index > items_.size()) {
                return false;
            }
            items_[index].data = std::make_shared<Ty>(data);
        }

        template <typename Ty>
        bool modifyItem(size_t index, const std::u16string_view& title, const Ty& data) {
            if (index > items_.size()) {
                return false;
            }
            items_[index].title = title;
            items_[index].data = std::make_shared<Ty>(data);
            notifyDataChanged();
        }

        template <typename Ty>
        void setDefaultItemData(const Ty& data) {
            default_item_.data = std::make_shared<Ty>(data);
        }

        template <typename Ty>
        void setDefaultItem(const std::u16string_view& title, const Ty& data) {
            setDefaultItem(title);
            setDefaultItemData(data);
        }

        template <typename Ty>
        const Ty& getItemData(size_t index) const {
            if (index > items_.size()) {
                index = items_.size();
            }
            return *static_cast<Ty*>(items_[index].data.get());
        }

        template <typename Ty>
        const Ty& getDefaultData(size_t index) const {
            return *static_cast<Ty*>(default_item_.data.get());
        }

    protected:
        class TextViewListItem : public ListItem {
        public:
            explicit TextViewListItem(TextView* v)
                : ListItem(v),
                  title_label(v) {}

            TextView* title_label;
        };

        // LayoutView
        Size onDetermineSize(const SizeInfo& info) override;
        void onLayout(const Rect& new_bounds, const Rect& old_bounds) override;

        // OnClickListener
        void onClick(View* v) override;

        // ListItemEventListener
        void onItemClicked(ListView* list_view, ListItem* item, View* v) override;

        // OnInnerWindowEventListener
        void onRequestDismissByTouchOutside(Levitator* lev) override;

        // ListSource
        ListItem* onCreateListItem(
            LayoutView* parent, ListItemEventRouter* router, size_t position) override;
        void onSetListItemData(
            LayoutView* parent, ListItemEventRouter* router, ListItem* item) override;
        size_t onGetListDataCount(LayoutView* parent) const override;

    private:
        typedef std::shared_ptr<void> DataPtr;

        struct ItemData {
            std::u16string title;
            DataPtr data;
        };

        void initViews();
        void determineViewsSize(const SizeInfo& info);

        void addItemItl(const std::u16string_view& title, const DataPtr& data);
        void addItemItl(size_t index, const std::u16string_view& title, const DataPtr& data);
        void itemAdded();

        void show(int width);
        void close();

        TextView* text_view_;
        DropdownButton* button_;
        ListView* list_view_ = nullptr;

        int selected_index_ = -1;
        std::vector<ItemData> items_;
        ItemData default_item_;

        bool is_finished_ = true;
        int min_dropdown_width_ = 0;
        ComboBoxSelectedListener* listener_ = nullptr;
        std::shared_ptr<Levitator> levitator_;
        utl::WeakRefNest<ComboBox> weak_ref_nest_;
    };

}

#endif  // UKIVE_VIEWS_COMBO_BOX_H_