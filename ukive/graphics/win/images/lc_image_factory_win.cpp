// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/images/lc_image_factory_win.h"

#include <fstream>

#include <Icm.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <VersionHelpers.h>

#include "utils/log.h"
#include "utils/numbers.hpp"
#include "utils/strings/string_utils.hpp"

#include "ukive/graphics/images/jpeg_appmarker_parser.h"
#include "ukive/graphics/images/lc_image.h"
#include "ukive/graphics/win/colors/color_manager_win.h"
#include "ukive/graphics/win/display_win.h"
#include "ukive/graphics/win/images/image_options_win_utils.h"
#include "ukive/graphics/win/images/lc_image_frame_win.h"
#include "ukive/window/window_dpi_utils.h"
#include <ukive/graphics/win/images/mstream.hpp>


namespace ukive {

    GUID mapImageContainer(ImageContainer container) {
        switch (container) {
        case ImageContainer::PNG:
            return GUID_ContainerFormatPng;
        case ImageContainer::JPEG:
            return GUID_ContainerFormatJpeg;
        case ImageContainer::BMP:
            return GUID_ContainerFormatBmp;
        default:
            ubassert(false);
            return GUID_ContainerFormatPng;
        }
    }

}

namespace ukive {
namespace win {

    LcImageFactoryWin::LcImageFactoryWin()
    {}

