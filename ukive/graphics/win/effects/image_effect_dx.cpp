// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_effect_dx.h"

#include "utils/log.h"
#include "utils/files/file_utils.h"
#include "utils/math/algebra/special_matrix.hpp"

#include "ukive/app/application.h"
#include "ukive/graphics/win/offscreen_buffer_win.h"
#include "ukive/graphics/win/images/image_frame_win.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/resources/resource_manager.h"
#include "ukive/window/window.h"


namespace ukive {
namespace win {

    ImageEffectGPU::ImageEffectGPU()
        : width_(0),
          height_(0),
          viewport_()
    {
    }

    ImageEffectGPU::~ImageEffectGPU() {}

    bool ImageEffectGPU::initialize() {
        // 顶点着色器
        auto device =
            Application::getGraphicDeviceManager()->getGPUDevice();

        if (!setVertexShader(u"image_effect_common_vs.cso")) {
            return false;
        }

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
        vconst_buffer_ = cb_ret;

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

    void ImageEffectGPU::destroy() {
        vs_.reset();
        input_layout_.reset();
        ps_.reset();
        vconst_buffer_.reset();
        pconst_buffer_.reset();
        rasterizer_state_.reset();

        org_srvs_.clear();
        target_tex2d_.reset();
        target_rtv_.reset();
        target_srv_.reset();
        vert_buffer_.reset();
        index_buffer_.reset();

        cache_.reset();
        cached_shaders_.clear();

        width_ = 0;
        height_ = 0;
        is_initialized_ = false;
    }

    bool ImageEffectGPU::generate(Canvas* c) {
        if (!is_initialized_) {
            return false;
        }

        if (cache_) {
            return true;
        }

        if (!target_tex2d_) {
            ubassert(false);
            return false;
        }

        render();

        if (!c) {
            return true;
        }

        auto image = c->createImage(target_tex2d_);
        if (!image) {
            return false;
        }

        cache_ = image;
        return true;
    }

    bool ImageEffectGPU::draw(Canvas* c) {
        if (!is_initialized_) {
            return false;
        }

        if (!cache_ && !generate(c)) {
            return false;
        }

        c->drawImage(c->getOpacity(), cache_.get());
        return true;
    }

    bool ImageEffectGPU::setSize(int width, int height, GPUDataFormat format) {
        if (width_ == width && height_ == height && format_ == format) {
            return true;
        }

        auto device = Application::getGraphicDeviceManager()->getGPUDevice();

        viewport_.width = float(width);
        viewport_.height = float(height);
        viewport_.min_depth = 0.0f;
        viewport_.max_depth = 1.0f;
        viewport_.x = 0;
        viewport_.y = 0;

        // 顶点缓存
        VertexData vertices[] {
            { {0,            float(height), 0}, {0, 1} },
            { {float(width), float(height), 0}, {1, 1} },
            { {float(width), 0,             0}, {1, 0} },
            { {0,            0,             0}, {0, 0} },
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

        float pos_x = width / 2.f;
        float pos_y = height / 2.f;

        // 摄像机位置。
        auto pos = utl::pt3f{ pos_x, pos_y, -2 };
        // 摄像机看向的位置。
        auto look_at = utl::pt3f{ pos_x, pos_y, 0 };
        // 摄像机上向量
        auto up = utl::vec3f{ 0.0f, 1.0f, 0.0f };

        world_matrix_.identity();
        view_matrix_ = utl::math::camera4x4(pos, pos - look_at, up);
        ortho_matrix_ = utl::math::orthoProj4x4<float>(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 1, 2);
        utl::mat4f adj{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, -0.5f, 2 / 2.f + 1,
            0, 0, 0, 1 };
        ortho_matrix_.mul(adj);

        wvo_matrix_ = ortho_matrix_ * view_matrix_ * world_matrix_;

        width_ = width;
        height_ = height;
        format_ = format;

        if (!createTexture(format, target_tex2d_, target_rtv_, target_srv_)) {
            return false;
        }

        return true;
    }

    void ImageEffectGPU::render() {
        if (!is_initialized_) {
            return;
        }

        auto context =
            Application::getGraphicDeviceManager()->getGPUContext();

        context->setVertexShader(vs_.get());
        context->setPixelShader(ps_.get());
        context->setInputLayout(input_layout_.get());
        context->setPrimitiveTopology(GPUContext::Topology::TriangleList);

        context->setRasterizerState(rasterizer_state_.get());
        context->setViewports(1, &viewport_);
        context->setRenderTargets(1, &target_rtv_, nullptr);

        if (!org_srvs_.empty()) {
            GPUShaderResource* srvs[D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
            size_t count = (std::min)(ARRAYSIZE(srvs), org_srvs_.size());
            for (size_t i = 0; i < count; ++i) {
                srvs[i] = org_srvs_[i].get();
            }
            context->setPShaderResources(0, count, srvs);
        }

        // VS ConstBuffer
        {
            auto data = context->lock(
                vconst_buffer_.get(), GPUContext::LOCK_WRITE, nullptr);
            if (data) {
                static_cast<ConstBuffer*>(data)->wvo = wvo_matrix_;
                context->unlock(vconst_buffer_.get());
            }
            context->setVConstantBuffers(0, 1, &vconst_buffer_);
        }

        // PS ConstBuffer
        if (pconst_buffer_ && param_update_handler_) {
            auto data = context->lock(
                pconst_buffer_.get(), GPUContext::LOCK_WRITE, nullptr);
            if (data) {
                if (param_update_handler_) {
                    param_update_handler_(data);
                }
                context->unlock(pconst_buffer_.get());
            }
            context->setPConstantBuffers(0, 1, &pconst_buffer_);
        }

        // Render
        FLOAT transparent[4] = { 0, 0, 0, 0 };
        context->clearRenderTarget(target_rtv_.get(), transparent);

        UINT vertexDataOffset = 0;
        UINT vertexStructSize = sizeof(VertexData);
        context->setVertexBuffers(0, 1, &vert_buffer_, &vertexStructSize, &vertexDataOffset);
        context->setIndexBuffer(index_buffer_.get(), GPUDataFormat::R32_UINT, 0);
        context->drawIndexed(6, 0, 0);
    }

    bool ImageEffectGPU::addInput(OffscreenBuffer* content) {
        if (!is_initialized_ || !content) {
            return false;
        }

        auto texture = static_cast<const OffscreenBufferWin*>(content)->getTexture();
        return addInput(texture);
    }

    bool ImageEffectGPU::addInput(const GPtr<GPUTexture>& texture) {
        if (!is_initialized_) {
            return false;
        }

        cache_.reset();

        if (texture) {
            auto ret = texture->createSRV();
            if (ret.raw_code() != 0) {
                LOG(Log::WARNING) << "Failed to create SRV: " << ret.raw_code();
                return false;
            }
            org_srvs_.push_back(texture->srv());
        } else {
            org_srvs_.push_back({});
        }

        return true;
    }

    void ImageEffectGPU::clearInputs() {
        cache_.reset();
        org_srvs_.clear();
    }

    bool ImageEffectGPU::setOutputFormat(
        unsigned int width,
        unsigned int height,
        GPUDataFormat format)
    {
        cache_.reset();
        return setSize(width, height, format);
    }

    GPtr<ImageFrame> ImageEffectGPU::getOutput() const {
        return cache_;
    }

    GPtr<GPUTexture> ImageEffectGPU::getOutputTexture() const {
        return target_tex2d_;
    }

    bool ImageEffectGPU::setVertexShader(const std::u16string& name) {
        auto device =
            Application::getGraphicDeviceManager()->getGPUDevice();

        auto res_mgr = Application::getResourceManager();
        auto shader_dir = res_mgr->getResRootPath() / u"shaders";

        std::string vs_bc;
        res_mgr->getFileData(shader_dir / name, &vs_bc);
        auto vs_ret = device->createVertexShader(vs_bc.data(), vs_bc.size());
        if (!vs_ret) {
            LOG(Log::WARNING) << "Failed to create vertex shader: " << vs_ret.code.raw_code();
            return false;
        }
        vs_ = vs_ret;

        using GILD = GPUInputLayout::Desc;
        GILD layout[2];
        layout[0] = GILD("POSITION", GPUDataFormat::R32G32B32_FLOAT, 0, 0, false);
        layout[1] = GILD("TEXCOORD", GPUDataFormat::R32G32_FLOAT);
        auto il_ret = device->createInputLayout(layout, ARRAYSIZE(layout), vs_bc.data(), vs_bc.size());
        if (!il_ret) {
            LOG(Log::WARNING) << "Failed to create input layout: " << il_ret.code.raw_code();
            return false;
        }
        input_layout_ = il_ret;
        return true;
    }

    bool ImageEffectGPU::setPixelShader(const std::u16string& name) {
        if (cached_shaders_.find(name) != cached_shaders_.end()) {
            ps_ = cached_shaders_[name];
            return true;
        }
        auto device =
            Application::getGraphicDeviceManager()->getGPUDevice();

        auto res_mgr = Application::getResourceManager();
        auto shader_dir = res_mgr->getResRootPath() / u"shaders";

        std::string ps_bc;
        res_mgr->getFileData(shader_dir / name, &ps_bc);
        auto ps_ret = device->createPixelShader(ps_bc.data(), ps_bc.size());
        if (!ps_ret) {
            LOG(Log::WARNING) << "Failed to create pixel shader: " << ps_ret.code.raw_code();
            return false;
        }
        ps_ = ps_ret;
        cached_shaders_[name] = ps_;
        return true;
    }

    bool ImageEffectGPU::setParameterSize(uint32_t size) {
        if (pconst_buffer_ &&
            pconst_buffer_->getDesc().byte_width == size)
        {
            return true;
        }

        auto device =
            Application::getGraphicDeviceManager()->getGPUDevice();

        GPUBuffer::Desc cb_desc;
        cb_desc.is_dynamic = true;
        cb_desc.byte_width = size;
        cb_desc.res_type = GPUBuffer::RES_CONSTANT_BUFFER;
        cb_desc.cpu_access_flag = GPUBuffer::CPU_ACCESS_WRITE;
        cb_desc.struct_byte_stride = 0;
        auto cb_ret = device->createBuffer(cb_desc, nullptr);
        if (!cb_ret) {
            LOG(Log::WARNING) << "Failed to create p const buffer: " << cb_ret.code.raw_code();
            return false;
        }
        pconst_buffer_ = cb_ret;
        return true;
    }

    void ImageEffectGPU::setParameterUpdateHandler(const ParameterUpdateHandler& h) {
        param_update_handler_ = h;
    }

    bool ImageEffectGPU::updateParameters() {
        cache_.reset();
        return true;
    }

    bool ImageEffectGPU::createTexture(
        GPUDataFormat format,
        GPtr<GPUTexture>& tex,
        GPtr<GPURenderTarget>& rtv,
        GPtr<GPUShaderResource>& srv)
    {
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