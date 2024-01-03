// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gaussian_blur_effect_dx.h"

#include "utils/log.h"
#include "utils/files/file_utils.h"
#include "utils/math/algebra/special_matrix.hpp"
#include "utils/strings/float_conv.h"

#include "ukive/app/application.h"
#include "ukive/graphics/win/offscreen_buffer_win.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/resources/resource_manager.h"
#include "ukive/window/window.h"


namespace {

    double gaussian_dist_w(int x, double sigma) {
        double exponent = -std::pow(x, 2) / (2 * std::pow(sigma, 2));
        return std::exp(exponent) / (std::sqrt(2 * 3.1415926536) * sigma);
    }

    std::string calculate_kernel() {
        constexpr int kCount = 13;
        constexpr int kIgnore = 0;

        double snd_w[kCount];
        double total_snd_w = 0;
        for (int i = 0; i < kCount; ++i) {
            snd_w[i] = gaussian_dist_w(i - kCount / 2, 16);
            if (i >= kIgnore && i < kCount - kIgnore) {
                total_snd_w += snd_w[i];
            }
        }

        std::string result;
        for (int i = 0; i < kCount; ++i) {
            snd_w[i] /= total_snd_w;
            if (i >= kIgnore && i <= kCount / 2) {
                result += utl::ftos8(snd_w[i], 10);
                if (i != kCount / 2) {
                    result += ", ";
                }
            }
        }

        return result;
    }

}

namespace ukive {
namespace win {

    GaussianBlurEffectGPU::GaussianBlurEffectGPU(Context context)
        : width_(0),
          height_(0),
          view_width_(0),
          view_height_(0),
          radius_(0),
          semi_radius_(0.f),
          context_(context)
    {
    }

    GaussianBlurEffectGPU::~GaussianBlurEffectGPU() {}

