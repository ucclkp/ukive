// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGES_LC_IMAGE_H_
#define UKIVE_GRAPHICS_IMAGES_LC_IMAGE_H_

#include <memory>
#include <vector>

#include "ukive/graphics/images/lc_image_frame.h"


namespace ukive {

    class LcImage {
    public:
        LcImage();

        void addFrame(LcImageFrame* frame);
        void removeFrame(LcImageFrame* frame, bool del);

        bool isValid() const;

        void setData(const std::shared_ptr<ImageData>& data);
        const std::shared_ptr<ImageData>& getData() const;

        Size getSize() const;
        const std::vector<LcImageFrame*>& getFrames() const;

    private:
        class ImageOntic;
        std::shared_ptr<ImageOntic> ontic_;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGES_LC_IMAGE_H_