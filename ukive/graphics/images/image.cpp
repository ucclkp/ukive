// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/images/image.h"

#include <vector>

#include "utils/stl_utils.h"


namespace ukive {

    class Image::ImageOntic {
    public:
        ~ImageOntic() {
            utl::STLDeleteElements(&frames);
        }

        std::vector<ImageFrame*> frames;
    };

}

namespace ukive {

    Image::Image() {}

    void Image::addFrame(ImageFrame* frame) {
        if (!ontic_) {
            ontic_ = std::make_shared<ImageOntic>();
        }
        ontic_->frames.push_back(frame);
    }

    void Image::removeFrame(ImageFrame* frame, bool del) {
        if (!ontic_ || !frame) {
            return;
        }

        auto& frames = ontic_->frames;
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
            ontic_.reset();
        }
    }

    void Image::clearFrames(bool del) {
        if (!ontic_) {
            return;
        }

        if (del) {
            utl::STLDeleteElements(&ontic_->frames);
        }

        ontic_->frames.clear();
        ontic_.reset();
    }

    bool Image::isValid() const {
        return !!ontic_;
    }

    SizeF Image::getBounds() const {
        if (!ontic_) {
            return {};
        }

        SizeF size;
        for (const auto frame : ontic_->frames) {
            size.setToMax(frame->getSize());
        }
        return size;
    }

    SizeU Image::getPixelBounds() const {
        if (!ontic_) {
            return {};
        }

        SizeU size;
        for (const auto frame : ontic_->frames) {
            size.setToMax(frame->getPixelSize());
        }
        return size;
    }

    const std::vector<ImageFrame*>& Image::getFrames() const {
        static std::vector<ImageFrame*> stub;
        if (!ontic_) {
            return stub;
        }
        return ontic_->frames;
    }

}
