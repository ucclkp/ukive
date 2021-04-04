// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_GALLERY_GALLERY_WINDOW_H_
#define SHELL_GALLERY_GALLERY_WINDOW_H_

#include <filesystem>
#include <stack>

#include "ukive/window/window.h"
#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/list/list_item_event_router.h"
#include "ukive/views/click_listener.h"

#include "shell/gallery/thumbnail_fetcher.h"


namespace ukive {
    class Button;
    class ListView;
    class TextView;
    class ImageView;
    class ImageFrame;
}

namespace shell {

    class GalleryListSource;

    class GalleryWindow :
        public ukive::Window,
        public ukive::ListSource,
        public ukive::ListItemRecycledListener,
        public ukive::ListItemEventListener,
        public ukive::OnClickListener,
        public ThumbnailFetchingListener
    {
    public:
        // ukive::Window
        void onCreated() override;
        void onDestroyed() override;
        bool onInputEvent(ukive::InputEvent* e) override;

        // ukive::ListSource
        ukive::ListItem* onCreateListItem(
            ukive::LayoutView* parent, ukive::ListItemEventRouter* router,
            size_t position) override;
        void onSetListItemData(
            ukive::LayoutView* parent, ukive::ListItemEventRouter* router,
            ukive::ListItem* item) override;
        size_t onGetListDataCount(ukive::LayoutView* parent) const override;

        // ukive::ListItemRecycledListener
        void onChildRecycled(ukive::ListView* lv, ukive::ListItem* item) override;

        // ThumbnailFetchingListener
        void onThumbnail(const Thumbnail& thumb) override;

        // ukive::OnClickListener
        void onClick(ukive::View* v) override;

        // ukive::ListItemEventListener
        void onItemClicked(
            ukive::ListView* list_view, ukive::ListItem* item, ukive::View* v) override;

    private:
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

        struct Info {
            size_t position;
            int offset;
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

        void navigateTo(const std::filesystem::path& path, bool back);
        void navigateBack();

        ukive::Button* back_btn_ = nullptr;
        ukive::TextView* cur_dir_tv_ = nullptr;
        ukive::ListView* list_view_ = nullptr;

        std::filesystem::path cur_dir_;
        std::stack<Info> back_info_;

        std::vector<Data> data_;
        std::unique_ptr<ThumbnailFetcher> thumb_fetcher_;
    };

}

#endif  // SHELL_GALLERY_GALLERY_WINDOW_H_