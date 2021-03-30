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
#include "ukive/views/list/list_view.h"
#include "ukive/views/click_listener.h"


namespace ukive {
    class Button;
    class ListView;
    class TextView;
}

namespace shell {

    class GalleryListSource;

    class GalleryWindow :
        public ukive::Window,
        public ukive::ListItemSelectedListener,
        public ukive::OnClickListener
    {
    public:
        // ukive::Window
        void onCreated() override;
        bool onInputEvent(ukive::InputEvent* e) override;

        // ukive::OnClickListener
        void onClick(ukive::View* v) override;

        // ukive::ListItemSelectedListener
        void onItemClicked(ukive::ListView* lv, ukive::ListItem* item) override;

    private:
        struct Info {
            int position;
            int offset;
        };

        void navigateTo(const std::filesystem::path& path, bool back);
        void navigateBack();

        ukive::Button* back_btn_ = nullptr;
        ukive::TextView* cur_dir_tv_ = nullptr;
        ukive::ListView* list_view_ = nullptr;
        GalleryListSource* list_source_ = nullptr;

        std::filesystem::path cur_dir_;
        std::stack<Info> back_info_;
    };

}

#endif  // SHELL_GALLERY_GALLERY_WINDOW_H_