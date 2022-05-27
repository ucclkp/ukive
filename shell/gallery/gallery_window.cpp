// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shell/gallery/gallery_window.h"

#include <algorithm>

#include "utils/files/file_utils.h"
#include "utils/numbers.hpp"
#include "utils/strings/string_utils.hpp"

#include "ukive/app/application.h"
#include "ukive/elements/ripple_element.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/resources/layout_parser.h"
#include "ukive/views/image_view.h"
#include "ukive/views/list/grid_list_layouter.h"
#include "ukive/views/button.h"

#include "shell/resources/necro_resources_id.h"
#include "shell/gallery/gallery_utils.h"
#include "shell/gallery/picture/picture_window.h"

#define ROOT_PATH  L""
//#define ROOT_PATH  L""


namespace shell {

    namespace fs = std::filesystem;

    // ExampleListItem
    GalleryWindow::GalleryListItem::GalleryListItem(ukive::View* v)
        : ListItem(v)
    {
        title_view_ = static_cast<ukive::TextView*>(v->findView(Res::Id::tv_gallery_item_title));
        img_view_ = static_cast<ukive::ImageView*>(v->findView(Res::Id::iv_gallery_item_img));
    }

    void GalleryWindow::onCreated() {
        Window::onCreated();
        setContentView(Res::Layout::gallery_window_layout_xml);

        showTitleBar();

        thumb_fetcher_ = std::make_unique<ThumbnailFetcher>();

        back_btn_ = findView<ukive::Button>(Res::Id::bt_gallery_window_back);
        back_btn_->setOnClickListener(this);

        cur_dir_tv_ = findView<ukive::TextView>(Res::Id::tv_gallery_window_name);

        list_view_ = findView<ukive::ListView>(Res::Id::lv_gallery_window_list);
        list_view_->setLayouter(new ukive::GridListLayouter(5));
        list_view_->setSource(this);
        list_view_->setChildRecycledListener(this);
        list_view_->setItemEventRouter(new ukive::ListItemEventRouter(this));

        thumb_fetcher_->setListener(this);
        thumb_fetcher_->launch();

        navigateTo(fs::path(ROOT_PATH), false);
    }

    void GalleryWindow::onDestroyed() {
        thumb_fetcher_->shutdown();
    }

    bool GalleryWindow::onInputEvent(ukive::InputEvent* e) {
        switch (e->getEvent()) {
        case ukive::InputEvent::EVM_UP:
            if (e->getMouseKey() == ukive::InputEvent::MK_XBUTTON_1) {
                navigateBack();
            }
            break;
        default:
            break;
        }

        bool result = Window::onInputEvent(e);
        if (result) {
            return result;
        }

        return result;
    }

    ukive::ListItem* GalleryWindow::onCreateListItem(
        ukive::LayoutView* parent, ukive::ListItemEventRouter* router, size_t position)
    {
        auto view = ukive::LayoutParser::from(
            parent->getContext(), parent, Res::Layout::gallery_item_layout_xml);
        view->setClickable(true);
        view->setOnClickListener(router);

        auto bg = new ukive::RippleElement();
        bg->setSolidColor(ukive::Color::White);
        bg->setSolidEnable(true);
        view->setBackground(bg);

        return new GalleryListItem(view);
    }

    void GalleryWindow::onSetListItemData(
        ukive::LayoutView* parent, ukive::ListItemEventRouter* router,
        ukive::ListItem* item)
    {
        auto gallery_list_item = static_cast<GalleryListItem*>(item);

        auto& d = data_[item->data_pos];
        if (d.ts == ThumbStatus::None) {
            thumb_fetcher_->add(d.path, !d.is_dir, utl::num_cast<int>(item->data_pos), d.token);
            d.ts = ThumbStatus::Fetching;
        }

        gallery_list_item->title_view_->setText(d.title);
        gallery_list_item->img_view_->setImage(d.img);
        gallery_list_item->item_view->getBackground()->resetState();
    }

    size_t GalleryWindow::onGetListDataCount(ukive::LayoutView* parent) const {
        return data_.size();
    }

