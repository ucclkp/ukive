// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/images/image.h"

#include <vector>


namespace ukive {

    class Image::ImageOntic {
    public:
        std::vector<GPtr<ImageFrame>> frames;
        std::shared_ptr<ImageData> data;
    };

}

namespace ukive {

    Image::Image() {}

    void Image::addFrame(const GPtr<ImageFrame>& frame) {
        if (!ontic_) {
            ontic_ = std::make_shared<ImageOntic>();
        }
        ontic_->frames.push_back(frame);
    }

    void Image::removeFrame(const GPtr<ImageFrame>& frame) {
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

        if (frames.empty()) {
            ontic_.reset();
        }
    }

    void Image::removeFrameAt(size_t index) {
        if (!ontic_) {
            return;
        }

        auto& frames = ontic_->frames;
        if (frames.size() <= index) {
            return;
        }

        auto frame = *(frames.begin() + index);
        frames.erase(frames.begin() + index);

        if (frames.empty()) {
            ontic_.reset();
        }
    }

    void Image::clearFrames() {
        if (!ontic_) {
            return;
        }

        ontic_->frames.clear();
        ontic_.reset();
    }

    void Image::setData(const std::shared_ptr<ImageData>& data) {
        if (!ontic_) {
            ontic_ = std::make_shared<ImageOntic>();
        }
        ontic_->data = data;
    }

    const std::shared_ptr<ImageData>& Image::getData() const {
        static std::shared_ptr<ImageData> stub;
        if (!ontic_) {
            return stub;
        }
        return ontic_->data;
    }

    bool Image::isValid() const {
        if (!ontic_) {
            return false;
        }
        return !ontic_->frames.empty();
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

    const std::vector<GPtr<ImageFrame>>& Image::getFrames() const {
        static std::vector<GPtr<ImageFrame>> stub;
        if (!ontic_) {
            return stub;
        }
        return ontic_->frames;
    }

}
