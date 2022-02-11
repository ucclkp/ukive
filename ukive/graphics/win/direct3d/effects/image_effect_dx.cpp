// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_effect_dx.h"

#include "utils/log.h"
#include "utils/files/file_utils.h"

#include "ukive/app/application.h"
#include "ukive/graphics/win/offscreen_buffer_win.h"
#include "ukive/graphics/win/cyro_render_target_d2d.h"
#include "ukive/graphics/win/images/image_frame_win.h"
#include "ukive/graphics/win/images/image_options_d2d_utils.h"
#include "ukive/graphics/win/gpu/gpu_context_d3d.h"
#include "ukive/graphics/canvas.h"
#include "ukive/resources/resource_manager.h"
#include "ukive/window/window.h"


namespace ukive {
namespace win {

    ImageEffectDX::ImageEffectDX(Context context)
        : width_(0),
          height_(0),
          view_width_(0),
          view_height_(0),
          viewport_(),
          context_(context)
    {
    }

    ImageEffectDX::~ImageEffectDX() {}

    bool ImageEffectDX::initialize() {
        // 顶点着色器
        auto device = static_cast<DirectXManager*>(
            Application::getGraphicDeviceManager())->getD3DDevice();

        D3D11_INPUT_ELEMENT_DESC layout[1];
        layout[0].SemanticName = "POSITION";
        layout[0].SemanticIndex = 0;
        layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        layout[0].InputSlot = 0;
        layout[0].AlignedByteOffset = 0;
        layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[0].InstanceDataStepRate = 0;

        auto res_mgr = Application::getResourceManager();
        auto shader_dir = res_mgr->getResRootPath() / u"shaders";

        std::string vs_bc;
        res_mgr->getFileData(shader_dir / u"image_effect_vs.cso", &vs_bc);
        HRESULT hr = device->CreateVertexShader(vs_bc.data(), vs_bc.size(), nullptr, &vs_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create vertex shader: " << hr;
            return false;
        }

        hr = device->CreateInputLayout(
            layout, ARRAYSIZE(layout),
            vs_bc.data(), vs_bc.size(), &input_layout_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create input layout: " << hr;
            return false;
        }

        // 像素着色器
        std::string ps_bc;
        res_mgr->getFileData(shader_dir / u"image_effect_ps.cso", &ps_bc);
        hr = device->CreatePixelShader(ps_bc.data(), ps_bc.size(), nullptr, &ps_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create pixel shader: " << hr;
            return false;
        }


        // 常量缓存
        D3D11_BUFFER_DESC cb_desc;
        cb_desc.Usage = D3D11_USAGE_DYNAMIC;
        cb_desc.ByteWidth = sizeof(ConstBuffer);
        cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cb_desc.MiscFlags = 0;
        cb_desc.StructureByteStride = 0;

        hr = device->CreateBuffer(&cb_desc, nullptr, &const_buffer_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create const buffer: " << hr;
            return false;
        }

        D3D11_RASTERIZER_DESC rasterDesc;
        ZeroMemory(&rasterDesc, sizeof(rasterDesc));

        // 设置光栅化描述，指定多边形如何被渲染.
        rasterDesc.AntialiasedLineEnable = FALSE;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = FALSE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = FALSE;
        rasterDesc.MultisampleEnable = FALSE;
        rasterDesc.ScissorEnable = FALSE;
        rasterDesc.SlopeScaledDepthBias = 0.0f;

        // 创建光栅化状态.
        hr = device->CreateRasterizerState(&rasterDesc, &rasterizer_state_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create rasterizer state: " << hr;
            return false;
        }

        is_initialized_ = true;
        return true;
    }

    void ImageEffectDX::destroy() {
        vs_.reset();
        input_layout_.reset();
        ps_.reset();
        const_buffer_.reset();
        rasterizer_state_.reset();

        org_srv_.reset();
        kernel_tex2d_.reset();
        kernel_srv_.reset();
        target_tex2d_.reset();
        target_rtv_.reset();
        target_srv_.reset();
        vert_buffer_.reset();
        index_buffer_.reset();

        cache_.reset();

        width_ = 0;
        height_ = 0;
        is_initialized_ = false;
    }

