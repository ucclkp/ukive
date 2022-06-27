// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shadow_effect_dx.h"

#include "utils/log.h"
#include "utils/files/file_utils.h"
#include "utils/math/algebra/special_matrix.hpp"

#include "ukive/app/application.h"
#include "ukive/graphics/win/offscreen_buffer_win.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/resources/resource_manager.h"
#include "ukive/window/window.h"


namespace {

    float getWeight(float x, float sigma) {
        float exponent = -std::pow(x, 2) / (2 * std::pow(sigma, 2));
        return std::exp(exponent) / (std::sqrt(2 * 3.1416f) * sigma);
    }

}

namespace ukive {
namespace win {

    ShadowEffectGPU::ShadowEffectGPU(Context context)
        : width_(0),
          height_(0),
          view_width_(0),
          view_height_(0),
          radius_(0),
          semi_radius_(0.f),
          context_(context)
    {
    }

    ShadowEffectGPU::~ShadowEffectGPU() {}

    bool ShadowEffectGPU::initialize() {
        // 顶点着色器
        auto device =
            Application::getGraphicDeviceManager()->getGPUDevice();

        auto res_mgr = Application::getResourceManager();
        auto shader_dir = res_mgr->getResRootPath() / u"shaders";

        std::string vs_bc;
        res_mgr->getFileData(shader_dir / u"shadow_effect_vs.cso", &vs_bc);
        auto vs_ret = device->createVertexShader(vs_bc.data(), vs_bc.size());
        if (!vs_ret) {
            LOG(Log::WARNING) << "Failed to create vertex shader: " << vs_ret.code.raw_code();
            return false;
        }
        vs_ = vs_ret;

        using GILD = GPUInputLayout::Desc;
        GILD layout[1];
        layout[0] = GILD("POSITION", GPUDataFormat::R32G32B32_FLOAT, 0, 0, false);
        auto il_ret = device->createInputLayout(layout, ARRAYSIZE(layout), vs_bc.data(), vs_bc.size());
        if (!il_ret) {
            LOG(Log::WARNING) << "Failed to create input layout: " << il_ret.code.raw_code();
            return false;
        }
        input_layout_ = il_ret;

        // 像素着色器
        std::string ps_bc;
        res_mgr->getFileData(shader_dir / u"shadow_effect_vert_ps.cso", &ps_bc);
        auto vert_ps_ret = device->createPixelShader(ps_bc.data(), ps_bc.size());
        if (!vert_ps_ret) {
            LOG(Log::WARNING) << "Failed to create pixel shader: " << vert_ps_ret.code.raw_code();
            return false;
        }
        vert_ps_ = vert_ps_ret;

        ps_bc.clear();
        res_mgr->getFileData(shader_dir / u"shadow_effect_hori_ps.cso", &ps_bc);
        auto hori_ps_ret = device->createPixelShader(ps_bc.data(), ps_bc.size());
        if (!hori_ps_ret) {
            LOG(Log::WARNING) << "Failed to create pixel shader: " << hori_ps_ret.code.raw_code();
            return false;
        }
        hori_ps_ = hori_ps_ret;

        // 常量缓存
        GPUBuffer::Desc cb_desc;
        cb_desc.is_dynamic = true;
        cb_desc.byte_width = sizeof(ConstBuffer);
        cb_desc.res_type = GPUBuffer::RES_CONSTANT_BUFFER;
        cb_desc.cpu_access_flag = GPUBuffer::CPU_ACCESS_WRITE;
        cb_desc.struct_byte_stride = 0;
        auto cb_ret = device->createBuffer(cb_desc, nullptr);
        if (!cb_ret) {
            LOG(Log::WARNING) << "Failed to create const buffer: " << cb_ret.code.raw_code();
            return false;
        }
        const_buffer_ = cb_ret;

        // 设置光栅化描述，指定多边形如何被渲染.
        GPURasterizerState::Desc ra_desc;
        ra_desc.antialiased_line_enabled = false;
        ra_desc.cull_mode = GPURasterizerState::CullMode::Back;
        ra_desc.depth_clip_enabled = false;
        ra_desc.fill_mode = GPURasterizerState::FillMode::Solid;
        ra_desc.front_counter_clockwise = false;
        ra_desc.multisample_enabled = false;
        ra_desc.scissor_enabled = false;

        // 创建光栅化状态.
        auto ra_ret = device->createRasterizerState(ra_desc);
        if (!ra_ret) {
            LOG(Log::WARNING) << "Failed to create rasterizer state: " << ra_ret.code.raw_code();
            return false;
        }
        rasterizer_state_ = ra_ret;

        is_initialized_ = true;
        return true;
    }

