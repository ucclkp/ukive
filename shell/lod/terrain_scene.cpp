// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "terrain_scene.h"

#include "utils/log.h"
#include "utils/math/algebra/geocal.hpp"

#include "ukive/app/application.h"
#include "ukive/event/input_event.h"
#include "ukive/event/keyboard.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/3d/space_object_manager.h"
#include "ukive/graphics/3d/camera.h"
#include "ukive/graphics/3d/space_object.h"
#include "ukive/graphics/gpu/gpu_rasterizer_state.h"
#include "ukive/views/text_view.h"
#include "ukive/views/space3d_view.h"
#include "ukive/window/window.h"

#include "shell/lod/lod_generator.h"
#include "shell/lod/terrain_configure.h"

#include "ukive/graphics/win/3d/assist_configure.h"
#include "ukive/graphics/win/3d/model_configure.h"


namespace {
    const int kNormalCube = 0;
    const int kWorldAxis = 1;
}

namespace shell {

    TerrainScene::TerrainScene()
        : prev_x_(0),
          prev_y_(0),
          width_(0),
          height_(0),
          assist_configure_(nullptr),
          model_configure_(nullptr),
          terrain_configure_(nullptr),
          space_view_(nullptr)
    {
        mouse_action_mode_ = MOUSE_ACTION_NONE;
        is_ctrl_key_pressed_ = false;
        is_shift_key_pressed_ = false;
        is_mouse_left_key_pressed_ = false;

        space_obj_mgr_ = new ukv3d::SpaceObjectManager();

        camera_ = new ukv3d::Camera(1, 1);
        lod_generator_ = new LodGenerator(8192, 5);

        createBuffers();
    }

    TerrainScene::~TerrainScene() {
        delete lod_generator_;

        delete space_obj_mgr_;
        delete camera_;
    }

    void TerrainScene::setRenderListener(std::function<void()>&& l) {
        on_render_handler_ = l;
    }

    void TerrainScene::createBuffers() {
        auto device = ukive::Application::getGraphicDeviceManager()->getGPUDevice();

        ukive::GPUBuffer::Desc vb;
        vb.is_dynamic = true;
        vb.byte_width = sizeof(TerrainVertexData) * lod_generator_->getVertexCount();
        vb.res_type = ukive::GPUResource::RES_VERTEX_BUFFER;
        vb.cpu_access_flag = ukive::GPUResource::CPU_ACCESS_WRITE;
        vb.struct_byte_stride = 0;

        ukive::GPUBuffer::ResourceData vb_data;
        vb_data.sys_mem = lod_generator_->getVertices();
        vb_data.pitch = 0;
        vb_data.slice_pitch = 0;
        vertex_buffer_ = device->createBuffer(vb, &vb_data);

        ukive::GPUBuffer::Desc ib;
        ib.is_dynamic = true;
        ib.byte_width = sizeof(int) * lod_generator_->getMaxIndexCount();
        ib.res_type = ukive::GPUResource::RES_INDEX_BUFFER;
        ib.cpu_access_flag = ukive::GPUResource::CPU_ACCESS_WRITE;
        ib.struct_byte_stride = 0;

        ukive::GPUBuffer::ResourceData ib_data;
        ib_data.sys_mem = lod_generator_->getIndices();
        ib_data.pitch = 0;
        ib_data.slice_pitch = 0;
        index_buffer_ = device->createBuffer(ib, &ib_data);
    }

    void TerrainScene::createStates() {
        auto device = ukive::Application::getGraphicDeviceManager()->getGPUDevice();

        // 设置光栅化描述，指定多边形如何被渲染.
        ukive::GPURasterizerState::Desc desc;
        desc.antialiased_line_enabled = false;
        desc.cull_mode = ukive::GPURasterizerState::CullMode::Back;
        desc.depth_clip_enabled = true;
        desc.fill_mode = ukive::GPURasterizerState::FillMode::WireFrame;
        desc.front_counter_clockwise = false;
        desc.multisample_enabled = false;
        desc.scissor_enabled = false;
        rasterizer_state_ = device->createRasterizerState(desc);
    }

    void TerrainScene::updateCube() {
        auto object = getSpaceObjectManager()->getByTag(kNormalCube);
        if (object) {
            auto gpu_context = ukive::Application::getGraphicDeviceManager()->getGPUContext();

            auto data = gpu_context->lock(
                object->vertex_buffer.get(), ukive::GPUContext::LOCK_WRITE, nullptr);
            if (data) {
                auto locked_vd = static_cast<ukive::ModelVertexData*>(data);
                auto object_vd = static_cast<ukive::ModelVertexData*>(object->vertices.get());

                object_vd[0].position.x() += 0.1f;

                for (unsigned int i = 0; i < object->vertex_count; ++i) {
                    locked_vd[i].position = object_vd[i].position;
                    locked_vd[i].color = object_vd[i].color;
                    locked_vd[i].normal = object_vd[i].normal;
                    locked_vd[i].texcoord = object_vd[i].texcoord;
                }

                gpu_context->unlock(object->vertex_buffer.get());
            }
        }
    }

