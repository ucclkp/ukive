// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_EFFECTS_SHADOW_EFFECT_DX_H_
#define UKIVE_GRAPHICS_WIN_EFFECTS_SHADOW_EFFECT_DX_H_

#include <memory>

#include "utils/math/algebra/point.hpp"

#include "ukive/graphics/effects/shadow_effect.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gpu/gpu_shader.h"
#include "ukive/graphics/gpu/gpu_types.h"
#include "ukive/window/context.h"


namespace ukive {

    class Canvas;
    class GPUBuffer;
    class GPUInputLayout;
    class GPUTexture;
    class GPURenderTarget;
    class GPUShaderResource;
    class GPURasterizerState;

namespace win {

    class ShadowEffectGPU : public ShadowEffect {
    public:
        explicit ShadowEffectGPU(Context context);
        ~ShadowEffectGPU();

        bool initialize() override;
        void destroy() override;

        bool generate(Canvas* c) override;
        bool draw(Canvas* c) override;
        bool setContent(OffscreenBuffer* content) override;
        GPtr<ImageFrame> getOutput() const override;

        void resetCache() override;
        bool hasCache() const override;

        bool setRadius(int radius) override;
        int getRadius() const override;

    private:
        struct ConstBuffer {
            utl::mat4f wvo;
        };

        struct VertexData {
            utl::pt3f position;
        };

        bool createTexture(
            GPtr<GPUTexture>& tex,
            GPtr<GPURenderTarget>& rtv,
            GPtr<GPUShaderResource>& srv);
        bool createKernelTexture();
        bool setSize(int width, int height, bool hdr);
        void render();

        int width_;
        int height_;
        int view_width_;
        int view_height_;
        int radius_;
        float semi_radius_;
        bool is_initialized_ = false;
        bool is_hdr_enabled_ = false;

        utl::mat4f wvo_matrix_;
        utl::mat4f world_matrix_;
        utl::mat4f view_matrix_;
        utl::mat4f ortho_matrix_;

        GPtr<GPURenderTarget> bg_rtv_;
        GPtr<GPUShaderResource> bg_srv_;

        GPtr<GPUTexture> kernel_tex2d_;
        GPtr<GPUShaderResource> kernel_srv_;

        GPtr<GPUTexture> shadow1_tex2d_;
        GPtr<GPURenderTarget> shadow1_rtv_;
        GPtr<GPUShaderResource> shadow1_srv_;

        GPtr<GPUTexture> shadow2_tex2d_;
        GPtr<GPURenderTarget> shadow2_rtv_;
        GPtr<GPUShaderResource> shadow2_srv_;

        GPtr<GPUBuffer> vert_buffer_;
        GPtr<GPUBuffer> index_buffer_;
        GPtr<GPUBuffer> const_buffer_;
        GPtr<GPUInputLayout> input_layout_;
        GPtr<GPUShader> vert_ps_;
        GPtr<GPUShader> hori_ps_;
        GPtr<GPUShader> vs_;

        Viewport viewport_;
        GPtr<GPURasterizerState> rasterizer_state_;

        Context context_;
        GPtr<ImageFrame> cache_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_EFFECTS_SHADOW_EFFECT_DX_H_