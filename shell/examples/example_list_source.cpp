// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "example_list_source.h"

#include "utils/log.h"
#include "utils/convert.h"

#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/views/image_view.h"
#include "ukive/views/text_view.h"
#include "ukive/elements/color_element.h"
#include "ukive/graphics/color.h"


namespace shell {

    ExampleListItem::ExampleListItem(ukive::View* v)
        : ListItem(v)
    {
        title_label = reinterpret_cast<ukive::TextView*>(v->findView(ID_TITLE));
        summary_label = reinterpret_cast<ukive::TextView*>(v->findView(ID_SUMMARY));
        avatar_image = reinterpret_cast<ukive::ImageView*>(v->findView(ID_AVATAR));
    }

    ukive::ListItem* ExampleListSource::onListCreateItem(
        ukive::LayoutView* parent, size_t position) {

        auto layout = new ukive::RestraintLayout(parent->getContext());
        layout->setBackground(new ukive::ColorElement(ukive::Color::Blue100));
        layout->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        layout->setLayoutMargin(4, 4, 4, 4);

        using Rlp = ukive::RestraintLayoutInfo;

        auto avatar_view = new ukive::ImageView(parent->getContext());
        avatar_view->setId(ID_AVATAR);
        avatar_view->setLayoutSize(36, 36);
        avatar_view->setLayoutMargin(16, 8, 0, 8);
        Rlp* av_lp = new Rlp();
        av_lp->startHandle(layout->getId(), Rlp::START);
        av_lp->topHandle(layout->getId(), Rlp::BOTTOM);
        av_lp->bottomHandle(layout->getId(), Rlp::BOTTOM);
        avatar_view->setExtraLayoutInfo(av_lp);
        layout->addView(avatar_view);

        ukive::TextView* title_label = new ukive::TextView(parent->getContext());
        title_label->setId(ID_TITLE);
        title_label->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        title_label->setLayoutMargin(8, 8, 8, 0);
        Rlp* tl_lp = new Rlp();
        tl_lp->startHandle(avatar_view->getId(), Rlp::END);
        tl_lp->topHandle(layout->getId(), Rlp::TOP);
        tl_lp->endHandle(layout->getId(), Rlp::END);
        title_label->setExtraLayoutInfo(tl_lp);
        layout->addView(title_label);

        ukive::TextView* summary_label = new ukive::TextView(parent->getContext());
        summary_label->setId(ID_SUMMARY);
        summary_label->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        summary_label->setLayoutMargin(8, 4, 8, 8);
        Rlp* sl_lp = new Rlp();
        sl_lp->startHandle(avatar_view->getId(), Rlp::END);
        sl_lp->topHandle(ID_TITLE, Rlp::BOTTOM);
        sl_lp->endHandle(layout->getId(), Rlp::END);
        sl_lp->bottomHandle(layout->getId(), Rlp::BOTTOM);
        summary_label->setExtraLayoutInfo(sl_lp);
        layout->addView(summary_label);

        return new ExampleListItem(layout);
    }

    void ExampleListSource::onListSetItemData(ukive::ListItem* item, size_t position) {
        auto& data = data_list_.at(position);
        ExampleListItem* example_list_item = reinterpret_cast<ExampleListItem*>(item);

        std::u16string str(data.title);
        str.append(u" ").append(utl::to_u16string(position));

        example_list_item->title_label->setText(str);
        example_list_item->summary_label->setText(data.summary);

        //LOG(INFO) << "ListSource::onListSetItemData():" << position << " data has been bound.";
    }

    size_t ExampleListSource::onListGetDataCount() const {
        return data_list_.size();
    }

    void ExampleListSource::addItem(int image_res_id, const std::u16string& title, const std::u16string& summary) {
        BindData data;
        data.image_resource_id = image_res_id;
        data.title = title;
        data.summary = summary;

        data_list_.push_back(data);
        notifyDataChanged();
    }

    void ExampleListSource::addItem(size_t pos, int image_res_id, const std::u16string& title, const std::u16string& summary) {
        BindData data;
        data.image_resource_id = image_res_id;
        data.title = title;
        data.summary = summary;

        if (pos > data_list_.size()) {
            pos = data_list_.size();
        }

        data_list_.insert(data_list_.begin() + pos, data);
        notifyDataChanged();
    }

    void ExampleListSource::modifyItem(int image_res_id, const std::u16string& title, const std::u16string& summary) {
        for (auto it = data_list_.begin();
            it != data_list_.end(); ++it) {
            if ((*it).title == title) {
                (*it).image_resource_id = image_res_id;
                (*it).summary = summary;
            }
        }

        notifyDataChanged();
    }

    void ExampleListSource::removeItem(const std::u16string& title) {
        for (auto it = data_list_.begin(); it != data_list_.end();) {
            if ((*it).title == title) {
                it = data_list_.erase(it);
            } else {
                ++it;
            }
        }

        notifyDataChanged();
    }

    void ExampleListSource::removeItem(size_t pos) {
        if (pos >= data_list_.size()) {
            return;
        }

        data_list_.erase(data_list_.begin() + pos);
        notifyDataChanged();
    }

}