    void TerrainScene::updateLodTerrain() {
        if (!index_buffer_) {
            return;
        }
        auto gpu_context = ukive::Application::getGraphicDeviceManager()->getGPUContext();

        auto data = gpu_context->lock(
            index_buffer_.get(), ukive::GPUContext::LOCK_WRITE, nullptr);
        if (data) {
            int* indices = static_cast<int*>(data);

            auto vPosition = getCamera()->getCameraPos();

            utl::mat4f wvpMatrix;
            getCamera()->getWVPMatrix(&wvpMatrix);

            lod_generator_->renderLodTerrain(*vPosition, wvpMatrix, indices);

            gpu_context->unlock(index_buffer_.get());
        }
    }

    void TerrainScene::elementAwareness(int ex, int ey) {
        utl::pt3f ori;
        utl::vec3f dir;
        getPickLine(ex, ey, &ori, &dir);

        bool isHitVer = false;
        utl::pt3d vPos;
        auto *object = getSpaceObjectManager()->getByTag(kNormalCube);
        if (object) {
            auto vData = static_cast<ukive::ModelVertexData*>(object->vertices.get());
            for (unsigned int i = 0; i < object->vertex_count; ++i) {
                auto pos = utl::pt3d(vData[i].position);
                auto distance = utl::math::distanceLinePoint3(
                    utl::pt3d(ori), utl::pt3d(ori + dir), pos);
                if (distance < 20.f) {
                    vPos = pos;
                    isHitVer = true;
                }
            }

            if (isHitVer) {
                if (!space_obj_mgr_->contains(155)) {
                    auto obj = assist_configure_->createBlockObj(155, utl::pt3f(vPos), 10.f);
                    space_obj_mgr_->add(obj);
                    space_view_->requestDraw();
                }
            } else {
                if (getSpaceObjectManager()->contains(155)) {
                    getSpaceObjectManager()->removeByTag(155);
                    space_view_->requestDraw();
                }
            }
        }
    }

    void TerrainScene::getPickLine(
        int sx, int sy, utl::pt3f* line_org, utl::vec3f* line_dir)
    {
        auto worldMatrix = camera_->getWorldMatrix();
        auto viewMatrix = camera_->getViewMatrix();
        auto projectionMatrix = camera_->getProjectionMatrix();

        auto vx = (2.0f * sx / width_ - 1.0f) / projectionMatrix->get(0, 0);
        auto vy = (-2.0f * sy / height_ + 1.0f) / projectionMatrix->get(1, 1);

        utl::hvec4d rayDir{ vx, vy, 1.0f, 0 };
        utl::hvec4d rayOrigin{ 0.0f, 0.0f, 0.0f, 1 };

        auto world = utl::mat4d(*worldMatrix);
        auto view = utl::mat4d(*viewMatrix);

        utl::mat4d inverseView = view.inverse();

        rayDir = (rayDir * inverseView).nor();
        rayOrigin = rayOrigin * inverseView;

        utl::mat4d inverseWorld = world.inverse();

        rayDir = (rayDir * inverseWorld).nor();
        rayOrigin = rayOrigin * inverseWorld;

        *line_org = { float(rayOrigin.x()), float(rayOrigin.y()), float(rayOrigin.z()) };
        *line_dir = utl::vec3f(rayDir.reduce<3>().T());
    }

    void TerrainScene::setViewports(float x, float y, float width, float height) {
        viewport_.width = width;
        viewport_.height = height;
        viewport_.min_depth = 0.0f;
        viewport_.max_depth = 1.0f;
        viewport_.x = x;
        viewport_.y = y;
    }

    void TerrainScene::recreate(int level) {
        if (level > 0 && level != lod_generator_->getLevel()) {
            delete lod_generator_;
            lod_generator_ = new LodGenerator(8192, level);
            createBuffers();
        }
    }

    void TerrainScene::reevaluate(float c1, float c2) {
        lod_generator_->setCoefficient(c1, c2);
        updateLodTerrain();
    }

    void TerrainScene::onSceneCreate(ukive::Space3DView* view) {
        space_view_ = view;

        getCamera()->setCameraPosition(1024, 1024, -1024);
        getCamera()->circuleCamera2(1.f, -0.2f);

        assist_configure_ = new ukive::AssistConfigure();
        assist_configure_->init();

        model_configure_ = new ukive::ModelConfigure();
        model_configure_->init();

        terrain_configure_ = new TerrainConfigure();
        terrain_configure_->init();

        auto obj = assist_configure_->createWorldAxisObj(kWorldAxis, 1024);
        space_obj_mgr_->add(obj);

        obj = model_configure_->createCubeObj(kNormalCube, 100);
        space_obj_mgr_->add(obj);

        createStates();
        updateLodTerrain();
    }

