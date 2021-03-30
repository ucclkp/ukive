// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shell/gallery/gallery_window.h"

#include <algorithm>

#include "utils/files/file_utils.h"
#include "utils/string_utils.h"

#include "ukive/app/application.h"
#include "ukive/views/list/grid_list_layouter.h"
#include "ukive/views/button.h"

#include "shell/gallery/gallery_list_source.h"
#include "shell/resources/necro_resources_id.h"
#include "shell/gallery/gallery_utils.h"
#include "shell/gallery/picture/picture_window.h"

#define ROOT_PATH  L""
//#define ROOT_PATH  L""


namespace shell {

    namespace fs = std::filesystem;

    void GalleryWindow::onCreated() {
        Window::onCreated();
        setContentView(Res::Layout::gallery_window_layout_xml);

        showTitleBar();

        back_btn_ = findView<ukive::Button>(Res::Id::bt_gallery_window_back);
        back_btn_->setOnClickListener(this);

        cur_dir_tv_ = findView<ukive::TextView>(Res::Id::tv_gallery_window_name);

        list_view_ = findView<ukive::ListView>(Res::Id::lv_gallery_window_list);

        list_source_ = new GalleryListSource(list_view_);

        list_view_->setLayouter(new ukive::GridListLayouter(5));
        list_view_->setSource(list_source_);
        list_view_->setItemSelectedListener(this);

        navigateTo(fs::path(ROOT_PATH), false);
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

    void GalleryWindow::onClick(ukive::View* v) {
        if (v == back_btn_) {
            navigateBack();
        }
    }

    void GalleryWindow::onItemClicked(ukive::ListView* lv, ukive::ListItem* item) {
        auto pos = item->data_pos;
        auto& data = list_source_->getItem(pos);

        std::error_code ec;
        auto next_file(std::filesystem::absolute(data.path, ec));
        if (!fs::is_directory(next_file, ec)) {
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
        } else {
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

        if (!back && list_source_->onListGetDataCount() > 0) {
            int pos, offset;
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

        list_source_->clearItems();
        for (const auto& f : dirs) {
            list_source_->addItem(f.name, f.path, true);
        }
        for (const auto& f : files) {
            list_source_->addItem(f.name, f.path, false);
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
