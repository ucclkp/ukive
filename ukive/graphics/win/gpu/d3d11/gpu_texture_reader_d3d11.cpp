// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/gpu/gpu_texture_reader.h"

#include <cstdint>

#include <d3d11.h>
#include <wincodec.h>

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/app/application.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_device_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_texture_d3d11.h"
#include "ukive/graphics/win/images/lc_image_frame_win.h"
#include "ukive/graphics/win/images/lc_image_factory_win.h"


namespace {

    DXGI_FORMAT mapWICFormatToDXGIFormat(const WICPixelFormatGUID& wic_format) {
        if (wic_format == GUID_WICPixelFormat32bppBGRA) {
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        } else if (wic_format == GUID_WICPixelFormat32bppPBGRA) {
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        } else if (wic_format == GUID_WICPixelFormat32bppRGBA) {
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        } else if (wic_format == GUID_WICPixelFormat32bppPRGBA) {
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        } else if (wic_format == GUID_WICPixelFormat32bppBGR) {
            return DXGI_FORMAT_B8G8R8X8_UNORM;
        } else if (wic_format == GUID_WICPixelFormat128bppRGBAFloat) {
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        } else if (wic_format == GUID_WICPixelFormat128bppPRGBAFloat) {
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        } else if (wic_format == GUID_WICPixelFormat64bppRGBAHalf) {
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        } else if (wic_format == GUID_WICPixelFormat64bppPRGBAHalf) {
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        } else if (wic_format == GUID_WICPixelFormat64bppRGBA) {
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        } else if (wic_format == GUID_WICPixelFormat64bppPRGBA) {
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        } else if (wic_format == GUID_WICPixelFormat32bppRGBA1010102XR) {
            return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
        } else if (wic_format == GUID_WICPixelFormat32bppR10G10B10A2 ||
            wic_format == GUID_WICPixelFormat32bppR10G10B10A2HDR10)
        {
            return DXGI_FORMAT_R10G10B10A2_UNORM;
        } else if (wic_format == GUID_WICPixelFormat32bppRGBE) {
            return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
        } else if (wic_format == GUID_WICPixelFormat16bppBGRA5551) {
            return DXGI_FORMAT_B5G5R5A1_UNORM;
        } else if (wic_format == GUID_WICPixelFormat16bppBGR565) {
            return DXGI_FORMAT_B5G6R5_UNORM;
        } else if (wic_format == GUID_WICPixelFormat32bppGrayFloat) {
            return DXGI_FORMAT_R32_FLOAT;
        } else if (wic_format == GUID_WICPixelFormat16bppGrayHalf) {
            return DXGI_FORMAT_R16_FLOAT;
        } else if (wic_format == GUID_WICPixelFormat16bppGray) {
            return DXGI_FORMAT_R16_UNORM;
        } else if (wic_format == GUID_WICPixelFormat8bppGray) {
            return DXGI_FORMAT_R8_UNORM;
        } else if (wic_format == GUID_WICPixelFormat8bppAlpha) {
            return DXGI_FORMAT_A8_UNORM;
        } else if (wic_format == GUID_WICPixelFormat96bppRGBFloat) {
            return DXGI_FORMAT_R32G32B32_FLOAT;
        }
        return DXGI_FORMAT_UNKNOWN;
    }

}

namespace ukive {
namespace win {

