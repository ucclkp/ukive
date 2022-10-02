// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_LOD_TERRAIN_SCENE_H_
#define SHELL_LOD_TERRAIN_SCENE_H_

#include <functional>

#include "utils/math/algebra/point.hpp"

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gpu/gpu_types.h"
#include "ukive/graphics/3d/scene.h"


namespace ukv3d {
    class Camera;
    class SpaceObjectManager;
}

namespace ukive {
    class GPUBuffer;
    class GPURasterizerState;
    class InputEvent;
    class Space3DView;
    class AssistConfigure;
    class ModelConfigure;
}

namespace shell {

    class LodGenerator;
    class TerrainConfigure;

    class TerrainScene : public ukv3d::Scene {
    public:
        TerrainScene();
        ~TerrainScene();

        void setRenderListener(std::function<void()>&& l);

        void recreate(int level);
        void reevaluate(float c1, float c2);

        void onSceneCreate(ukive::Space3DView* view) override;
        void onSceneResize(int width, int height) override;
        bool onSceneInput(ukive::InputEvent* e) override;
        void onSceneRender(ukive::GPUContext* context, ukive::GPURenderTarget* rt) override;
        void onSceneDestroy() override;

        void onGraphicDeviceLost() override;
        void onGraphicDeviceRestored() override;

        ukv3d::Camera* getCamera() const;
        ukv3d::SpaceObjectManager* getSpaceObjectManager() const;
        LodGenerator* getLodGenerator() const;

    private:
        enum {
            MOUSE_ACTION_NONE = 1,
            MOUSE_ACTION_MOVED = 2,
            MOUSE_ACTION_MOVING = 3,
        };

        void createBuffers();
        void createStates();
        void updateCube();
        void updateLodTerrain();
        void elementAwareness(int ex, int ey);
        void getPickLine(int sx, int sy, utl::pt3f* line_org, utl::vec3f* line_dir);
        void setViewports(float x, float y, float width, float height);

    private:
        int prev_x_;
        int prev_y_;
        int width_;
        int height_;

        int mouse_action_mode_;
        bool is_ctrl_key_pressed_;
        bool is_shift_key_pressed_;
        bool is_mouse_left_key_pressed_;

        LodGenerator* lod_generator_;
        ukive::AssistConfigure* assist_configure_;
        ukive::ModelConfigure* model_configure_;
        TerrainConfigure* terrain_configure_;

        ukive::Viewport viewport_;
        ukive::GPtr<ukive::GPUBuffer> index_buffer_;
        ukive::GPtr<ukive::GPUBuffer> vertex_buffer_;
        ukive::GPtr<ukive::GPURasterizerState> rasterizer_state_;

        ukv3d::Camera* camera_;
        ukive::Space3DView* space_view_;
        ukv3d::SpaceObjectManager* space_obj_mgr_;

        std::function<void()> on_render_handler_;
    };

}

#endif  // SHELL_LOD_TERRAIN_SCENE_H_