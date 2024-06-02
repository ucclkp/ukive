// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_TEXTURE_READER_H_
#define UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_TEXTURE_READER_H_

#include <cstdint>

#include <d3d11.h>
#include <wincodec.h>


namespace ukive {
namespace win {

    enum TextureReadingFlags {
        TRF_NONE = 0,
        TRF_SRGB = 1u << 0,
        TRF_GEN_MIPMAP = 1u << 1,
    };

    HRESULT CreateD3DTextureFromWIC(
        ID3D11Device* device,
        IWICImagingFactory* img_factory,
        IWICBitmapSource* source,
        uint32_t flags,
        unsigned int* tex_width,
        unsigned int* tex_height,
        ID3D11Texture2D** texture,
        ID3D11ShaderResourceView** srv);

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_TEXTURE_READER_H_