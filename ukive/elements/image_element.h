// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_IMAGE_ELEMENT_H_
#define UKIVE_ELEMENTS_IMAGE_ELEMENT_H_

#include "ukive/elements/element.h"
#include "ukive/graphics/images/image.h"


namespace ukive {

    class ImageElement : public Element {
    public:
        enum ExtendMode {
            Clamp,
            Wrap,
        };

        explicit ImageElement(const GPtr<ImageFrame>& img);
        ~ImageElement() = default;

        void setOpacity(float opt);
        void setExtendMode(ExtendMode mode);

        void draw(Canvas* canvas) override;

        Opacity getOpacity() const override;

        int getContentWidth() const override;
        int getContentHeight() const override;

        GPtr<ImageFrame> getImage() const;

    protected:
        void onContextChanged(Context::Type type, const Context& context) override;

    private:
        float opacity_ = 1.f;
        ExtendMode mode_ = Clamp;
        GPtr<ImageFrame> image_;
    };

}

#endif  // UKIVE_ELEMENTS_IMAGE_ELEMENT_H_