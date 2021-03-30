// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_VISUALIZE_VISUAL_LAYOUT_SCENE_H_
#define SHELL_VISUALIZE_VISUAL_LAYOUT_SCENE_H_

#include <memory>

#include "ukive/graphics/gpu/gpu_types.h"
#include "ukive/graphics/3d/scene.h"
#include "ukive/graphics/win/direct3d/assist_configure.h"
#include "ukive/graphics/win/direct3d/model_configure.h"


namespace ukv3d {
    class Camera;
    class DrawingObjectManager;
    class GraphCreator;
}

namespace ukive {
    class InputEvent;
    class Space3DView;
    class GPUShader;
    class GPUBuffer;
    class GPUTexture;
    class GPUInputLayout;
    class GPURasterizerState;
    class GPUDepthStencilState;
    class GPUDepthStencil;
    class GPUShaderResource;
    class GPUSamplerState;
}

namespace vsul {

    class VisualLayoutScene : public ukv3d::Scene {
    public:
        VisualLayoutScene();
        ~VisualLayoutScene();

        // ukive::Scene
        void onSceneCreate(ukive::Space3DView* d3d_view) override;
        void onSceneResize(int width, int height) override;
        void onSceneRender(ukive::GPUContext* context, ukive::GPURenderTarget* rt) override;
        void onSceneDestroy() override;
        void onGraphicDeviceLost() override;
        void onGraphicDeviceRestored() override;

    private:
        bool createStates();
        void releaseStates();
        bool createResources(int width, int height);
        void releaseResources();
        void setViewports(float x, float y, float width, float height);

        ukv3d::Camera* camera_ = nullptr;
        ukv3d::GraphCreator* graph_creator_ = nullptr;
        ukv3d::DrawingObjectManager* drawing_obj_mgr_ = nullptr;
        ukive::AssistConfigure assist_configure_;
        ukive::ModelConfigure model_configure_;

        ukive::Viewport viewport_;
        std::unique_ptr<ukive::GPUTexture> depth_stencil_buffer_;
        std::unique_ptr<ukive::GPUDepthStencil> depth_stencil_view_;
        std::unique_ptr<ukive::GPUShaderResource> shader_resource_view_;
        std::unique_ptr<ukive::GPUDepthStencilState> depth_stencil_state_;
        std::unique_ptr<ukive::GPURasterizerState> rasterizer_state_;
        std::unique_ptr<ukive::GPUSamplerState> sampler_state_;
    };

}

#endif  // SHELL_VISUALIZE_VISUAL_LAYOUT_SCENE_H_