    bool ImageEffectDX::generate(Canvas* c) {
        if (!is_initialized_) {
            return false;
        }

        if (cache_) {
            return true;
        }

        auto buffer = c->getBuffer();
        if (!buffer || !target_tex2d_) {
            assert(false);
            return false;
        }

        auto rt = static_cast<CyroRenderTargetD2D*>(buffer->getRT())->getNative();
        if (!rt) {
            assert(false);
            return false;
        }

        render();

        D2D1_BITMAP_PROPERTIES bmp_prop = mapBitmapProps(c->getBuffer()->getImageOptions());

        auto dxgi_surface = target_tex2d_.cast<IDXGISurface>();
        if (!dxgi_surface) {
            assert(false);
            return false;
        }

        ComPtr<ID2D1Bitmap> bitmap;
        HRESULT hr = rt->CreateSharedBitmap(
            __uuidof(IDXGISurface), dxgi_surface.get(), &bmp_prop, &bitmap);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return false;
        }

        auto context = static_cast<GPUContextD3D*>(
            Application::getGraphicDeviceManager()->getGPUContext())->getNative();

        cache_.reset(new ImageFrameWin(bitmap, rt, context, target_tex2d_));
        return true;
    }

    bool ImageEffectDX::draw(Canvas* c) {
        if (!is_initialized_) {
            return false;
        }

        if (!cache_ && !generate(c)) {
            return false;
        }

        c->drawImage(c->getOpacity(), cache_.get());
        return true;
    }

    bool ImageEffectDX::createKernelTexture(const uint8_t* kernel, size_t len) {
        auto d3d_device = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->getD3DDevice();

        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tex_desc.Format = DXGI_FORMAT_R8_UINT;
        tex_desc.Width = len;
        tex_desc.Height = 1;
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = kernel;
        data.SysMemPitch = sizeof(uint8_t) * len;
        data.SysMemSlicePitch = 0;

        HRESULT hr = d3d_device->CreateTexture2D(&tex_desc, &data, &kernel_tex2d_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create 2d texture: " << hr;
            return false;
        }

        hr = d3d_device->CreateShaderResourceView(kernel_tex2d_.get(), nullptr, &kernel_srv_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return false;
        }
        return true;
    }
    bool ImageEffectDX::setSize(int width, int height, bool hdr) {
        if (width_ == width && height_ == height && is_hdr_enabled_ == hdr) {
            return true;
        }

        width_ = width;
        height_ = height;
        is_hdr_enabled_ = hdr;

        viewport_.Width = float(width_);
        viewport_.Height = float(height_);
        viewport_.MinDepth = 0.0f;
        viewport_.MaxDepth = 1.0f;
        viewport_.TopLeftX = 0;
        viewport_.TopLeftY = 0;

        // 顶点缓存
        VertexData vertices[] {
            { ukv3d::Point3F(0,            float(height), 0) },
            { ukv3d::Point3F(float(width), float(height), 0) },
            { ukv3d::Point3F(float(width), 0,             0) },
            { ukv3d::Point3F(0,            0,             0) },
        };

        D3D11_BUFFER_DESC vb_desc;
        vb_desc.Usage = D3D11_USAGE_DYNAMIC;
        vb_desc.ByteWidth = sizeof(VertexData) * ARRAYSIZE(vertices);
        vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vb_desc.MiscFlags = 0;
        vb_desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA vertex_data;
        vertex_data.pSysMem = vertices;
        vertex_data.SysMemPitch = 0;
        vertex_data.SysMemSlicePitch = 0;

        HRESULT hr = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            getD3DDevice()->CreateBuffer(&vb_desc, &vertex_data, &vert_buffer_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create vertex buffer: " << hr;
            return false;
        }

        // 索引缓存
        int indices[] = {0, 1, 2, 0, 2, 3};

        D3D11_BUFFER_DESC ib_desc;
        ib_desc.Usage = D3D11_USAGE_DYNAMIC;
        ib_desc.ByteWidth = sizeof(int)* ARRAYSIZE(indices);
        ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ib_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        ib_desc.MiscFlags = 0;
        ib_desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA index_data;
        index_data.pSysMem = indices;
        index_data.SysMemPitch = 0;
        index_data.SysMemSlicePitch = 0;

        hr = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            getD3DDevice()->CreateBuffer(&ib_desc, &index_data, &index_buffer_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create index buffer: " << hr;
            return false;
        }

        float pos_x = width_ / 2.f;
        float pos_y = height_ / 2.f;

        // 摄像机位置。
        auto pos = ukv3d::Point3F(pos_x, pos_y, -2);
        // 摄像机看向的位置。
        auto look_at = ukv3d::Point3F(pos_x, pos_y, 0);
        // 摄像机上向量
        auto up = ukv3d::Vector3F(0.0f, 1.0f, 0.0f);

        world_matrix_.identity();
        view_matrix_ = ukv3d::Matrix4x4F::camera(pos, pos - look_at, up);
        ortho_matrix_ = ukv3d::Matrix4x4F::orthoProj(-width_ / 2.f, width_ / 2.f, -height_ / 2.f, height_ / 2.f, 1, 2);
        ukv3d::Matrix4x4F adj(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, -0.5f, 2 / 2.f + 1,
            0, 0, 0, 1);
        ortho_matrix_.mul(adj);

        wvo_matrix_ = ortho_matrix_ * view_matrix_ * world_matrix_;

        if (!createTexture(target_tex2d_, target_rtv_, target_srv_)) {
            return false;
        }
        return true;
    }