    bool GaussianBlurEffectGPU::initialize() {
        //calculate_kernel();

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
        res_mgr->getFileData(shader_dir / u"gaussian_blur_effect_vert_ps.cso", &ps_bc);
        auto vert_ps_ret = device->createPixelShader(ps_bc.data(), ps_bc.size());
        if (!vert_ps_ret) {
            LOG(Log::WARNING) << "Failed to create pixel shader: " << vert_ps_ret.code.raw_code();
            return false;
        }
        vert_ps_ = vert_ps_ret;

        ps_bc.clear();
        res_mgr->getFileData(shader_dir / u"gaussian_blur_effect_hori_ps.cso", &ps_bc);
        auto hori_ps_ret = device->createPixelShader(ps_bc.data(), ps_bc.size());
        if (!hori_ps_ret) {
            LOG(Log::WARNING) << "Failed to create pixel shader: " << hori_ps_ret.code.raw_code();
            return false;
        }
        hori_ps_ = hori_ps_ret;

        // 顶点常量缓存
        GPUBuffer::Desc cb_desc;
        cb_desc.is_dynamic = true;
        cb_desc.byte_width = sizeof(VConstBuffer);
        cb_desc.res_type = GPUBuffer::RES_CONSTANT_BUFFER;
        cb_desc.cpu_access_flag = GPUBuffer::CPU_ACCESS_WRITE;
        cb_desc.struct_byte_stride = 0;
        auto cb_ret = device->createBuffer(cb_desc, nullptr);
        if (!cb_ret) {
            LOG(Log::WARNING) << "Failed to create v const buffer: " << cb_ret.code.raw_code();
            return false;
        }
        vconst_buffer_ = cb_ret;

        // 像素常量缓存
        cb_desc.byte_width = sizeof(PConstBuffer);
        cb_ret = device->createBuffer(cb_desc, nullptr);
        if (!cb_ret) {
            LOG(Log::WARNING) << "Failed to create p const buffer: " << cb_ret.code.raw_code();
            return false;
        }
        pconst_buffer_ = cb_ret;

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

    void GaussianBlurEffectGPU::destroy() {
        vs_.reset();
        input_layout_.reset();
        vert_ps_.reset();
        hori_ps_.reset();
        vconst_buffer_.reset();
        pconst_buffer_.reset();
        rasterizer_state_.reset();

        bg_rtv_.reset();
        bg_srv_.reset();
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

    void GaussianBlurEffectGPU::onDemolish() {
        vs_.reset();
        input_layout_.reset();
        vert_ps_.reset();
        hori_ps_.reset();
        vconst_buffer_.reset();
        pconst_buffer_.reset();
        rasterizer_state_.reset();

        bg_rtv_.reset();
        bg_srv_.reset();
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

    void GaussianBlurEffectGPU::onRebuild(bool succeeded) {
        if (!initialize()) {
            return;
        }

        setRadius(radius_);
    }

    bool GaussianBlurEffectGPU::generate(Canvas* c) {
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

    bool GaussianBlurEffectGPU::draw(Canvas* c) {
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

    bool GaussianBlurEffectGPU::setSize(int width, int height, bool hdr) {
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

    void GaussianBlurEffectGPU::render() {
        if (!is_initialized_) {
            return;
        }

        auto context =
            Application::getGraphicDeviceManager()->getGPUContext();

        context->setVertexShader(vs_.get());
        context->setInputLayout(input_layout_.get());
        context->setPrimitiveTopology(GPUContext::Topology::TriangleList);

        context->setRasterizerState(rasterizer_state_.get());
        context->setViewports(1, &viewport_);

        // ConstBuffer
        {
            auto data = context->lock(
                vconst_buffer_.get(), GPUContext::LOCK_WRITE, nullptr);
            if (data) {
                static_cast<VConstBuffer*>(data)->wvo = wvo_matrix_;
                context->unlock(vconst_buffer_.get());
            }
            context->setVConstantBuffers(0, 1, &vconst_buffer_);
        }
        {
            auto data = context->lock(
                pconst_buffer_.get(), GPUContext::LOCK_WRITE, nullptr);
            if (data) {
                static_cast<PConstBuffer*>(data)->rt_size = { (uint32_t)width_, (uint32_t)height_ };
                context->unlock(pconst_buffer_.get());
            }
            context->setPConstantBuffers(0, 1, &pconst_buffer_);
        }

        FLOAT transparent[4] = { 0, 0, 0, 0 };

        UINT vertexDataOffset = 0;
        UINT vertexStructSize = sizeof(VertexData);
        context->setVertexBuffers(0, 1, &vert_buffer_, &vertexStructSize, &vertexDataOffset);
        context->setIndexBuffer(index_buffer_.get(), GPUDataFormat::R32_UINT, 0);

        GPUShaderResource* first_img = bg_srv_.get();
        int count = std::round(std::sqrtf(radius_ * context_.getAutoScale() / (13 / 2.f)));
        if (count < 1) count = 1;

        for (int i = 0; i < count; ++i) {
            // Render hori
            context->setPixelShader(hori_ps_.get());
            {
                context->setRenderTargets(1, &shadow1_rtv_, nullptr);
                GPUShaderResource* srvs[] = { first_img };
                context->setPShaderResources(0, ARRAYSIZE(srvs), srvs);
            }
            context->clearRenderTarget(shadow1_rtv_.get(), transparent);
            context->drawIndexed(6, 0, 0);

            // Render vert
            context->setPixelShader(vert_ps_.get());
            {
                context->setRenderTargets(1, &shadow2_rtv_, nullptr);
                GPUShaderResource* srvs[] = { shadow1_srv_.get() };
                context->setPShaderResources(0, ARRAYSIZE(srvs), srvs);
            }
            context->clearRenderTarget(shadow2_rtv_.get(), transparent);
            context->drawIndexed(6, 0, 0);

            first_img = shadow2_srv_.get();
        }
    }

    void GaussianBlurEffectGPU::resetCache() {
        cache_.reset();
    }

    bool GaussianBlurEffectGPU::hasCache() const {
        return cache_ != nullptr;
    }

    bool GaussianBlurEffectGPU::setRadius(int radius) {
        if (!is_initialized_) {
            return false;
        }
        if (radius == radius_ || radius <= 0) {
            return true;
        }

        radius_ = radius;
        semi_radius_ = radius_ / 2.f;
        cache_.reset();
        return true;
    }

    bool GaussianBlurEffectGPU::setContent(OffscreenBuffer* content) {
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

    int GaussianBlurEffectGPU::getRadius() const {
        return radius_;
    }

    GPtr<ImageFrame> GaussianBlurEffectGPU::getOutput() const {
        return cache_;
    }

    bool GaussianBlurEffectGPU::createTexture(
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

}
}