    void TerrainScene::onSceneResize(int width, int height) {
        Scene::onSceneResize(width, height);

        width_ = width;
        height_ = height;

        setViewports(0, 0, float(width), float(height));
        camera_->resize(width, height);

        updateLodTerrain();
    }

    bool TerrainScene::onSceneInput(ukive::InputEvent *e) {
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
                    getCamera()->circuleCamera2(
                        float(-dx) / width_,
                        float(-dy) / height_);

                    updateLodTerrain();
                    space_view_->requestDraw();
                } else if (ukive::Keyboard::isKeyPressed(ukive::Keyboard::KEY_CONTROL)) {
                    getCamera()->moveCamera(float(-dx), float(dy));

                    updateLodTerrain();
                    space_view_->requestDraw();
                }

                prev_x_ = e->getX();
                prev_y_ = e->getY();
            } else {
                elementAwareness(e->getX(), e->getY());
            }
            break;

        case ukive::InputEvent::EVM_UP:
            if (e->getMouseKey() == ukive::InputEvent::MK_LEFT) {
                if (mouse_action_mode_ != MOUSE_ACTION_MOVED) {
                    updateCube();
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
            getCamera()->scaleCamera(e->getWheelValue() > 0 ? 0.9f : 1.1f);
            updateLodTerrain();
            space_view_->requestDraw();
            break;
        default:
            break;
        }

        return result;
    }

    void TerrainScene::onSceneRender(
        ukive::GPUContext* context, ukive::GPURenderTarget* rt)
    {
        on_render_handler_();

        utl::mat4f wvp_matrix;

        context->setRasterizerState(rasterizer_state_.get());
        context->setViewports(1, &viewport_);

        getCamera()->getWVPMatrix(&wvp_matrix);
        utl::mat4f wvp_matrix_t = wvp_matrix.T();

        assist_configure_->active(context);
        assist_configure_->setMatrix(context, wvp_matrix_t);

        context->setPrimitiveTopology(ukive::GPUContext::Topology::LineList);
        getSpaceObjectManager()->draw(context, kWorldAxis);

        terrain_configure_->active(context);
        terrain_configure_->setMatrix(context, wvp_matrix_t);

        context->setPrimitiveTopology(ukive::GPUContext::Topology::TriangleList);

        unsigned int vertexDataOffset = 0;
        unsigned int vertexStructSize = sizeof(TerrainVertexData);
        context->setVertexBuffers(
            0, 1, &vertex_buffer_, &vertexStructSize, &vertexDataOffset);
        context->setIndexBuffer(index_buffer_.get(), ukive::GPUDataFormat::R32_UINT, 0);
        context->drawIndexed(lod_generator_->getIndexCount(), 0, 0);

        context->setPrimitiveTopology(ukive::GPUContext::Topology::TriangleList);
        space_obj_mgr_->draw(context, 155);

        model_configure_->active(context);
        model_configure_->setMatrix(context, wvp_matrix_t);

        context->setPrimitiveTopology(ukive::GPUContext::Topology::TriangleList);
        space_obj_mgr_->draw(context, kNormalCube);
    }

    void TerrainScene::onSceneDestroy() {
        Scene::onSceneDestroy();

        rasterizer_state_.reset();

        if (assist_configure_) {
            assist_configure_->close();
            delete assist_configure_;
            assist_configure_ = nullptr;
        }

        if (model_configure_) {
            model_configure_->close();
            delete model_configure_;
            model_configure_ = nullptr;
        }

        if (terrain_configure_) {
            terrain_configure_->close();
            delete terrain_configure_;
            terrain_configure_ = nullptr;
        }
    }

    void TerrainScene::onGraphicDeviceLost() {
        if (assist_configure_) {
            assist_configure_->close();
        }

        if (model_configure_) {
            model_configure_->close();
        }

        if (terrain_configure_) {
            terrain_configure_->close();
        }

        vertex_buffer_.reset();
        index_buffer_.reset();
        rasterizer_state_.reset();

        space_obj_mgr_->notifyGraphicDeviceLost();
    }

    void TerrainScene::onGraphicDeviceRestored() {
        if (assist_configure_) {
            assist_configure_->init();
        }

        if (model_configure_) {
            model_configure_->init();
        }

        if (terrain_configure_) {
            terrain_configure_->init();
        }

        createBuffers();
        createStates();

        space_obj_mgr_->notifyGraphicDeviceRestored();
    }

    ukv3d::Camera* TerrainScene::getCamera() const {
        return camera_;
    }

    ukv3d::SpaceObjectManager* TerrainScene::getSpaceObjectManager() const {
        return space_obj_mgr_;
    }

    LodGenerator* TerrainScene::getLodGenerator() const {
        return lod_generator_;
    }

}