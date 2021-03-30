// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_GALLERY_PICTURE_SLIDE_IMAGE_VIEW_H_
#define SHELL_GALLERY_PICTURE_SLIDE_IMAGE_VIEW_H_

#include <vector>

#include "ukive/views/image_view.h"


namespace shell {

    class SlideImageView : public ukive::ImageView {
    public:
        explicit SlideImageView(ukive::Context c);

        void setFirstImageFile(const std::u16string& img_file);
        void setImageFileExts(const std::vector<std::u16string>& exts);

    protected:
        bool onInputEvent(ukive::InputEvent* e) override;

    private:
        struct Info {
            std::u16string path;
            std::u16string name;
        };

        void prevImage();
        void nextImage();
        bool checkFileExt(const std::u16string& ext);

        int cur_index_;
        std::vector<Info> files_;
        std::vector<std::u16string> exts_;

        int down_x_ = 0;
        int down_y_ = 0;
        bool is_down_ = false;
    };

}

#endif  // SHELL_GALLERY_PICTURE_SLIDE_IMAGE_VIEW_H_