    void ShadowEffectGPU::destroy() {
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

    void ShadowEffectGPU::onDemolish() {
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

        width_ = 0;
        height_ = 0;
        is_initialized_ = false;
    }

    void ShadowEffectGPU::onRebuild(bool succeeded) {
        if (!initialize()) {
            return;
        }

        setRadius(radius_);
    }

    bool ShadowEffectGPU::generate(Canvas* c) {
        if (!is_initialized_) {
            return false;
        }

        if (cache_) {
            return true;
        }

        if (!shadow2_tex2d_) {
            assert(false);
            return false;
        }

        render();

        auto cache = c->createImage(shadow2_tex2d_);
        if (!cache) {
            return false;
        }

        cache_ = cache;
        return true;
    }

    bool ShadowEffectGPU::draw(Canvas* c) {
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

    bool ShadowEffectGPU::setSize(int width, int height, bool hdr) {
        if (width_ == width &&
            height_ == height &&
            is_hdr_enabled_ == hdr)
        {
            return true;
        }

        auto device = Application::getGraphicDeviceManager()->getGPUDevice();

        width_ = width;
        height_ = height;
        is_hdr_enabled_ = hdr;

        viewport_.width = float(width_);
        viewport_.height = float(height_);
        viewport_.min_depth = 0.0f;
        viewport_.max_depth = 1.0f;
        viewport_.x = 0;
        viewport_.y = 0;

        // 顶点缓存
        VertexData vertices[] {
            { 0,            float(height), 0 },
            { float(width), float(height), 0 },
            { float(width), 0,             0 },
            { 0,            0,             0 },
        };

        GPUBuffer::Desc vb_desc;
        vb_desc.is_dynamic = true;
        vb_desc.byte_width = sizeof(VertexData) * ARRAYSIZE(vertices);
        vb_desc.res_type = GPUBuffer::RES_VERTEX_BUFFER;
        vb_desc.cpu_access_flag = GPUBuffer::CPU_ACCESS_WRITE;
        vb_desc.struct_byte_stride = 0;

        GPUBuffer::ResourceData vertex_data;
        vertex_data.sys_mem = vertices;
        vertex_data.pitch = 0;
        vertex_data.slice_pitch = 0;

        auto vb_ret = device->createBuffer(vb_desc, &vertex_data);
        if (!vb_ret) {
            LOG(Log::WARNING) << "Failed to create vertex buffer: " << vb_ret.code.raw_code();
            return false;
        }
        vert_buffer_ = vb_ret;

        // 索引缓存
        int indices[] = {0, 1, 2, 0, 2, 3};

        GPUBuffer::Desc ib_desc;
        ib_desc.is_dynamic = true;
        ib_desc.byte_width = sizeof(int) * ARRAYSIZE(indices);
        ib_desc.res_type = GPUBuffer::RES_INDEX_BUFFER;
        ib_desc.cpu_access_flag = GPUBuffer::CPU_ACCESS_WRITE;
        ib_desc.struct_byte_stride = 0;

        GPUBuffer::ResourceData index_data;
        index_data.sys_mem = indices;
        index_data.pitch = 0;
        index_data.slice_pitch = 0;

        auto ib_ret = device->createBuffer(ib_desc, &index_data);
        if (!ib_ret) {
            LOG(Log::WARNING) << "Failed to create index buffer: " << ib_ret.code.raw_code();
            return false;
        }
        index_buffer_ = ib_ret;

        float pos_x = width_ / 2.f;
        float pos_y = height_ / 2.f;

        // 摄像机位置。
        auto pos = utl::pt3f{ pos_x, pos_y, -2 };
        // 摄像机看向的位置。
        auto look_at = utl::pt3f{ pos_x, pos_y, 0 };
        // 摄像机上向量
        auto up = utl::vec3f{ 0.0f, 1.0f, 0.0f };

        world_matrix_.identity();
        view_matrix_ = utl::math::camera4x4(pos, pos - look_at, up);
        ortho_matrix_ = utl::math::orthoProj4x4<float>(-width_ / 2.f, width_ / 2.f, -height_ / 2.f, height_ / 2.f, 1, 2);
        utl::mat4f adj{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, -0.5f, 2 / 2.f + 1,
            0, 0, 0, 1 };
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

    void ShadowEffectGPU::render() {
        if (!is_initialized_) {
            return;
        }

        auto context =
            Application::getGraphicDeviceManager()->getGPUContext();

        context->setVertexShader(vs_.get());
        context->setPixelShader(hori_ps_.get());
        context->setInputLayout(input_layout_.get());
        context->setPrimitiveTopology(GPUContext::Topology::TriangleList);

        context->setRasterizerState(rasterizer_state_.get());
        context->setViewports(1, &viewport_);

        {
            GPUShaderResource* srvs[] = { bg_srv_.get(), kernel_srv_.get() };
            context->setPShaderResources(0, ARRAYSIZE(srvs), srvs);
            context->setRenderTargets(1, &shadow1_rtv_, nullptr);
        }

        // VS ConstBuffer
        {
            auto data = context->lock(
                const_buffer_.get(), GPUContext::LOCK_WRITE, nullptr);
            if (data) {
                static_cast<ConstBuffer*>(data)->wvo = wvo_matrix_;
                context->unlock(const_buffer_.get());
            }
        }

        context->setVConstantBuffers(0, 1, &const_buffer_);

        // Render
        FLOAT transparent[4] = { 0, 0, 0, 0 };
        context->clearRenderTarget(shadow1_rtv_.get(), transparent);

        UINT vertexDataOffset = 0;
        UINT vertexStructSize = sizeof(VertexData);
        context->setVertexBuffers(0, 1, &vert_buffer_, &vertexStructSize, &vertexDataOffset);
        context->setIndexBuffer(index_buffer_.get(), GPUDataFormat::R32_UINT, 0);
        context->drawIndexed(6, 0, 0);

        context->setPixelShader(vert_ps_.get());
        {
            context->setRenderTargets(1, &shadow2_rtv_, nullptr);
            GPUShaderResource* srvs[] = { shadow1_srv_.get(), bg_srv_.get(), kernel_srv_.get() };
            context->setPShaderResources(0, ARRAYSIZE(srvs), srvs);
        }

        // Render
        context->clearRenderTarget(shadow2_rtv_.get(), transparent);
        context->drawIndexed(6, 0, 0);
    }

    void ShadowEffectGPU::resetCache() {
        cache_.reset();
    }

    bool ShadowEffectGPU::hasCache() const {
        return cache_ != nullptr;
    }

    bool ShadowEffectGPU::setRadius(int radius) {
        if (!is_initialized_) {
            return false;
        }
        if ((radius == radius_ && kernel_tex2d_ && kernel_srv_) ||
            radius <= 0)
        {
            return true;
        }

        radius_ = radius;
        semi_radius_ = radius_ / 2.f;
        cache_.reset();

        return createKernelTexture();
    }

    bool ShadowEffectGPU::setContent(OffscreenBuffer* content) {
        if (!is_initialized_ || !content) {
            return false;
        }

        auto texture = static_cast<const OffscreenBufferWin*>(content)->getTexture();

        auto& desc = texture->getDesc();
        view_width_ = desc.width;
        view_height_ = desc.height;

        int radius = int(std::ceil(radius_ * context_.getAutoScale()));
        int width = view_width_ + radius * 2;
        int height = view_height_ + radius * 2;
        cache_.reset();

        auto device =
            Application::getGraphicDeviceManager()->getGPUDevice();

        auto ret = texture->createSRV();
        if (!ret) {
            LOG(Log::WARNING) << "Failed to create SRV: " << ret.raw_code();
            return false;
        }
        bg_srv_ = texture->srv();

        auto bg_rtv = device->createRenderTarget(texture.get());
        if (!bg_rtv) {
            LOG(Log::WARNING) << "Failed to create RTV: " << bg_rtv.code.raw_code();
            return false;
        }
        bg_rtv_ = bg_rtv;

        return setSize(
            width, height,
            content->getImageOptions().pixel_format == ImagePixelFormat::HDR);
    }

    int ShadowEffectGPU::getRadius() const {
        return radius_;
    }

    GPtr<ImageFrame> ShadowEffectGPU::getOutput() const {
        return cache_;
    }

    bool ShadowEffectGPU::createTexture(
        GPtr<GPUTexture>& tex,
        GPtr<GPURenderTarget>& rtv,
        GPtr<GPUShaderResource>& srv)
    {
        GPUDataFormat format;
        if (is_hdr_enabled_) {
            format = GPUDataFormat::R16G16B16A16_FLOAT;
        } else {
            format = GPUDataFormat::B8G8B8R8_UNORM;
        }

        tex = GPUTexture::createShaderTex2D(width_, height_, format, true);
        if (!tex) {
            return false;
        }

        auto ret = tex->createSRV();
        if (!ret) {
            LOG(Log::WARNING) << "Failed to create SRV: " << ret.raw_code();
            return false;
        }
        srv = tex->srv();

        auto device = Application::getGraphicDeviceManager()->getGPUDevice();

        auto rtv_ret = device->createRenderTarget(tex.get());
        if (!rtv_ret) {
            LOG(Log::WARNING) << "Failed to create RTV: " << rtv_ret.code.raw_code();
            return false;
        }
        rtv = rtv_ret;
        return true;
    }

    bool ShadowEffectGPU::createKernelTexture() {
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

        kernel_tex2d_ = GPUTexture::createShaderTex2D(
            radius + 1, 1, GPUDataFormat::R32_FLOAT, false,
            sizeof(float) * (radius + 1), weight_matrix.get());
        if (!kernel_tex2d_) {
            LOG(Log::WARNING) << "Failed to create 2d texture!";
            return false;
        }

        auto ret = kernel_tex2d_->createSRV();
        if (!ret) {
            LOG(Log::WARNING) << "Failed to create SRV: " << ret.raw_code();
            return false;
        }
        kernel_srv_ = kernel_tex2d_->srv();
        return true;
    }
}
}