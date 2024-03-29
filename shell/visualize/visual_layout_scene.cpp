// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "visual_layout_scene.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/event/input_event.h"
#include "ukive/graphics/gpu/gpu_types.h"
#include "ukive/graphics/gpu/gpu_context.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/3d/space_object_manager.h"
#include "ukive/graphics/gpu/gpu_shader.h"
#include "ukive/resources/resource_manager.h"
#include "ukive/graphics/3d/camera.h"
#include "ukive/event/keyboard.h"
#include "ukive/graphics/3d/camera.h"
#include "ukive/views/space3d_view.h"


namespace vsul {

    VisualLayoutScene::VisualLayoutScene() {}
    VisualLayoutScene::~VisualLayoutScene() {}

    void VisualLayoutScene::onSceneCreate(ukive::Space3DView* d3d_view) {
        space_view_ = d3d_view;

        space_obj_mgr_ = new ukv3d::SpaceObjectManager();

        camera_ = new ukv3d::Camera(1, 1);
        camera_->setCameraPosition(1024, 1024, -1024);
        //camera_->circuleCamera2(1.f, -0.2f);

        assist_configure_.init();
        model_configure_.init();

        auto obj = assist_configure_.createWorldAxisObj(0, 1024);
        space_obj_mgr_->add(obj);

        obj = model_configure_.createCubeObj(1, 200);
        space_obj_mgr_->add(obj);

        createStates();
    }

    void VisualLayoutScene::onSceneResize(int width, int height) {
        camera_->resize(width, height);
        setViewports(0, 0, float(width), float(height));

        width_ = width;
        height_ = height;

        releaseResources();
        createResources(width, height);
    }

    bool VisualLayoutScene::onSceneInput(ukive::InputEvent* e) {
        bool result = Scene::onSceneInput(e);

        switch (e->getEvent()) {
        case ukive::InputEvent::EVM_DOWN:
            if (e->getMouseKey() == ukive::InputEvent::MK_LEFT) {
                is_mouse_left_key_pressed_ = true;
                prev_x_ = e->getX();
                prev_y_ = e->getY();
                result = true;
            }
            break;

        case ukive::InputEvent::EVM_MOVE:
            if (is_mouse_left_key_pressed_ == true) {
                mouse_action_mode_ = MOUSE_ACTION_MOVED;

                int dx = e->getX() - prev_x_;
                int dy = e->getY() - prev_y_;

                if (ukive::Keyboard::isKeyPressed(ukive::Keyboard::KEY_SHIFT)) {
                    camera_->circuleCamera2(
                        float(-dx) / width_ * 4,
                        float(-dy) / height_ * 4);
                    space_view_->requestDraw();
                } else if (ukive::Keyboard::isKeyPressed(ukive::Keyboard::KEY_CONTROL)) {
                    camera_->moveCamera(float(-dx), float(dy));
                    space_view_->requestDraw();
                }

                prev_x_ = e->getX();
                prev_y_ = e->getY();
            }
            break;

        case ukive::InputEvent::EVM_UP:
            if (e->getMouseKey() == ukive::InputEvent::MK_LEFT) {
                if (mouse_action_mode_ != MOUSE_ACTION_MOVED) {
                }

                is_mouse_left_key_pressed_ = false;
            }

            mouse_action_mode_ = MOUSE_ACTION_NONE;
            break;

        case ukive::InputEvent::EVK_DOWN:
            if (e->getKeyboardKey() == ukive::Keyboard::KEY_SHIFT) {
                is_shift_key_pressed_ = true;
            }
            if (e->getKeyboardKey() == ukive::Keyboard::KEY_CONTROL) {
                is_ctrl_key_pressed_ = true;
            }
            break;

        case ukive::InputEvent::EVK_UP:
            if (e->getKeyboardKey() == ukive::Keyboard::KEY_SHIFT) {
                is_shift_key_pressed_ = false;
            }
            if (e->getKeyboardKey() == ukive::Keyboard::KEY_CONTROL) {
                is_ctrl_key_pressed_ = false;
            }
            break;

        case ukive::InputEvent::EVM_WHEEL:
            camera_->scaleCamera(e->getWheelValue() > 0 ? 0.9f : 1.1f);
            space_view_->requestDraw();
            break;
        default:
            break;
        }

        return result;
    }