    HRESULT CreateD3DTextureFromWIC(
        ID3D11Device* device,
        IWICBitmap* bmp,
        uint32_t flags,
        unsigned int* tex_width,
        unsigned int* tex_height,
        ID3D11Texture2D** texture,
        ID3D11ShaderResourceView** srv)
    {
        // 获取图片大小
        UINT width, height;
        HRESULT hr = bmp->GetSize(&width, &height);
        if (FAILED(hr)) {
            return hr;
        }

        // 创建 Texture2D
        utl::win::ComPtr<ID3D11DeviceContext> context;
        if ((flags & TRF_GEN_MIPMAP) && srv) {
            device->GetImmediateContext(&context);
        }

        WICPixelFormatGUID wic_format;
        hr = bmp->GetPixelFormat(&wic_format);
        if (FAILED(hr)) {
            return hr;
        }

        DXGI_FORMAT dxgi_format = mapWICFormatToDXGIFormat(wic_format);

        bool using_mipmap = false;
        if (context) {
            UINT supported = 0;
            hr = device->CheckFormatSupport(dxgi_format, &supported);
            if (SUCCEEDED(hr) && (supported & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN)) {
                using_mipmap = true;
            }
        }

        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.Width = width;
        tex_desc.Height = height;
        tex_desc.ArraySize = 1;
        if (using_mipmap) {
            tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
            tex_desc.MipLevels = 0;
        } else {
            tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            tex_desc.MiscFlags = 0;
            tex_desc.MipLevels = 1;
        }
        tex_desc.Format = dxgi_format;
        tex_desc.CPUAccessFlags = 0;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.Usage = D3D11_USAGE_DEFAULT;

        WICRect lock_rect = { 0, 0, INT(width), INT(height) };
        utl::win::ComPtr<IWICBitmapLock> bmp_lock;
        hr = bmp->Lock(&lock_rect, WICBitmapLockRead, &bmp_lock);
        if (FAILED(hr)) {
            return hr;
        }

        UINT data_size;
        WICInProcPointer pixels;
        hr = bmp_lock->GetDataPointer(&data_size, &pixels);
        if (FAILED(hr)) {
            return hr;
        }

        UINT stride;
        hr = bmp_lock->GetStride(&stride);
        if (FAILED(hr)) {
            return hr;
        }

        D3D11_SUBRESOURCE_DATA tex_data;
        tex_data.pSysMem = pixels;
        tex_data.SysMemPitch = stride;
        tex_data.SysMemSlicePitch = 0;

        ID3D11Texture2D* _texture;
        hr = device->CreateTexture2D(&tex_desc, using_mipmap ? nullptr : &tex_data, &_texture);
        if (FAILED(hr)) {
            return hr;
        }

        ID3D11ShaderResourceView* _srv;
        if (srv) {
            D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
            SRVDesc.Format = dxgi_format;
            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            if (using_mipmap) {
                SRVDesc.Texture2D.MipLevels = -1;
            } else {
                SRVDesc.Texture2D.MipLevels = 1;
            }

            hr = device->CreateShaderResourceView(_texture, &SRVDesc, &_srv);
            if (FAILED(hr)) {
                _texture->Release();
                return hr;
            }

            if (using_mipmap) {
                context->UpdateSubresource(
                    _texture, 0, nullptr, tex_data.pSysMem, tex_data.SysMemPitch, tex_data.SysMemSlicePitch);
                context->GenerateMips(_srv);
            }
            *srv = _srv;
        }

        if (tex_width) {
            *tex_width = width;
        }
        if (tex_height) {
            *tex_height = height;
        }

        if (texture) {
            *texture = _texture;
        } else {
            _texture->Release();
        }
        return S_OK;

    }

}

    GPtr<GPUTexture> CreateGPUTexture2DFromLcImage(
        const GPtr<GPUDevice>& device,
        const GPtr<LcImageFrame>& source,
        uint32_t flags)
    {
        auto dev = device.cast<win::GPUDeviceD3D11>()->getNative();
        auto src = source.cast<win::LcImageFrameWin>();

        int ret = src->createIfNecessary();
        if (ret != 0) {
            return {};
        }

        auto bmp = src->getNative();

        unsigned int width, height;
        utl::win::ComPtr<ID3D11Texture2D> d2d_tex;
        HRESULT hr = win::CreateD3DTextureFromWIC(
            dev.get(),
            bmp.get(),
            flags,
            &width,
            &height,
            &d2d_tex,
            // TODO:
            nullptr);
        if (FAILED(hr)) {
            return {};
        }

        auto ptr = new win::GPUTexture2DD3D11(d2d_tex);
        return GPtr<GPUTexture>(ptr);
    }

}