// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shadow_effect_dx.h"

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


namespace {

    float getWeight(float x, float sigma) {
        float exponent = -std::pow(x, 2) / (2 * std::pow(sigma, 2));
        return std::exp(exponent) / (std::sqrt(2 * 3.1416f) * sigma);
    }

}

namespace ukive {

    ShadowEffectDX::ShadowEffectDX(Context context)
        : width_(0),
          height_(0),
          view_width_(0),
          view_height_(0),
          radius_(0),
          semi_radius_(0.f),
          viewport_(),
          context_(context)
    {
    }

    ShadowEffectDX::~ShadowEffectDX() {}

    bool ShadowEffectDX::initialize() {
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
        res_mgr->getFileData(shader_dir / u"shadow_effect_vs.cso", &vs_bc);
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
        res_mgr->getFileData(shader_dir / u"shadow_effect_vert_ps.cso", &ps_bc);
        hr = device->CreatePixelShader(ps_bc.data(), ps_bc.size(), nullptr, &vert_ps_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create pixel shader: " << hr;
            return false;
        }

        ps_bc.clear();
        res_mgr->getFileData(shader_dir / u"shadow_effect_hori_ps.cso", &ps_bc);
        hr = device->CreatePixelShader(ps_bc.data(), ps_bc.size(), nullptr, &hori_ps_);
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

    void ShadowEffectDX::destroy() {
        vs_.reset();
        input_layout_.reset();
        vert_ps_.reset();
        hori_ps_.reset();
        const_buffer_.reset();
        rasterizer_state_.reset();

        bg_rtv_.reset();
        bg_srv_.reset();
        kernel_tex2d_.reset();
        kernel_srv_.reset();
        shadow1_tex2d_.reset();
        shadow1_rtv_.reset();
        shadow1_srv_.reset();
        shadow2_tex2d_.reset();
        shadow2_rtv_.reset();
        shadow2_srv_.reset();
        vert_buffer_.reset();
        index_buffer_.reset();

        cache_.reset();

        width_ = 0;
        height_ = 0;
        radius_ = 0;
        is_initialized_ = false;
    }

    bool ShadowEffectDX::generate(Canvas* c) {
        if (!is_initialized_) {
            return false;
        }

        if (cache_) {
            return true;
        }

        auto buffer = c->getBuffer();
        if (!buffer || !shadow2_tex2d_) {
            assert(false);
            return false;
        }

        auto rt = static_cast<CyroRenderTargetD2D*>(buffer->getRT())->getNative();
        if (!rt) {
            assert(false);
            return false;
        }

        render();

        auto dxgi_surface = shadow2_tex2d_.cast<IDXGISurface>();
        if (!dxgi_surface) {
            assert(false);
            return false;
        }

        D2D1_BITMAP_PROPERTIES bmp_prop = mapBitmapProps(c->getBuffer()->getImageOptions());

        ComPtr<ID2D1Bitmap> bitmap;
        HRESULT hr = rt->CreateSharedBitmap(
            __uuidof(IDXGISurface), dxgi_surface.get(), &bmp_prop, &bitmap);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return false;
        }

        auto context = static_cast<GPUContextD3D*>(
            Application::getGraphicDeviceManager()->getGPUContext())->getNative();

        cache_.reset(new ImageFrameWin(bitmap, rt, context, shadow2_tex2d_));
        return true;
    }

    bool ShadowEffectDX::draw(Canvas* c) {
        if (!is_initialized_) {
            return false;
        }

        if (!cache_ && !generate(c)) {
            return false;
        }

        c->save();
        c->translate(-std::floor(semi_radius_ * 2), -std::floor(semi_radius_ * 2));
        c->drawImage(c->getOpacity(), cache_.get());
        c->restore();

        return true;
    }

    bool ShadowEffectDX::setSize(int width, int height, bool hdr) {
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

        if (!createTexture(shadow1_tex2d_, shadow1_rtv_, shadow1_srv_)) {
            return false;
        }
        if (!createTexture(shadow2_tex2d_, shadow2_rtv_, shadow2_srv_)) {
            return false;
        }
        return true;
    }