    void VisualLayoutScene::onSceneRender(ukive::GPUContext* context, ukive::GPURenderTarget* rt) {
        utl::mat4f wvp_matrix;

        context->setRasterizerState(rasterizer_state_.get());
        context->setViewports(1, &viewport_);

        //d3d_dc->OMSetDepthStencilState(depth_stencil_state_.get(), 1);
        //d3d_dc->PSSetSamplers(0, 1, &sampler_state_);
        // d3d_dc->PSSetShaderResources(0, 1, &shader_resource_view_);
        //d3d_dc->ClearDepthStencilView(depth_stencil_view_.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        camera_->getWVPMatrix(&wvp_matrix);
        utl::mat4f wvp_matrix_t = wvp_matrix.T();

        // Assist
        assist_configure_.active(context);
        assist_configure_.setMatrix(context, wvp_matrix_t);
        context->setPrimitiveTopology(ukive::GPUContext::Topology::LineList);
        space_obj_mgr_->draw(context, 0);

        // Panel
        model_configure_.active(context);
        model_configure_.setMatrix(context, wvp_matrix_t);
        context->setPrimitiveTopology(ukive::GPUContext::Topology::TriangleList);
        space_obj_mgr_->draw(context, 1);
    }

    void VisualLayoutScene::onSceneDestroy() {
        delete space_obj_mgr_;
        delete camera_;
        assist_configure_.close();
        model_configure_.close();
        releaseStates();
        releaseResources();
    }

    void VisualLayoutScene::onGraphicDeviceLost() {
        releaseStates();
        releaseResources();
        assist_configure_.close();
        model_configure_.close();
        space_obj_mgr_->notifyGraphicDeviceLost();
    }

    void VisualLayoutScene::onGraphicDeviceRestored() {
        createStates();
        assist_configure_.init();
        model_configure_.init();
        space_obj_mgr_->notifyGraphicDeviceRestored();
    }

    bool VisualLayoutScene::createStates() {
        auto device = ukive::Application::getGraphicDeviceManager()->getGPUDevice();

        // 设置深度模版状态描述
        ukive::GPUDepthStencilState::Desc ds_desc;
        ds_desc.depth_enabled = true;
        ds_desc.depth_write_mask = ukive::GPUDepthStencilState::WriteMask::All;

        // D3D11_DEPTH_WRITE_MASK_ZERO 禁止写深度缓冲
        ds_desc.depth_func = ukive::ComparisonFunc::Less;
        ds_desc.stencil_enabled = true;
        ds_desc.stencil_read_mask = 0xFF;
        ds_desc.stencil_write_mask = 0xFF;

        // 对于front face 像素使用的模版操作操作
        ds_desc.front_face.stencil_fail_op = ukive::GPUDepthStencilState::StencilOp::Keep;
        ds_desc.front_face.stencil_depth_fail_op = ukive::GPUDepthStencilState::StencilOp::Incr;
        ds_desc.front_face.stencil_pass_op = ukive::GPUDepthStencilState::StencilOp::Keep;
        ds_desc.front_face.stencil_func = ukive::ComparisonFunc::Always;

        // 对于back face像素使用的模版操作模式
        ds_desc.back_face.stencil_fail_op = ukive::GPUDepthStencilState::StencilOp::Keep;
        ds_desc.back_face.stencil_depth_fail_op = ukive::GPUDepthStencilState::StencilOp::Decr;
        ds_desc.back_face.stencil_pass_op = ukive::GPUDepthStencilState::StencilOp::Keep;
        ds_desc.back_face.stencil_func = ukive::ComparisonFunc::Always;
        depth_stencil_state_ = device->createDepthStencilState(ds_desc);
        if (!depth_stencil_state_) {
            LOG(Log::WARNING) << "Failed to create depth stencil state";
            return false;
        }

        // 设置光栅化描述，指定多边形如何被渲染.
        ukive::GPURasterizerState::Desc desc;
        desc.antialiased_line_enabled = false;
        desc.cull_mode = ukive::GPURasterizerState::CullMode::None;
        desc.depth_clip_enabled = true;
        desc.fill_mode = ukive::GPURasterizerState::FillMode::Solid;
        desc.front_counter_clockwise = false;
        desc.multisample_enabled = false;
        desc.scissor_enabled = false;
        rasterizer_state_ = device->createRasterizerState(desc);
        if (!rasterizer_state_) {
            LOG(Log::WARNING) << "Failed to create rasterizer state";
            return false;
        }

        ukive::GPUSamplerState::Desc ss_desc;
        ss_desc.filter = ukive::GPUSamplerState::Filter::MinMagPointMipLinear;
        ss_desc.addr_u = ukive::GPUSamplerState::TextureAddrMode::Wrap;
        ss_desc.addr_v = ukive::GPUSamplerState::TextureAddrMode::Wrap;
        ss_desc.addr_w = ukive::GPUSamplerState::TextureAddrMode::Wrap;
        ss_desc.mip_lod_bias = 0;
        ss_desc.max_anisotropy = 1;
        ss_desc.comp_func = ukive::ComparisonFunc::Always;
        ss_desc.border_color[0] = 0;
        ss_desc.border_color[1] = 0;
        ss_desc.border_color[2] = 0;
        ss_desc.border_color[3] = 0;
        ss_desc.min_lod = 0;
        ss_desc.max_lod = (std::numeric_limits<float>::max)();
        sampler_state_ = device->createSamplerState(ss_desc);
        if (!sampler_state_) {
            LOG(Log::WARNING) << "Failed to create sampler state";
            return false;
        }
        return true;
    }

