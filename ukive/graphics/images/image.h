// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGES_IMAGE_H_
#define UKIVE_GRAPHICS_IMAGES_IMAGE_H_

#include <memory>
#include <vector>

#include "ukive/graphics/images/image_frame.h"


namespace ukive {

    class Image {
    public:
        Image();

        void addFrame(ImageFrame* frame);
        void removeFrame(ImageFrame* frame, bool del);
        void clearFrames(bool del);

        bool isValid() const;

        SizeF getBounds() const;
        SizeU getPixelBounds() const;
        const std::vector<ImageFrame*>& getFrames() const;

    private:
        class ImageOntic;
        std::shared_ptr<ImageOntic> ontic_;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGES_IMAGE_H_
