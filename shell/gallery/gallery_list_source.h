// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_GALLERY_GALLERY_LIST_SOURCE_H_
#define SHELL_GALLERY_GALLERY_LIST_SOURCE_H_

#include <vector>

#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_source.h"
#include "ukive/views/list/list_view.h"

#include "shell/gallery/thumbnail_fetcher.h"


namespace ukive {
    class ListView;
    class TextView;
    class ImageView;

    class ImageFrame;
}

namespace shell {

    class GalleryListSource
        : public ukive::ListSource,
          public ukive::ListItemRecycledListener,
          public ThumbnailFetchingListener {
    public:
        class GalleryListItem : public ukive::ListItem {
        public:
            explicit GalleryListItem(ukive::View* v);

            ukive::TextView* title_view_;
            ukive::ImageView* img_view_;
        };

        enum class ThumbStatus {
            None,
            Fetching,
            Complete,
        };

        struct Data {
            std::u16string title;
            std::u16string path;
            std::u16string first_img_name;
            std::shared_ptr<ukive::ImageFrame> img;
            bool is_dir;
            ThumbStatus ts = ThumbStatus::None;
            std::shared_ptr<int> token = std::make_shared<int>(0);
        };

        explicit GalleryListSource(ukive::ListView* lv);
        ~GalleryListSource();

        // ukive::ListSource
        ukive::ListItem* onListCreateItem(ukive::LayoutView* parent, int position) override;
        void onListSetItemData(ukive::ListItem* item, int position) override;
        int onListGetDataCount() override;

        void addItem(std::u16string title, std::u16string path, bool is_dir);
        const Data& getItem(int pos) const;
        void clearItems();

    protected:
        // ukive::ListItemRecycledListener
        void onChildRecycled(ukive::ListView* lv, ukive::ListItem* item) override;

        // ThumbnailFetchingListener
        void onThumbnail(const Thumbnail& thumb) override;

    private:
        std::vector<Data> data_;
        ukive::ListView* list_view_;
        std::unique_ptr<ThumbnailFetcher> thumb_fetcher_;
    };

}

#endif  // SHELL_GALLERY_GALLERY_LIST_SOURCE_H_