    void ImageEffectDX::render() {
        if (!is_initialized_) {
            return;
        }

        auto d3d_dc = static_cast<DirectXManager*>(
            Application::getGraphicDeviceManager())->getD3DDeviceContext();

        d3d_dc->VSSetShader(vs_.get(), nullptr, 0);
        d3d_dc->PSSetShader(ps_.get(), nullptr, 0);
        d3d_dc->IASetInputLayout(input_layout_.get());
        d3d_dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        d3d_dc->RSSetState(rasterizer_state_.get());
        d3d_dc->RSSetViewports(1, &viewport_);

        {
            ID3D11ShaderResourceView* srvs[] = { org_srv_.get(), kernel_srv_.get() };
            d3d_dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
            d3d_dc->OMSetRenderTargets(1, &target_rtv_, nullptr);
        }

        // VS ConstBuffer
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT hr = d3d_dc->Map(const_buffer_.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            if (SUCCEEDED(hr)) {
                static_cast<ConstBuffer*>(mappedResource.pData)->wvo = wvo_matrix_;
                d3d_dc->Unmap(const_buffer_.get(), 0);
            }
        }

        d3d_dc->VSSetConstantBuffers(0, 1, &const_buffer_);

        // Render
        FLOAT transparent[4] = { 0, 0, 0, 0 };
        d3d_dc->ClearRenderTargetView(target_rtv_.get(), transparent);

        UINT vertexDataOffset = 0;
        UINT vertexStructSize = sizeof(VertexData);
        d3d_dc->IASetVertexBuffers(0, 1, &vert_buffer_, &vertexStructSize, &vertexDataOffset);
        d3d_dc->IASetIndexBuffer(index_buffer_.get(), DXGI_FORMAT_R32_UINT, 0);
        d3d_dc->DrawIndexed(6, 0, 0);
    }

    bool ImageEffectDX::setContent(OffscreenBuffer* content) {
        if (!is_initialized_ || !content) {
            return false;
        }

        auto texture = static_cast<const OffscreenBufferWin*>(content)->getTexture();

        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);
        view_width_ = desc.Width;
        view_height_ = desc.Height;

        int width = view_width_;
        int height = view_height_;
        cache_.reset();

        auto device = static_cast<DirectXManager*>(
            Application::getGraphicDeviceManager())->getD3DDevice();

        org_srv_.reset();
        HRESULT hr = device->CreateShaderResourceView(texture.get(), nullptr, &org_srv_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return false;
        }

        return setSize(
            width, height,
            content->getImageOptions().pixel_format == ImagePixelFormat::HDR);
    }

    bool ImageEffectDX::setContent(ID3D11Texture2D* texture) {
        if (!is_initialized_ || !texture) {
            return false;
        }

        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);
        view_width_ = desc.Width;
        view_height_ = desc.Height;

        int width = view_width_;
        int height = view_height_;
        cache_.reset();

        auto device = static_cast<DirectXManager*>(
            Application::getGraphicDeviceManager())->getD3DDevice();

        org_srv_.reset();
        HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &org_srv_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return false;
        }

        return setSize(width, height, false);
    }

    ImageFrame* ImageEffectDX::getOutput() const {
        return cache_.get();
    }

    bool ImageEffectDX::createTexture(
        ComPtr<ID3D11Texture2D>& tex,
        ComPtr<ID3D11RenderTargetView>& rtv,
        ComPtr<ID3D11ShaderResourceView>& srv)
    {
        tex = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createTexture2D(width_, height_, true, is_hdr_enabled_, false);
        if (!tex) {
            return false;
        }

        auto device = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->getD3DDevice();

        srv.reset();
        HRESULT hr = device->CreateShaderResourceView(tex.get(), nullptr, &srv);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return false;
        }

        rtv.reset();
        hr = device->CreateRenderTargetView(tex.get(), nullptr, &rtv);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create RTV: " << hr;
            return false;
        }
        return true;
    }

}
}