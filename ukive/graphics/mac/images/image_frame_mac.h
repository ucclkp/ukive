// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_IMAGES_IMAGE_FRAME_MAC_H_
#define UKIVE_GRAPHICS_MAC_IMAGES_IMAGE_FRAME_MAC_H_

#include "utils/mac/objc_utils.hpp"

#include "ukive/graphics/byte_data.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/gref_count_impl.h"

UTL_OBJC_CLASS(NSBitmapImageRep);


namespace ukive {
namespace mac {

    class ImageFrameMac :
        public ImageFrame,
        public GRefCountImpl
    {
    public:
        explicit ImageFrameMac(
            const ImageOptions& options,
            NSBitmapImageRep* img,
            bool already_flipped,
            const GPtr<ByteData>& buf);
        ~ImageFrameMac();

        void setDpi(float dpi_x, float dpi_y) override;
        void getDpi(float *dpi_x, float *dpi_y) const override;

        SizeF getSize() const override;
        SizeU getPixelSize() const override;

        bool alreadyFilpped() const;
        NSBitmapImageRep* getNative() const;

    private:
        GPtr<ByteData> buf_;
        bool already_flipped_;
        NSBitmapImageRep* img_;
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_IMAGES_IMAGE_FRAME_MAC_H_
