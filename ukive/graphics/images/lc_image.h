// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGES_LC_IMAGE_H_
#define UKIVE_GRAPHICS_IMAGES_LC_IMAGE_H_

#include <memory>
#include <vector>

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/images/lc_image_frame.h"


namespace ukive {

    class LcImage {
    public:
        LcImage();

        void addFrame(const GPtr<LcImageFrame>& frame);
        void removeFrame(const GPtr<LcImageFrame>& frame);
        void clearFrames();

        bool isValid() const;

        void setData(const std::shared_ptr<ImageData>& data);
        const std::shared_ptr<ImageData>& getData() const;

        SizeF getBounds() const;
        SizeU getPixelBounds() const;
        const std::vector<GPtr<LcImageFrame>>& getFrames() const;

    private:
        class ImageOntic;
        std::shared_ptr<ImageOntic> ontic_;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGES_LC_IMAGE_H_