    void ShadowEffectDX::render() {
        if (!is_initialized_) {
            return;
        }

        auto d3d_dc = static_cast<DirectXManager*>(
            Application::getGraphicDeviceManager())->getD3DDeviceContext();

        d3d_dc->VSSetShader(vs_.get(), nullptr, 0);
        d3d_dc->PSSetShader(hori_ps_.get(), nullptr, 0);
        d3d_dc->IASetInputLayout(input_layout_.get());
        d3d_dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        d3d_dc->RSSetState(rasterizer_state_.get());
        d3d_dc->RSSetViewports(1, &viewport_);

        {
            ID3D11ShaderResourceView* srvs[] = { bg_srv_.get(), kernel_srv_.get() };
            d3d_dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
            d3d_dc->OMSetRenderTargets(1, &shadow1_rtv_, nullptr);
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
        d3d_dc->ClearRenderTargetView(shadow1_rtv_.get(), transparent);

        UINT vertexDataOffset = 0;
        UINT vertexStructSize = sizeof(VertexData);
        d3d_dc->IASetVertexBuffers(0, 1, &vert_buffer_, &vertexStructSize, &vertexDataOffset);
        d3d_dc->IASetIndexBuffer(index_buffer_.get(), DXGI_FORMAT_R32_UINT, 0);
        d3d_dc->DrawIndexed(6, 0, 0);

        d3d_dc->PSSetShader(vert_ps_.get(), nullptr, 0);
        {
            d3d_dc->OMSetRenderTargets(1, &shadow2_rtv_, nullptr);
            ID3D11ShaderResourceView* srvs[] = { shadow1_srv_.get(), bg_srv_.get(), kernel_srv_.get() };
            d3d_dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
        }

        // Render
        d3d_dc->ClearRenderTargetView(shadow2_rtv_.get(), transparent);
        d3d_dc->DrawIndexed(6, 0, 0);
    }

    void ShadowEffectDX::resetCache() {
        cache_.reset();
    }

    bool ShadowEffectDX::hasCache() const {
        return cache_ != nullptr;
    }

    bool ShadowEffectDX::setRadius(int radius) {
        if (!is_initialized_) {
            return false;
        }
        if (radius == radius_ || radius <= 0) {
            return true;
        }

        radius_ = radius;
        semi_radius_ = radius_ / 2.f;
        cache_.reset();

        return createKernelTexture();
    }

    bool ShadowEffectDX::setContent(OffscreenBuffer* content) {
        if (!is_initialized_ || !content) {
            return false;
        }

        auto texture = static_cast<const OffscreenBufferWin*>(content)->getTexture();

        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);
        view_width_ = desc.Width;
        view_height_ = desc.Height;

        int radius = int(std::ceil(radius_ * context_.getAutoScale()));
        int width = view_width_ + radius * 2;
        int height = view_height_ + radius * 2;
        cache_.reset();

        auto device = static_cast<DirectXManager*>(
            Application::getGraphicDeviceManager())->getD3DDevice();

        bg_srv_.reset();
        HRESULT hr = device->CreateShaderResourceView(texture.get(), nullptr, &bg_srv_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return false;
        }

        bg_rtv_.reset();
        hr = device->CreateRenderTargetView(texture.get(), nullptr, &bg_rtv_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create RTV: " << hr;
            return false;
        }

        return setSize(
            width, height,
            content->getImageOptions().pixel_format == ImagePixelFormat::HDR);
    }

    int ShadowEffectDX::getRadius() const {
        return radius_;
    }

    ImageFrame* ShadowEffectDX::getOutput() const {
        return cache_.get();
    }

    bool ShadowEffectDX::createTexture(
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

    bool ShadowEffectDX::createKernelTexture() {
        int radius = int(std::ceil(radius_ * context_.getAutoScale()));
        float total_weight = 0;
        std::unique_ptr<float[]> weight_matrix(new float[radius + 1]());
        for (int i = 0; i < radius + 1; ++i) {
            float w = getWeight(float(radius - i), semi_radius_ * context_.getAutoScale());
            weight_matrix[i] = w;
            if (i != radius) {
                total_weight += w;
            }
        }

        total_weight *= 2;
        total_weight += weight_matrix[radius];
        for (int i = 0; i < radius + 1; ++i) {
            weight_matrix[i] /= total_weight;
        }

        auto d3d_device = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->getD3DDevice();

        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tex_desc.Format = DXGI_FORMAT_R32_FLOAT;
        tex_desc.Width = radius + 1;
        tex_desc.Height = 1;
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = weight_matrix.get();
        data.SysMemPitch = sizeof(float) * (radius + 1);
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
}