    bool LcImageFactoryWin::initialize() {
        static bool is_win8 = ::IsWindows8OrGreater();

        HRESULT hr = CoCreateInstance(
            is_win8 ? CLSID_WICImagingFactory : CLSID_WICImagingFactory1,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wic_factory_));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create WIC factory: " << hr;
            return false;
        }
        return true;
    }

    void LcImageFactoryWin::destroy() {
        wic_factory_.reset();
    }

    GPtr<LcImageFrame> LcImageFactoryWin::create(
        int width, int height, const ImageOptions& options)
    {
        if (width <= 0 || height <= 0) {
            ubassert(false);
            return {};
        }

        auto format = mapWICFormat(options);

        utl::win::ComPtr<IWICBitmap> bmp;
        HRESULT hr = wic_factory_->CreateBitmap(
            width, height, format,
            WICBitmapCacheOnDemand, &bmp);
        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }

        switch (options.dpi_type) {
        case ImageDPIType::SPECIFIED:
            bmp->SetResolution(options.dpi_x, options.dpi_y);
            break;

        case ImageDPIType::DEFAULT:
        default:
            bmp->SetResolution(kDefaultDpi, kDefaultDpi);
            break;
        }

        return GPtr<LcImageFrame>(
            new LcImageFrameWin(options, {}, wic_factory_, bmp));
    }

    GPtr<LcImageFrame> LcImageFactoryWin::create(
        int width, int height,
        const GPtr<ByteData>& pixel_data, size_t stride,
        const ImageOptions& options)
    {
        if (width <= 0 || height <= 0) {
            ubassert(false);
            return {};
        }

        auto format = mapWICFormat(options);

        auto owned_data = pixel_data->substantiate();

        utl::win::ComPtr<IWICBitmap> bmp;
        HRESULT hr = wic_factory_->CreateBitmapFromMemory(
            width, height, format,
            utl::num_cast<UINT>(stride),
            utl::num_cast<UINT>(owned_data->getSize()),
            static_cast<BYTE*>(owned_data->getData()),
            &bmp);
        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }

        switch (options.dpi_type) {
        case ImageDPIType::SPECIFIED:
            bmp->SetResolution(options.dpi_x, options.dpi_y);
            break;

        case ImageDPIType::DEFAULT:
        default:
            bmp->SetResolution(kDefaultDpi, kDefaultDpi);
            break;
        }

        return GPtr<LcImageFrame>(
            new LcImageFrameWin(options, owned_data, wic_factory_, bmp));
    }

    GPtr<LcImageFrame> LcImageFactoryWin::createThumbnail(
        const std::u16string_view& file_name,
        int frame_width, int frame_height, ImageOptions* options)
    {
        if (!options) {
            return {};
        }

        GPtr<LcImageFrame> frame;
        std::wstring wfn(file_name.begin(), file_name.end());

        utl::win::ComPtr<IShellItemImageFactory> factory;
        HRESULT hr = ::SHCreateItemFromParsingName(wfn.c_str(), nullptr, IID_PPV_ARGS(&factory));
        if (SUCCEEDED(hr)) {
            HBITMAP hbmp;
            hr = factory->GetImage({ frame_width, frame_height }, SIIGBF_RESIZETOFIT, &hbmp);
            if (SUCCEEDED(hr)) {
                BITMAP bmp;
                // TODO:
                if (::GetObjectW(hbmp, sizeof(BITMAP), &bmp) && bmp.bmBitsPixel == 32) {
                    BITMAPINFO info = {};
                    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    info.bmiHeader.biWidth = bmp.bmWidth;
                    info.bmiHeader.biHeight = -bmp.bmHeight;
                    info.bmiHeader.biPlanes = bmp.bmPlanes;
                    info.bmiHeader.biBitCount = bmp.bmBitsPixel;
                    info.bmiHeader.biCompression = BI_RGB;

                    size_t buf_size = bmp.bmWidth * bmp.bmHeight * 4;
                    auto buf = new unsigned char[buf_size];

                    HDC hdc = ::GetDC(nullptr);
                    if (::GetDIBits(
                        hdc, hbmp, 0, bmp.bmHeight,
                        buf, &info, DIB_RGB_COLORS))
                    {
                        options->dpi_type = ImageDPIType::DEFAULT;
                        options->pixel_format = ImagePixelFormat::B8G8R8A8_UNORM;
                        options->alpha_mode = ImageAlphaMode::IGNORED;
                        frame = create(
                            bmp.bmWidth, bmp.bmHeight,
                            ByteData::ownPtr(buf, buf_size), bmp.bmWidth * 4, *options);
                    } else {
                        delete[] buf;
                    }

                    ::ReleaseDC(nullptr, hdc);
                }

                ::DeleteObject(hbmp);
            }
        }
        return frame;
    }

    LcImage LcImageFactoryWin::decodeFile(
        const std::u16string_view& file_name, const ImageOptions& options)
    {
        auto decoder = createDecoder(file_name);
        if (!decoder) {
            return {};
        }

        std::ifstream file_stream(utl::u16tow(file_name), std::ios::binary);
        return processDecoder(file_stream, decoder.get(), options);
    }

    LcImage LcImageFactoryWin::decodeMemory(
        const void* buffer, size_t size, const ImageOptions& options)
    {
        auto decoder = createDecoder(buffer, size);
        if (!decoder) {
            return {};
        }

        utl::imemorystream ims((const char*)buffer, size);
        return processDecoder(ims, decoder.get(), options);
    }

    bool LcImageFactoryWin::saveToFile(
        int width, int height,
        const void* data, size_t len, size_t stride,
        ImageContainer container,
        const ImageOptions& options,
        const std::u16string_view& file_name)
    {
        if (width <= 0 || height <= 0 ||
            !data || len == 0 || stride == 0)
        {
            ubassert(false);
            return false;
        }

        utl::win::ComPtr<IWICStream> stream;
        HRESULT hr = wic_factory_->CreateStream(&stream);
        if (FAILED(hr)) {
            return false;
        }

        std::wstring wfn(file_name.begin(), file_name.end());
        hr = stream->InitializeFromFilename(wfn.c_str(), GENERIC_WRITE);
        if (FAILED(hr)) {
            return false;
        }

        utl::win::ComPtr<IWICBitmapEncoder> encoder;
        hr = wic_factory_->CreateEncoder(mapImageContainer(container), nullptr, &encoder);
        if (FAILED(hr)) {
            return false;
        }

        hr = encoder->Initialize(stream.get(), WICBitmapEncoderNoCache);
        if (FAILED(hr)) {
            return false;
        }

        utl::win::ComPtr<IWICBitmapFrameEncode> frame;
        hr = encoder->CreateNewFrame(&frame, nullptr);
        if (FAILED(hr)) {
            return false;
        }

        hr = frame->Initialize(nullptr);
        if (FAILED(hr)) {
            return false;
        }

        hr = frame->SetSize(width, height);
        if (FAILED(hr)) {
            return false;
        }

        auto target_format = mapWICFormat(options);
        auto cur_format = target_format;

        hr = frame->SetPixelFormat(&cur_format);
        if (FAILED(hr)) {
            return false;
        }

        hr = IsEqualGUID(cur_format, target_format) ? S_OK : E_FAIL;
        if (FAILED(hr)) {
            return false;
        }

        hr = frame->WritePixels(
            height,
            utl::num_cast<UINT>(stride),
            utl::num_cast<UINT>(len),
            const_cast<BYTE*>(static_cast<const BYTE*>(data)));
        if (FAILED(hr)) {
            return false;
        }

        hr = frame->Commit();
        if (FAILED(hr)) {
            return false;
        }

        hr = encoder->Commit();
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool LcImageFactoryWin::getGifGlobalMetadata(IWICBitmapDecoder* decoder, GifImageData* data) {
        utl::win::ComPtr<IWICMetadataQueryReader> reader;
        HRESULT hr = decoder->GetMetadataQueryReader(&reader);
        if (FAILED(hr)) {
            return false;
        }

        PROPVARIANT prop_var;
        PropVariantInit(&prop_var);
        hr = reader->GetMetadataByName(L"/logscrdesc/Width", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
            data->width = prop_var.uiVal;
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/logscrdesc/Height", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
            data->height = prop_var.uiVal;
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/logscrdesc/GlobalColorTableFlag", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_BOOL && prop_var.boolVal) {
            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/logscrdesc/BackgroundColorIndex", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI1) {
                UINT bg_index = prop_var.bVal;
                utl::win::ComPtr<IWICPalette> palette;
                hr = wic_factory_->CreatePalette(&palette);
                if (SUCCEEDED(hr)) {
                    hr = decoder->CopyPalette(palette.get());
                }

                if (SUCCEEDED(hr)) {
                    UINT color_count = 0;
                    hr = palette->GetColorCount(&color_count);
                    if (SUCCEEDED(hr) && color_count > 0) {
                        UINT actual_count = 0;
                        WICColor* color_table = new WICColor[color_count];
                        hr = palette->GetColors(color_count, color_table, &actual_count);
                        if (SUCCEEDED(hr) && actual_count > 0 && bg_index < actual_count) {
                            data->bg_color = Color::ofARGB(color_table[bg_index]);
                        }
                        delete[] color_table;
                    }
                }
            }
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/logscrdesc/PixelAspectRatio", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_UI1) {
            UINT ratio = prop_var.bVal;
            if (ratio != 0) {
                float pixel_ratio = (ratio + 15.f) / 64.f;
                if (pixel_ratio > 1.f) {
                    data->height = static_cast<int>(data->height / pixel_ratio);
                } else {
                    data->width = static_cast<int>(data->width * pixel_ratio);
                }
            }
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/appext/application", &prop_var);
        if (SUCCEEDED(hr)
            && prop_var.vt == (VT_UI1 | VT_VECTOR)
            && prop_var.caub.cElems == 11
            && (!memcmp(prop_var.caub.pElems, "NETSCAPE2.0", prop_var.caub.cElems) ||
                !memcmp(prop_var.caub.pElems, "ANIMEXTS1.0", prop_var.caub.cElems))) {

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/appext/data", &prop_var);
            if (SUCCEEDED(hr)
                && (prop_var.vt == (VT_UI1 | VT_VECTOR)
                    && prop_var.caub.cElems >= 4
                    && prop_var.caub.pElems[0] > 0
                    && prop_var.caub.pElems[1] == 1)) {
                data->loop_count = MAKEWORD(prop_var.caub.pElems[2],
                    prop_var.caub.pElems[3]);
            }
        }
        PropVariantClear(&prop_var);
        return true;
    }

    bool LcImageFactoryWin::getGifFrameMetadata(IWICBitmapFrameDecode* decoder, GifImageFrData* data) {
        utl::win::ComPtr<IWICMetadataQueryReader> reader;
        HRESULT hr = decoder->GetMetadataQueryReader(&reader);
        if (FAILED(hr)) {
            return false;
        }

        PROPVARIANT prop_var;
        PropVariantInit(&prop_var);
        hr = reader->GetMetadataByName(L"/grctlext/Disposal", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_UI1) {
            data->disposal = prop_var.bVal;
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/grctlext/Delay", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
            data->frame_interval = prop_var.uiVal * 10;
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/imgdesc/Left", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
            data->left = prop_var.uiVal;
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/imgdesc/Top", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
            data->top = prop_var.uiVal;
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/imgdesc/Width", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
            data->width = prop_var.uiVal;
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/imgdesc/Height", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
            data->height = prop_var.uiVal;
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/imgdesc/InterlaceFlag", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_BOOL) {
            auto v_interlace = prop_var.boolVal;
            if (v_interlace) {
                data->interlace = true;
            } else {
                data->interlace = false;
            }
        }

        PropVariantClear(&prop_var);
        hr = reader->GetMetadataByName(L"/imgdesc/SortFlag", &prop_var);
        if (SUCCEEDED(hr) && prop_var.vt == VT_BOOL) {
            auto v_sort = prop_var.boolVal;
            if (v_sort) {
                data->sort = true;
            } else {
                data->sort = false;
            }
        }
        PropVariantClear(&prop_var);
        return true;
    }

    bool LcImageFactoryWin::getJpegFrameMetadata(
        IWICBitmapFrameDecode* decoder)
    {
        utl::win::ComPtr<IWICMetadataQueryReader> reader;
        HRESULT hr = decoder->GetMetadataQueryReader(&reader);
        if (FAILED(hr)) {
            return false;
        }

        PROPVARIANT prop;
        for (int i = 0;; ++i) {
            auto name = L"/[" + std::to_wstring(i) + L"]unknown";

            ::PropVariantInit(&prop);
            hr = reader->GetMetadataByName(name.c_str(), &prop);
            if (FAILED(hr)) {
                break;
            }
            if (prop.vt != VT_UNKNOWN) {
                ::PropVariantClear(&prop);
            }

            utl::win::ComPtr<IWICMetadataQueryReader> nr;
            hr = prop.punkVal->QueryInterface(IID_PPV_ARGS(&nr));
            if (SUCCEEDED(hr)) {
                PROPVARIANT nr_prop;
                ::PropVariantInit(&nr_prop);
                hr = nr->GetMetadataByName(L"/", &nr_prop);
                if (SUCCEEDED(hr) && nr_prop.vt == VT_BLOB) {
                    // APP2
                }
                ::PropVariantClear(&nr_prop);
            }

            ::PropVariantClear(&prop);
        }

        return true;
    }

    utl::win::ComPtr<IWICBitmapDecoder> LcImageFactoryWin::createDecoder(
        const std::u16string_view& file_name)
    {
        /*
         * 这里，某些图片使用 WICDecodeMetadataCacheOnLoad 解码时会报错，
         * 可能是 WIC 对属性的要求较为严格所致，使用 WICDecodeMetadataCacheOnDemand 即可。
         */
        utl::win::ComPtr<IWICBitmapDecoder> decoder;
        std::wstring wfn(file_name.begin(), file_name.end());
        HRESULT hr = wic_factory_->CreateDecoderFromFilename(
            wfn.c_str(),
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            &decoder);
        if (FAILED(hr)) {
            DLOG(Log::WARNING) << "Failed to decode file: " << utl::u16to8(file_name) << " " << std::hex << hr;
            return {};
        }

        return decoder;
    }

    utl::win::ComPtr<IWICBitmapDecoder> LcImageFactoryWin::createDecoder(const void* buffer, size_t size) {
        utl::win::ComPtr<IWICStream> stream;
        HRESULT hr = wic_factory_->CreateStream(&stream);
        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }

        utl::win::ComPtr<IStream> is(
            ::SHCreateMemStream(
                static_cast<const BYTE*>(buffer),
                utl::num_cast<UINT>(size)));
        if (!is) {
            ubassert(false);
            return {};
        }

        hr = stream->InitializeFromIStream(is.get());
        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }

        utl::win::ComPtr<IWICBitmapDecoder> decoder;
        hr = wic_factory_->CreateDecoderFromStream(
            stream.get(), nullptr, WICDecodeMetadataCacheOnDemand, &decoder);
        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }
        return decoder;
    }

    utl::win::ComPtr<IWICBitmapSource> LcImageFactoryWin::convertPixelFormat(
        IWICBitmapSource* frame, const ImageOptions& options)
    {
        utl::win::ComPtr<IWICFormatConverter> converter;
        HRESULT hr = wic_factory_->CreateFormatConverter(&converter);
        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }

        auto format = mapWICFormat(options);

        hr = converter->Initialize(
            frame,                          // Input bitmap to convert
            format,                         // Destination pixel format
            WICBitmapDitherTypeNone,        // Specified dither pattern
            nullptr,                        // Specify a particular palette
            0.f,                            // Alpha threshold
            WICBitmapPaletteTypeCustom      // Palette translation type
        );

        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }

        return converter.cast<IWICBitmapSource>();
    }

    LcImage LcImageFactoryWin::processDecoder(
        std::istream& s,
        IWICBitmapDecoder* decoder, const ImageOptions& options)
    {
        UINT frame_count = 0;
        HRESULT hr = decoder->GetFrameCount(&frame_count);
        if (FAILED(hr) || frame_count < 1) {
            ubassert(false);
            return {};
        }

        GUID img_format;
        hr = decoder->GetContainerFormat(&img_format);
        if (FAILED(hr)) {
            ubassert(false);
            return {};
        }

        LcImage image;
        if (img_format == GUID_ContainerFormatGif) {
            auto data = std::make_shared<GifImageData>();
            getGifGlobalMetadata(decoder, data.get());
            image.setData(data);
        } else if (img_format == GUID_ContainerFormatJpeg) {
            auto data = std::make_shared<JpegImageData>();
            parse_jpeg_metadata(s, &data->metadata);
            image.setData(data);
        }

        //utl::win::ComPtr<IWICColorContext> dst_cc;
        //hr = wic_factory_->CreateColorContext(&dst_cc);
        //if (SUCCEEDED(hr)) {
        //    std::wstring display_icm;
        //
        //    auto display = Display::fromPrimary();
        //    static_cast<DisplayWin*>(display.get())->getICMProfilePath(&display_icm);
        //
        //    hr = dst_cc->InitializeFromFilename(display_icm.c_str());
        //    if (FAILED(hr)) {
        //        dst_cc.reset();
        //    }
        //}

        for (UINT i = 0; i < frame_count; ++i) {
            utl::win::ComPtr<IWICBitmapFrameDecode> frame_decoder;
            hr = decoder->GetFrame(i, &frame_decoder);
            if (FAILED(hr)) {
                ubassert(false);
                return {};
            }

            WICPixelFormatGUID sf;
            hr = frame_decoder->GetPixelFormat(&sf);
            if (FAILED(hr)) {
                ubassert(false);
                return {};
            }

            std::wstring dmp_md;
            utl::win::ComPtr<IWICMetadataQueryReader> reader;
            hr = frame_decoder->GetMetadataQueryReader(&reader);
            if (SUCCEEDED(hr)) {
                dumpMetadata(reader.get(), 0, &dmp_md);
            }

            //exploreColorProfile(frame_decoder.get());

            utl::win::ComPtr<IWICBitmapSource> source;
            //source = convertGamut(frame_decoder.get(), dst_cc.get());
            if (!source) {
                source = frame_decoder.cast<IWICBitmapSource>();
            }

            auto new_options = options;
            if (options.pixel_format != ImagePixelFormat::RAW) {
                source = convertPixelFormat(source.get(), options);
            } else {
                new_options.pixel_format = mapImagePixelFormatFromWIC(sf);
            }

            if (!source) {
                return {};
            }

            LcImageFrame* frame = new LcImageFrameWin(new_options, {}, wic_factory_, source);

            switch (new_options.dpi_type) {
            case ImageDPIType::SPECIFIED:
                frame->setDpi(new_options.dpi_x, new_options.dpi_y);
                break;

            case ImageDPIType::DEFAULT:
            default:
                frame->setDpi(kDefaultDpi, kDefaultDpi);
                break;
            }

            if (img_format == GUID_ContainerFormatGif) {
                auto fr_data = std::make_shared<GifImageFrData>();
                getGifFrameMetadata(frame_decoder.get(), fr_data.get());
                frame->setData(fr_data);
            } else if (img_format == GUID_ContainerFormatJpeg) {
                //getJpegFrameMetadata(frame_decoder.get());
            }
            image.addFrame(GPtr<LcImageFrame>(frame));
        }

        return image;
    }

    bool LcImageFactoryWin::exploreColorProfile(IWICBitmapFrameDecode* frame) {
        UINT cc_count;
        HRESULT hr = frame->GetColorContexts(0, nullptr, &cc_count);
        if (FAILED(hr)) {
            return false;
        }

        bool constructing = true;
        IWICColorContext** ccs = new IWICColorContext*[cc_count];
        for (UINT i = 0; i < cc_count; ++i) {
            if (constructing) {
                hr = wic_factory_->CreateColorContext(&ccs[i]);
                if (FAILED(hr)) {
                    cc_count = i;
                    i = 0;
                    constructing = false;
                }
            } else {
                ccs[i]->Release();
            }
        }

        if (!constructing) {
            return false;
        }

        std::shared_ptr<IWICColorContext*> ccs_stub(
            ccs, [cc_count](IWICColorContext** c)
        {
                for (UINT i = 0; i < cc_count; ++i) {
                    c[i]->Release();
                }
        });

        hr = frame->GetColorContexts(cc_count, ccs, &cc_count);
        if (FAILED(hr)) {
            return false;
        }

        for (UINT i = 0; i < cc_count; ++i) {
            auto cc = ccs[i];

            WICColorContextType cc_type;
            hr = cc->GetType(&cc_type);
            if (FAILED(hr)) {
                return false;
            }

            if (cc_type != WICColorContextProfile) {
                continue;
            }

            UINT length;
            hr = cc->GetProfileBytes(0, nullptr, &length);
            if (FAILED(hr)) {
                return false;
            }

            std::unique_ptr<BYTE[]> buf(new BYTE[length]);
            hr = cc->GetProfileBytes(length, buf.get(), &length);
            if (FAILED(hr)) {
                return false;
            }

            PROFILE profile{ PROFILE_MEMBUFFER, buf.get(), length };
            HPROFILE cf = ::WcsOpenColorProfileW(
                &profile, nullptr, nullptr, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING, 0);
            if (!cf) {
                return false;
            }

            PROFILEHEADER header;
            if (::GetColorProfileHeader(cf, &header) != TRUE) {
                return false;
            }

            char* cmm_type = reinterpret_cast<char*>(&header.phCMMType);
            char* signature = reinterpret_cast<char*>(&header.phSignature);
            char* platform = reinterpret_cast<char*>(&header.phPlatform);

            DWORD size = 256;
            char desc[256];
            BOOL ref;
            if (::GetColorProfileElement(cf, 0x64657363, 12, &size, desc, &ref) == FALSE) {
            }

            ::CloseColorProfile(cf);
        }

        return true;
    }

    utl::win::ComPtr<IWICBitmapSource> LcImageFactoryWin::convertGamut(
        IWICBitmapFrameDecode* src, IWICColorContext* dst_cc)
    {
        if (!src || !dst_cc) {
            return {};
        }

        utl::win::ComPtr<IWICColorTransform> transform;
        HRESULT hr = wic_factory_->CreateColorTransformer(&transform);
        if (FAILED(hr)) {
            return {};
        }

        WICPixelFormatGUID pf;
        hr = src->GetPixelFormat(&pf);
        if (FAILED(hr)) {
            return {};
        }

        utl::win::ComPtr<IWICColorContext> src_cc;
        hr = wic_factory_->CreateColorContext(&src_cc);
        if (FAILED(hr)) {
            return {};
        }

        UINT cc_count;
        hr = src->GetColorContexts(1, &src_cc, &cc_count);
        if (FAILED(hr)) {
            return {};
        }

        if (cc_count == 0) {
            // 图片里没嵌颜色配置文件的话，就用默认的 sRGB
            std::wstring srgb_profile_path;
            if (!ColorManagerWin::getDefaultProfile(&srgb_profile_path)) {
                return {};
            }

            hr = src_cc->InitializeFromFilename(srgb_profile_path.c_str());
            if (FAILED(hr)) {
                return {};
            }
        }

        hr = transform->Initialize(src, src_cc.get(), dst_cc, pf);
        if (FAILED(hr)) {
            return {};
        }

        return transform.cast<IWICBitmapSource>();
    }

    bool LcImageFactoryWin::isSRGBFrame(IWICBitmapFrameDecode* frame) {
        utl::win::ComPtr<IWICMetadataQueryReader> reader;
        HRESULT hr = frame->GetMetadataQueryReader(&reader);
        if (FAILED(hr)) {
            return false;
        }

        GUID format;
        hr = reader->GetContainerFormat(&format);
        if (FAILED(hr)) {
            return false;
        }

        PROPVARIANT prop;
        PropVariantInit(&prop);

        if (format == GUID_ContainerFormatPng) {
            hr = reader->GetMetadataByName(L"/sRGB/RenderingIntent", &prop);
            if (SUCCEEDED(hr) && prop.vt == VT_UI1) {
                PropVariantClear(&prop);
                return true;
            }

            hr = reader->GetMetadataByName(L"/gAMA/ImageGamma", &prop);
            if (SUCCEEDED(hr) && prop.vt == VT_UI4 && prop.uintVal == 45455) {
                PropVariantClear(&prop);
                return true;
            }
        } else if (format == GUID_ContainerFormatJpeg) {
            hr = reader->GetMetadataByName(L"/app1/ifd/exif/{ushort=40961}", &prop);
            if (SUCCEEDED(hr) && prop.vt == VT_UI2 && prop.uiVal == 1) {
                PropVariantClear(&prop);
                return true;
            }
        } else if (format == GUID_ContainerFormatTiff) {
            hr = reader->GetMetadataByName(L"/ifd/exif/{ushort=40961}", &prop);
            if (SUCCEEDED(hr) && prop.vt == VT_UI2 && prop.uiVal == 1) {
                PropVariantClear(&prop);
                return true;
            }
        } else {
            hr = reader->GetMetadataByName(L"System.Image.ColorSpace", &prop);
            if (SUCCEEDED(hr) && prop.vt == VT_UI2 && prop.uiVal == 1) {
                PropVariantClear(&prop);
                return true;
            }
        }

        PropVariantClear(&prop);
        return false;

    }

    bool LcImageFactoryWin::dumpMetadata(
        IWICMetadataQueryReader* reader, int depth, std::wstring* out)
    {
        utl::win::ComPtr<IEnumString> enumerator;
        HRESULT hr = reader->GetEnumerator(&enumerator);
        if (FAILED(hr)) {
            return false;
        }

        std::wostringstream ss;

        for (;;) {
            LPOLESTR _name;
            hr = enumerator->Next(1, &_name, nullptr);
            if (FAILED(hr)) {
                break;
            } else if (hr == S_FALSE) {
                break;
            }

            std::wstring name(_name);
            ::CoTaskMemFree(_name);

            PROPVARIANT prop;
            ::PropVariantInit(&prop);

            for (int i = 0; i < depth; ++i) {
                ss << L"  ";
            }

            hr = reader->GetMetadataByName(name.c_str(), &prop);
            if (FAILED(hr)) {
                ss << name << ": [FAILED] " << hr << "\n";
                continue;
            }

            switch (prop.vt) {
            case VT_EMPTY: ss << name << ": [EMPTY]\n"; break;
            case VT_NULL:  ss << name << ": [NULL]\n"; break;
            case VT_I2:    ss << name << ": " << prop.iVal << "\n"; break;
            case VT_I4:    ss << name << ": " << prop.lVal << "\n"; break;
            case VT_R4:    ss << name << ": " << prop.fltVal << "\n"; break;
            case VT_R8:    ss << name << ": " << prop.dblVal << "\n"; break;
            case VT_CY:    ss << name << ": " << prop.cyVal.int64 << "\n"; break;
            case VT_DATE:  ss << name << ": " << prop.date << "\n"; break;
            case VT_BSTR:  ss << name << ": " << prop.bstrVal << "\n"; break;
            case VT_DISPATCH: ss << name << ": [DISPATCH]\n"; break;
            case VT_ERROR: ss << name << ": " << prop.scode << "\n"; break;
            case VT_BOOL:  ss << name << ": " << (prop.boolVal ? "true" : "false") << "\n"; break;
            case VT_VARIANT: ss << name << ": [VARIANT]\n"; break;
            case VT_UNKNOWN: ss << name << ": [obj]\n"; break;
            case VT_DECIMAL: ss << name << ": [DECIMAL]\n"; break;
            case VT_I1:    ss << name << ": " << prop.cVal << "\n"; break;
            case VT_UI1:   ss << name << ": " << prop.bVal << "\n"; break;
            case VT_UI2:   ss << name << ": " << prop.uiVal << "\n"; break;
            case VT_UI4:   ss << name << ": " << prop.ulVal << "\n"; break;
            case VT_I8:    ss << name << ": " << prop.hVal.QuadPart << "\n"; break;
            case VT_UI8:   ss << name << ": " << prop.uhVal.QuadPart << "\n"; break;
            case VT_INT:   ss << name << ": " << prop.intVal << "\n"; break;
            case VT_UINT:  ss << name << ": " << prop.uintVal << "\n"; break;
            case VT_VOID:  ss << name << ": [VOID]\n"; break;
            case VT_HRESULT: ss << name << ": " << prop.lVal << "\n"; break;
            case VT_PTR:   ss << name << ": [PTR]\n"; break;
            case VT_SAFEARRAY: ss << name << ": [SAFEARRAY]\n"; break;
            case VT_CARRAY: ss << name << ": [CARRAY]\n"; break;
            case VT_USERDEFINED: ss << name << ": [USERDEFINED]\n"; break;
            case VT_LPSTR:    ss << name << ": " << prop.pszVal << "\n"; break;
            case VT_LPWSTR:   ss << name << ": " << prop.pwszVal << "\n"; break;
            case VT_RECORD:   ss << name << ": [RECORD]\n"; break;
            case VT_INT_PTR:  ss << name << ": p:" << prop.intVal << "\n"; break;
            case VT_UINT_PTR: ss << name << ": p:" << prop.uintVal << "\n"; break;
            case VT_FILETIME: ss << name << ": [FILETIME]\n"; break;
            case VT_BLOB:
            {
                ss << name << ": [BLOB] ";
                ss << "size:" << prop.blob.cbSize;
                if (prop.blob.cbSize > 0) {
                    ss << ", ";
                }
                ULONG i = 0;
                for (; i < prop.blob.cbSize; ++i) {
                    char ch = (char)prop.blob.pBlobData[i];
                    if (ch >= 0x20 && ch <= 0x7E) {
                        if (i == 0) ss << "'";
                        ss << ch;
                    } else {
                        break;
                    }
                }
                if (i > 0) ss << "'";
                for (; i < prop.blob.cbSize && i < 16; ++i) {
                    auto ch = prop.blob.pBlobData[i];
                    ss << " " << std::hex << std::setfill(L'0') << std::setw(2) << (unsigned int)ch;
                }
                if (i < prop.blob.cbSize) {
                    ss << "...";
                }
                ss << "\n";
                break;
            }
            case VT_STREAM: ss << name << ": [STREAM]\n"; break;
            case VT_STORAGE: ss << name << ": [STORAGE]\n"; break;
            case VT_STREAMED_OBJECT: ss << name << ": [STREAMED_OBJECT]\n"; break;
            case VT_STORED_OBJECT: ss << name << ": [STORED_OBJECT]\n"; break;
            case VT_BLOB_OBJECT: ss << name << ": [BLOB_OBJECT]\n"; break;
            case VT_CF: ss << name << ": [CF]\n"; break;
            case VT_CLSID: ss << name << ": [CLSID]\n"; break;
            case VT_VERSIONED_STREAM: ss << name << ": [VERSIONED_STREAM]\n"; break;
            case VT_BSTR_BLOB: ss << name << ": [BSTR_BLOB]\n"; break;
            case VT_VECTOR | VT_I1: {
                ss << name << ": ";
                for (ULONG i = 0; i < prop.cac.cElems; ++i) {
                    ss << prop.cac.pElems[i] << " ";
                }
                ss << "\n";
            } break;
            case VT_VECTOR | VT_I2: {
                ss << name << ": ";
                for (ULONG i = 0; i < prop.cai.cElems; ++i) {
                    ss << prop.cai.pElems[i] << " ";
                }
                ss << "\n";
            } break;
            case VT_VECTOR | VT_UI1: {
                ss << name << ": ";
                for (ULONG i = 0; i < prop.caub.cElems; ++i) {
                    ss << prop.caub.pElems[i] << " ";
                }
                ss << "\n";
            } break;
            case VT_VECTOR | VT_UI2: {
                ss << name << ": ";
                for (ULONG i = 0; i < prop.caui.cElems; ++i) {
                    ss << prop.caui.pElems[i] << " ";
                }
                ss << "\n";
            } break;
            case VT_VECTOR | VT_UI4: {
                ss << name << ": ";
                for (ULONG i = 0; i < prop.caul.cElems; ++i) {
                    ss << prop.caul.pElems[i] << " ";
                }
                ss << "\n";
            } break;
            case VT_VECTOR | VT_UI8: {
                ss << name << ": ";
                for (ULONG i = 0; i < prop.cauh.cElems; ++i) {
                    ss << prop.cauh.pElems[i].QuadPart << " ";
                }
                ss << "\n";
            } break;
            case VT_VECTOR | VT_BOOL: {
                ss << name << ": ";
                for (ULONG i = 0; i < prop.cabool.cElems; ++i) {
                    ss << (prop.cabool.pElems[i] ? "true" : "false") << " ";
                }
                ss << "\n";
            } break;
            case VT_VECTOR | VT_LPWSTR: {
                ss << name << ": ";
                for (ULONG i = 0; i < prop.calpwstr.cElems; ++i) {
                    ss << prop.calpwstr.pElems[i] << " ";
                }
                ss << "\n";
            } break;
            default: ss << name << ": [?]\n"; break;
            }

            if (prop.vt == VT_UNKNOWN) {
                utl::win::ComPtr<IWICMetadataQueryReader> nr;
                hr = prop.punkVal->QueryInterface(IID_PPV_ARGS(&nr));
                if (SUCCEEDED(hr)) {
                    std::wstring ns;
                    dumpMetadata(nr.get(), depth + 1, &ns);
                    ss << ns;
                    int asfd = 0;
                }
            }

            ::PropVariantClear(&prop);
        }

        *out = ss.str();
        return true;
    }

}
}