// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shell/gallery/picture/slide_image_view.h"

#include <algorithm>
#include <filesystem>

#include "utils/numbers.hpp"
#include "utils/strings/string_utils.hpp"

#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/images/image_options.h"
#include "shell/gallery/gallery_utils.h"
#include "ukive/event/input_event.h"
#include "ukive/elements/chessboard_element.h"
#include "ukive/window/window.h"


namespace shell {

    namespace fs = std::filesystem;

    SlideImageView::SlideImageView(ukive::Context c)
        : ImageView(c),
          cur_index_(0)
    {
        setScaleType(FIT_ALWAYS);
        setBackground(new ukive::ChessboardElement(c));
    }

    void SlideImageView::setFirstImageFile(const std::u16string& img_file) {
        setImage(
            ukive::ImageFrame::decodeFile(
                getWindow()->getCanvas(), img_file, ukive::ImageOptions()));

        files_.clear();
        std::error_code ec;
        for (auto& f : fs::directory_iterator(fs::path(img_file).parent_path(), ec)) {
            auto name(f.path().filename().u16string());
            if (f.is_directory(ec) || name == u"." || name == u"..") {
                continue;
            }
            if (!checkFileExt(f.path().extension().u16string())) {
                continue;
            }
            files_.push_back({ f.path().u16string(), name });
        }

        std::sort(
            files_.begin(), files_.end(),
            [](const Info& f1, const Info& f2)->bool
        {
            int ret = compareLingString(f1.name, f2.name);
            return ret == 1;
        });

        int i = 0;
        bool hit = false;
        for (const auto& f : files_) {
            if (f.path == img_file) {
                hit = true;
                break;
            }
            ++i;
        }
        if (hit) {
            cur_index_ = i;
        } else {
            ubassert(false);
        }
    }

    void SlideImageView::setImageFileExts(const std::vector<std::u16string>& exts) {
        exts_ = exts;
    }

    void SlideImageView::prevImage() {
        if (cur_index_ > 0) {
            --cur_index_;
            setImage(
                ukive::ImageFrame::decodeFile(
                    getWindow()->getCanvas(), files_[cur_index_].path, ukive::ImageOptions()));
        }
    }

    void SlideImageView::nextImage() {
        if (cur_index_ + 1 < utl::num_cast<int>(files_.size())) {
            ++cur_index_;
            setImage(
                ukive::ImageFrame::decodeFile(
                    getWindow()->getCanvas(), files_[cur_index_].path, ukive::ImageOptions()));
        }
    }

    bool SlideImageView::checkFileExt(const std::u16string& ext) {
        for (const auto& allowed_ext : exts_) {
            if (utl::isLitEqual(ext, allowed_ext)) {
                return true;
            }
        }
        return false;
    }

    bool SlideImageView::onInputEvent(ukive::InputEvent* e) {
        ImageView::onInputEvent(e);

        switch (e->getEvent()) {
        case ukive::InputEvent::EVM_DOWN:
        case ukive::InputEvent::EVT_DOWN:
            is_down_ = true;
            down_x_ = e->getX();
            down_y_ = e->getY();
            return true;

        case ukive::InputEvent::EVT_MULTI_DOWN:
            break;

        case ukive::InputEvent::EVT_MULTI_UP:
            break;

        case ukive::InputEvent::EVM_MOVE:
        case ukive::InputEvent::EVT_MOVE:
            if (is_down_ &&
                std::abs(e->getX() - down_x_) >= getContext().dp2px(8) &&
                std::abs(e->getY() - down_y_) >= getContext().dp2px(8))
            {

            }
            break;

        case ukive::InputEvent::EVM_UP:
        case ukive::InputEvent::EVT_UP:
            is_down_ = false;
            if (std::abs(e->getX() - down_x_) < getContext().dp2px(8) &&
                std::abs(e->getY() - down_y_) < getContext().dp2px(8))
            {
                if (e->getX() >= getWidth() / 2) {
                    nextImage();
                } else {
                    prevImage();
                }
            }
            break;

        case ukive::InputEvent::EVM_WHEEL:
            if (e->getWheelValue() < 0) {
                nextImage();
            } else if (e->getWheelValue() > 0) {
                prevImage();
            }
            return true;

        case ukive::InputEvent::EV_LEAVE_VIEW:
            is_down_ = false;
            break;

        default:
            break;
        }

        return false;
    }

}