// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_CYRO_RENDER_TARGET_H_
#define UKIVE_GRAPHICS_CYRO_RENDER_TARGET_H_

#include "ukive/graphics/gptr.hpp"


namespace ukive {

    class ImageFrame;
    class GPUTexture;
    class ImageOptions;

    class CyroRenderTarget {
    public:
        virtual ~CyroRenderTarget() = default;

        virtual GPtr<ImageFrame> createSharedImageFrame(
            const GPtr<GPUTexture>& texture, const ImageOptions& options) = 0;
        virtual void destroy() = 0;
    };

}

#endif  // UKIVE_GRAPHICS_CYRO_RENDER_TARGET_H_