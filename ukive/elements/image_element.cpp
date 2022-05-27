// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_element.h"

#include "ukive/graphics/canvas.h"
#include "ukive/window/context.h"


namespace ukive {

    ImageElement::ImageElement(const GPtr<ImageFrame>& img)
        : image_(img) {}


    void ImageElement::setOpacity(float opt) {
        opacity_ = opt;
    }

    void ImageElement::setExtendMode(ExtendMode mode) {
        mode_ = mode;
    }

    void ImageElement::draw(Canvas* canvas) {
        if (!image_) {
            return;
        }

        if (mode_ == Wrap) {
            canvas->fillImageRepeat(RectF(getBounds()), image_.get());
        } else {
            canvas->drawImage(RectF(getBounds()), opacity_, image_.get());
        }
    }

    bool ImageElement::isTransparent() const {
        if (opacity_ <= 0) {
            return true;
        }
        return !image_;
    }

    int ImageElement::getContentWidth() const {
        if (!image_) {
            return 0;
        }
        return int(std::ceil(image_->getSize().width()));
    }

    int ImageElement::getContentHeight() const {
        if (!image_) {
            return 0;
        }
        return int(std::ceil(image_->getSize().height()));
    }

    GPtr<ImageFrame> ImageElement::getImage() const {
        return image_;
    }

    void ImageElement::onContextChanged(Context::Type type, const Context& context) {
    }

}