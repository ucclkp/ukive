// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/images/image.h"

#include <vector>

#include "utils/stl_utils.h"


namespace ukive {

    class Image::ImageData {
    public:
        ~ImageData() {
            utl::STLDeleteElements(&frames);
        }

        std::vector<ImageFrame*> frames;
    };

}

namespace ukive {

    Image::Image() {}

    void Image::addFrame(ImageFrame* frame) {
        if (!data_) {
            data_ = std::make_shared<ImageData>();
        }
        data_->frames.push_back(frame);
    }

    void Image::removeFrame(ImageFrame* frame, bool del) {
        if (!data_ || !frame) {
            return;
        }

        auto& frames = data_->frames;
        for (auto it = frames.begin(); it != frames.end();) {
            if (*it == frame) {
                it = frames.erase(it);
            } else {
                ++it;
            }
        }

        if (del) {
            delete frame;
        }

        if (frames.empty()) {
            data_.reset();
        }
    }

    void Image::clear() {
        if (data_) {
            data_->frames.clear();
            data_.reset();
        }
    }

    bool Image::isValid() const {
        return !!data_;
    }

    SizeF Image::getBoundSize() const {
        if (!data_) {
            return {};
        }

        SizeF size;
        for (const auto frame : data_->frames) {
            size.setToMax(frame->getSize());
        }
        return size;
    }

    const std::vector<ImageFrame*>& Image::getFrames() const {
        static std::vector<ImageFrame*> stub;
        if (!data_) {
            return stub;
        }
        return data_->frames;
    }

}
