// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_EFFECTS_IMAGE_EFFECT_DX_H_
#define UKIVE_GRAPHICS_WIN_EFFECTS_IMAGE_EFFECT_DX_H_

#include <memory>

#include "utils/math/algebra/point.hpp"

#include "ukive/graphics/effects/image_effect.h"
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

    class ImageEffectGPU : public ImageEffect {
    public:
        explicit ImageEffectGPU(Context context);
        ~ImageEffectGPU();

        bool initialize() override;
        void destroy() override;

        bool generate(Canvas* c) override;
        bool draw(Canvas* c) override;
        bool setContent(OffscreenBuffer* content) override;
        bool setContent(const GPtr<GPUTexture>& texture);
        GPtr<ImageFrame> getOutput() const override;

        bool setSize(int width, int height, bool hdr);

    private:
        struct ConstBuffer {
            utl::mat4f wvo;
        };

        struct VertexData {
            utl::pt3f position;

            VertexData() {}
            VertexData(const utl::pt3f& pos)
                : position(pos) {}
        };

        bool createTexture(
            GPtr<GPUTexture>& tex,
            GPtr<GPURenderTarget>& rtv,
            GPtr<GPUShaderResource>& srv);
        bool createKernelTexture(const uint8_t* kernel, size_t len);
        void render();

        int width_;
        int height_;
        int view_width_;
        int view_height_;
        bool is_initialized_ = false;
        bool is_hdr_enabled_ = false;

        utl::mat4f wvo_matrix_;
        utl::mat4f world_matrix_;
        utl::mat4f view_matrix_;
        utl::mat4f ortho_matrix_;

        GPtr<GPUShaderResource> org_srv_;
        GPtr<GPUTexture> kernel_tex2d_;

        GPtr<GPUTexture> target_tex2d_;
        GPtr<GPURenderTarget> target_rtv_;
        GPtr<GPUShaderResource> target_srv_;

        GPtr<GPUBuffer> vert_buffer_;
        GPtr<GPUBuffer> index_buffer_;
        GPtr<GPUBuffer> const_buffer_;
        GPtr<GPUInputLayout> input_layout_;
        GPtr<GPUShader> ps_;
        GPtr<GPUShader> vs_;

        Viewport viewport_;
        GPtr<GPURasterizerState> rasterizer_state_;

        Context context_;
        GPtr<ImageFrame> cache_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_EFFECTS_IMAGE_EFFECT_DX_H_