    void VisualLayoutScene::releaseStates() {
        depth_stencil_state_.reset();
        rasterizer_state_.reset();
        sampler_state_.reset();
    }

    bool VisualLayoutScene::createResources(int width, int height) {
        if (width <= 0 || height <= 0) {
            return false;
        }

        auto device = ukive::Application::getGraphicDeviceManager()->getGPUDevice();

        // Depth buffer
        ukive::GPUTexture::Desc tex_desc;
        tex_desc.format = ukive::GPUDataFormat::D24_UNORM_S8_UINT;
        tex_desc.width = width;
        tex_desc.height = height;
        tex_desc.depth = 1;
        tex_desc.mip_levels = 1;
        tex_desc.res_type = ukive::GPUResource::RES_DEPTH_STENCIL;
        tex_desc.is_dynamic = false;
        tex_desc.dim = ukive::GPUTexture::Dimension::_2D;
        depth_stencil_buffer_ = device->createTexture(tex_desc, nullptr);
        if (!depth_stencil_buffer_) {
            LOG(Log::WARNING) << "Failed to create 2d texture";
            return false;
        }

        // 设置深度模版视图描述
        ukive::GPUDepthStencil::Desc dsv_desc;
        dsv_desc.format = ukive::GPUDataFormat::D24_UNORM_S8_UINT;
        dsv_desc.view_dim = ukive::GPUDepthStencil::DSV_DIMENSION_TEXTURE2D;

        depth_stencil_view_ = device->createDepthStencil(dsv_desc, depth_stencil_buffer_.get());
        if (!depth_stencil_view_) {
            LOG(Log::WARNING) << "Failed to create depth stencil view";
            return false;
        }

        // std::wstring ddsFileName = ukive::Application::getExecFileName(true);
        // RH(DirectX::CreateDDSTextureFromFile(
        // d3d_device.get(), ddsFileName + L"\\top.dds", 0, &shader_resource_view_));
        return true;
    }

    void VisualLayoutScene::releaseResources() {
        depth_stencil_buffer_.reset();
        depth_stencil_view_.reset();
        shader_resource_view_.reset();
    }

    void VisualLayoutScene::setViewports(float x, float y, float width, float height) {
        viewport_.width = width;
        viewport_.height = height;
        viewport_.min_depth = 0.0f;
        viewport_.max_depth = 1.0f;
        viewport_.x = x;
        viewport_.y = y;
    }

}
