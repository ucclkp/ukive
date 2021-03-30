// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shell/gallery/gallery_list_source.h"

#include <algorithm>

#include "utils/convert.h"
#include "utils/number.hpp"

#include "ukive/resources/layout_instantiator.h"
#include "ukive/views/layout/layout_view.h"
#include "ukive/views/text_view.h"
#include "ukive/views/image_view.h"
#include "ukive/elements/color_element.h"
#include "ukive/elements/ripple_element.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/images/image_options.h"

#include "shell/resources/necro_resources_id.h"
#include "shell/gallery/gallery_utils.h"


namespace shell {

    // ExampleListItem
    GalleryListSource::GalleryListItem::GalleryListItem(ukive::View* v)
        : ListItem(v)
    {
        title_view_ = static_cast<ukive::TextView*>(v->findView(Res::Id::tv_gallery_item_title));
        img_view_ = static_cast<ukive::ImageView*>(v->findView(Res::Id::iv_gallery_item_img));
    }


    // GalleryListSource
    GalleryListSource::GalleryListSource(ukive::ListView* lv)
        : list_view_(lv),
          thumb_fetcher_(std::make_unique<ThumbnailFetcher>())
    {
        list_view_->setChildRecycledListener(this);
        thumb_fetcher_->setListener(this);

        thumb_fetcher_->launch();
    }

    GalleryListSource::~GalleryListSource() {
        thumb_fetcher_->shutdown();
    }

    ukive::ListItem*
        GalleryListSource::onListCreateItem(ukive::LayoutView* parent, int position)
    {
        auto view = ukive::LayoutInstantiator::from(
            parent->getContext(), parent, Res::Layout::gallery_item_layout_xml);
        view->setClickable(true);

        auto bg = new ukive::RippleElement();
        bg->setTintColor(ukive::Color::White);
        view->setBackground(bg);

        return new GalleryListItem(view);
    }

    void GalleryListSource::onListSetItemData(ukive::ListItem* item, int position) {
        auto gallery_list_item = static_cast<GalleryListItem*>(item);

        auto& d = data_[position];
        if (d.ts == ThumbStatus::None) {
            thumb_fetcher_->add(d.path, !d.is_dir, item->data_pos, d.token);
            d.ts = ThumbStatus::Fetching;
        }

        gallery_list_item->title_view_->setText(d.title);
        gallery_list_item->img_view_->setImage(d.img);
        gallery_list_item->item_view->getBackground()->resetState();
    }

    int GalleryListSource::onListGetDataCount() {
        return utl::num_cast<int>(data_.size());
    }

    void GalleryListSource::onChildRecycled(ukive::ListView* lv, ukive::ListItem* item) {
        auto pos = item->data_pos;
        if (pos >= onListGetDataCount()) {
            return;
        }

        auto& d = data_[pos];
        if (d.ts == ThumbStatus::Fetching) {
            d.token = std::make_shared<int>();
            d.ts = ThumbStatus::None;
        }
    }

    void GalleryListSource::onThumbnail(const Thumbnail& thumb) {
        if (!thumb.w_token.lock()) {
            return;
        }

        ukive::ImageFrame* img;
        if (thumb.data.empty()) {
            img = img = ukive::ImageFrame::create(
                list_view_->getWindow(), thumb.ph_bmp.get());
        } else {
            img = ukive::ImageFrame::create(
                list_view_->getWindow(),
                thumb.width, thumb.height,
                reinterpret_cast<const uint8_t*>(thumb.data.data()),
                thumb.data.size(), thumb.width * 4, thumb.options);
        }

        auto& d = data_[thumb.id];
        d.ts = ThumbStatus::Complete;
        d.img = std::shared_ptr<ukive::ImageFrame>(img);
        notifyDataChanged();
    }

    void GalleryListSource::addItem(
        std::u16string title, std::u16string path, bool is_dir)
    {
        Data d;
        d.title = std::move(title);
        d.path = std::move(path);
        d.is_dir = is_dir;
        data_.push_back(std::move(d));
    }

    const GalleryListSource::Data& GalleryListSource::getItem(int pos) const {
        return data_[pos];
    }

    void GalleryListSource::clearItems() {
        data_.clear();
        thumb_fetcher_->clear();
    }

}
