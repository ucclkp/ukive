// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "terrain_scene.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/event/input_event.h"
#include "ukive/event/keyboard.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/3d/drawing_object_manager.h"
#include "ukive/graphics/3d/camera.h"
#include "ukive/graphics/3d/geocal.hpp"
#include "ukive/graphics/gpu/gpu_rasterizer_state.h"
#include "ukive/views/text_view.h"
#include "ukive/views/space3d_view.h"
#include "ukive/window/window.h"

#include "shell/lod/lod_generator.h"
#include "shell/lod/terrain_configure.h"


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

        drawing_obj_mgr_ = new ukv3d::DrawingObjectManager();
        graph_creator_ = new ukv3d::GraphCreator(drawing_obj_mgr_);
        graph_creator_->putCube(kNormalCube, 100);

        camera_ = new ukv3d::Camera(1, 1);
        lod_generator_ = new LodGenerator(8192, 5);

        createBuffers();
    }

    TerrainScene::~TerrainScene() {
        delete lod_generator_;

        delete drawing_obj_mgr_;
        delete graph_creator_;
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
        vertex_buffer_.reset(device->createBuffer(&vb, &vb_data));

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
        index_buffer_.reset(device->createBuffer(&ib, &ib_data));
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
        rasterizer_state_.reset(device->createRasterizerState(&desc));
    }

    void TerrainScene::updateCube() {
        auto object = getDrawingObjectManager()->getByTag(kNormalCube);
        if (object) {
            auto gpu_context = ukive::Application::getGraphicDeviceManager()->getGPUContext();

            auto data = gpu_context->lock(object->vertex_buffer.get());
            if (data) {
                auto locked_vd = static_cast<ukive::ModelVertexData*>(data);
                auto object_vd = static_cast<ukive::ModelVertexData*>(object->vertices.get());

                object_vd[0].position.x += 0.1f;

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

        auto data = gpu_context->lock(index_buffer_.get());
        if (data) {
            int* indices = static_cast<int*>(data);

            auto vPosition = getCamera()->getCameraPos();

            ukv3d::Matrix4x4F wvpMatrix;
            getCamera()->getWVPMatrix(&wvpMatrix);

            lod_generator_->renderLodTerrain(*vPosition, wvpMatrix, indices);

            gpu_context->unlock(index_buffer_.get());
        }
    }

    void TerrainScene::elementAwareness(int ex, int ey) {
        ukv3d::Point3F ori;
        ukv3d::Vector3F dir;
        getPickLine(ex, ey, &ori, &dir);

        bool isHitVer = false;
        ukv3d::Point3D vPos;
        auto *object = getDrawingObjectManager()->getByTag(kNormalCube);
        if (object) {
            auto vData = static_cast<ukive::ModelVertexData*>(object->vertices.get());
            for (unsigned int i = 0; i < object->vertex_count; ++i) {
                auto pos = ukv3d::Point3D(vData[i].position);
                auto distance = ukv3d::distanceLinePoint(
                    ukv3d::Point3D(ori), ukv3d::Point3D(ori + dir), pos);
                if (distance < 20.f) {
                    vPos = pos;
                    isHitVer = true;
                }
            }

            if (isHitVer) {
                if (!drawing_obj_mgr_->contains(155)) {
                    graph_creator_->putBlock(155, ukv3d::Point3F(vPos), 10.f);
                    space_view_->requestDraw();
                }
            } else {
                if (getDrawingObjectManager()->contains(155)) {
                    getDrawingObjectManager()->removeByTag(155);
                    space_view_->requestDraw();
                }
            }
        }
    }

    void TerrainScene::getPickLine(
        int sx, int sy, ukv3d::Point3F* line_org, ukv3d::Vector3F* line_dir)
    {
        auto worldMatrix = camera_->getWorldMatrix();
        auto viewMatrix = camera_->getViewMatrix();
        auto projectionMatrix = camera_->getProjectionMatrix();

        auto vx = (2.0f * sx / width_ - 1.0f) / projectionMatrix->m11;
        auto vy = (-2.0f * sy / height_ + 1.0f) / projectionMatrix->m22;

        ukv3d::Vector4D rayDir(vx, vy, 1.0f, 0);
        ukv3d::Vector4D rayOrigin(0.0f, 0.0f, 0.0f, 1);

        auto world = ukv3d::Matrix4x4D(*worldMatrix);
        auto view = ukv3d::Matrix4x4D(*viewMatrix);

        ukv3d::Matrix4x4D inverseView;
        view.inverse(&inverseView);

        rayDir = (rayDir * inverseView).nor();
        rayOrigin = rayOrigin * inverseView;

        ukv3d::Matrix4x4D inverseWorld;
        world.inverse(&inverseWorld);

        rayDir = (rayDir * inverseWorld).nor();
        rayOrigin = rayOrigin * inverseWorld;

        line_org->set(rayOrigin.x, rayOrigin.y, rayOrigin.z);
        *line_dir = ukv3d::Vector3F(rayDir.v3());
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

        getGraphCreator()->putWorldAxis(kWorldAxis, 1024);

        createStates();
        updateLodTerrain();
    }

    void TerrainScene::onSceneResize(int width, int height) {
        Scene::onSceneResize(width, height);

        width_ = width;
        height_ = height;

        setViewports(0, 0, width, height);
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

        ukv3d::Matrix4x4F wvp_matrix;
        ukv3d::Matrix4x4F wvp_matrix_t;

        context->setRasterizerState(rasterizer_state_.get());
        context->setViewport(1, &viewport_);

        getCamera()->getWVPMatrix(&wvp_matrix);
        wvp_matrix.transport(&wvp_matrix_t);

        assist_configure_->active(context);
        assist_configure_->setMatrix(context, wvp_matrix_t);

        context->setPrimitiveTopology(ukive::GPUContext::Topology::LineList);
        getDrawingObjectManager()->draw(kWorldAxis);

        terrain_configure_->active(context);
        terrain_configure_->setMatrix(context, wvp_matrix_t);

        context->setPrimitiveTopology(ukive::GPUContext::Topology::TriangleList);

        unsigned int vertexDataOffset = 0;
        unsigned int vertexStructSize = sizeof(TerrainVertexData);
        context->setVertexBuffers(
            0, 1, vertex_buffer_.get(), &vertexStructSize, &vertexDataOffset);
        context->setIndexBuffer(index_buffer_.get(), ukive::GPUDataFormat::R32_UINT, 0);
        context->drawIndexed(lod_generator_->getIndexCount(), 0, 0);

        context->setPrimitiveTopology(ukive::GPUContext::Topology::TriangleList);
        drawing_obj_mgr_->draw(155);

        model_configure_->active(context);
        model_configure_->setMatrix(context, wvp_matrix_t);

        context->setPrimitiveTopology(ukive::GPUContext::Topology::TriangleList);
        drawing_obj_mgr_->draw(kNormalCube);
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

        drawing_obj_mgr_->notifyGraphicDeviceLost();
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

        drawing_obj_mgr_->notifyGraphicDeviceRestored();
    }

    ukv3d::Camera* TerrainScene::getCamera() const {
        return camera_;
    }

    ukv3d::GraphCreator* TerrainScene::getGraphCreator() const {
        return graph_creator_;
    }

    ukv3d::DrawingObjectManager* TerrainScene::getDrawingObjectManager() const {
        return drawing_obj_mgr_;
    }

    LodGenerator* TerrainScene::getLodGenerator() const {
        return lod_generator_;
    }

}