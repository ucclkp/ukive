// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_GALLERY_PICTURE_PICTURE_WINDOW_H_
#define SHELL_GALLERY_PICTURE_PICTURE_WINDOW_H_

#include <vector>

#include "ukive/window/window.h"
#include "ukive/views/click_listener.h"


namespace ukive {
    class Button;
    class TextView;
    class ImageView;
}

namespace shell {

    class SlideImageView;

    class PictureWindow : public ukive::Window, public ukive::OnClickListener {
    public:
        // ukive::Window
        void onCreated() override;

        // ukive::OnClickListener
        void onClick(ukive::View* v) override;

    private:
        void initData();
        bool checkReg();
        bool associateExts();
        bool unassociateExts();

        ukive::Button* asso_btn_ = nullptr;
        ukive::Button* unasso_btn_ = nullptr;
        SlideImageView* gallery_view_ = nullptr;
        std::vector<std::u16string> allowed_exts_;
    };

}

#endif  // SHELL_GALLERY_PICTURE_PICTURE_WINDOW_H_