    void GalleryWindow::onChildRecycled(ukive::ListView* lv, ukive::ListItem* item) {
        auto pos = item->data_pos;
        if (pos >= onGetListDataCount(lv)) {
            return;
        }

        auto& d = data_[pos];
        if (d.ts == ThumbStatus::Fetching) {
            d.token = std::make_shared<int>();
            d.ts = ThumbStatus::None;
        }
    }

    void GalleryWindow::onThumbnail(const Thumbnail& thumb) {
        if (!thumb.w_token.lock()) {
            return;
        }

        auto img = ukive::ImageFrame::create(
            list_view_->getWindow()->getCanvas(), thumb.ph_bmp);

        auto& d = data_[thumb.id];
        d.ts = ThumbStatus::Complete;
        d.img = img;
        notifyDataChanged();
    }

    void GalleryWindow::onClick(ukive::View* v) {
        if (v == back_btn_) {
            navigateBack();
        }
    }

    void GalleryWindow::onItemClicked(
        ukive::ListView* list_view, ukive::ListItem* item, ukive::View* v)
    {
        auto& data = data_[item->data_pos];

        std::error_code ec;
        auto next_file(std::filesystem::absolute(data.path, ec));
        if (!is_directory(next_file, ec)) {
            auto slide_window = new PictureWindow();
            slide_window->setTitle(u"XPicture");
            slide_window->setWidth(ukive::Application::dp2pxi(600));
            slide_window->setHeight(ukive::Application::dp2pxi(600));
            slide_window->setOwnership(true);
            {
                ukive::Purpose purpose;
                purpose.params["target"] = data.path;
                slide_window->setPurpose(purpose);
            }
            slide_window->center();
            slide_window->init(InitParams());
            slide_window->maximize();
        }
        else {
            navigateTo(next_file, false);
        }
    }

    void GalleryWindow::navigateTo(const fs::path& path, bool back) {
        struct Info {
            std::u16string path;
            std::u16string name;
        };

        std::error_code ec;
        std::vector<Info> dirs;
        std::vector<Info> files;
        for (auto& entry : fs::directory_iterator(path, ec)) {
            if (entry.is_directory(ec)) {
                auto name(entry.path().filename().u16string());
                if (name == u"." || name == u"..") {
                    continue;
                }
                dirs.push_back({ entry.path().u16string(), name });
            } else {
                if (isImageExtension(entry.path().extension().u16string())) {
                    files.push_back({ entry.path().u16string(), entry.path().filename().u16string() });
                }
            }
        }
        if (dirs.empty() && files.empty()) {
            return;
        }

        cur_dir_ = path;
        cur_dir_tv_->setText(cur_dir_.filename().u16string());

        if (!back && data_.size() > 0) {
            size_t pos;
            int offset;
            list_view_->getCurPosition(&pos, &offset);
            back_info_.push({ pos, offset });
        }

        std::sort(
            dirs.begin(), dirs.end(),
            [](const Info& f1, const Info& f2)->bool
        {
            int ret = compareLingString(f1.name, f2.name);
            return ret == 1;
        });
        std::sort(
            files.begin(), files.end(),
            [](const Info& f1, const Info& f2)->bool
        {
            int ret = compareLingString(f1.name, f2.name);
            return ret == 1;
        });

        data_.clear();
        for (const auto& f : dirs) {
            Data d;
            d.title = f.name;
            d.path = f.path;
            d.is_dir = true;
            data_.push_back(std::move(d));
        }
        for (const auto& f : files) {
            Data d;
            d.title = f.name;
            d.path = f.path;
            d.is_dir = false;
            data_.push_back(std::move(d));
        }

        if (back && !back_info_.empty()) {
            auto& info = back_info_.top();
            list_view_->scrollToPosition(info.position, info.offset, false);
            back_info_.pop();
        } else {
            list_view_->scrollToPosition(0, 0, false);
        }
    }

    void GalleryWindow::navigateBack() {
        if (cur_dir_ != ROOT_PATH) {
            auto prev_dir = cur_dir_.parent_path();
            navigateTo(prev_dir, true);
        }
    }

}
