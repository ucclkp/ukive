// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/images/lc_image.h"

#include <vector>

#include "utils/stl_utils.h"


namespace ukive {

    class LcImage::ImageOntic {
    public:
        ~ImageOntic() {
            utl::STLDeleteElements(&frames);
        }

        std::vector<LcImageFrame*> frames;
        std::shared_ptr<ImageData> data;
    };

}

namespace ukive {

    LcImage::LcImage() {}

    void LcImage::addFrame(LcImageFrame* frame) {
        if (!ontic_) {
            ontic_ = std::make_shared<ImageOntic>();
        }
        ontic_->frames.push_back(frame);
    }

    void LcImage::removeFrame(LcImageFrame* frame, bool del) {
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

    void LcImage::setData(const std::shared_ptr<ImageData>& data) {
        if (!ontic_) {
            ontic_ = std::make_shared<ImageOntic>();
        }
        ontic_->data = data;
    }

    const std::shared_ptr<ImageData>& LcImage::getData() const {
        static std::shared_ptr<ImageData> stub;
        if (!ontic_) {
            return stub;
        }
        return ontic_->data;
    }

    bool LcImage::isValid() const {
        if (!ontic_) {
            return false;
        }
        return !ontic_->frames.empty();
    }

    Size LcImage::getSize() const {
        if (!ontic_) {
            return {};
        }

        Size size;
        for (const auto frame : ontic_->frames) {
            size.setToMax(frame->getSize());
        }
        return size;
    }

    const std::vector<LcImageFrame*>& LcImage::getFrames() const {
        static std::vector<LcImageFrame*> stub;
        if (!ontic_) {
            return stub;
        }
        return ontic_->frames;
    }

}
