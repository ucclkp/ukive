// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_EFFECTS_IMAGE_EFFECT_DX_H_
#define UKIVE_GRAPHICS_WIN_EFFECTS_IMAGE_EFFECT_DX_H_

#include <map>
#include <memory>
#include <string>

#include "utils/math/algebra/point.hpp"

#include "ukive/graphics/effects/image_effect.h"
#include "ukive/graphics/gpu/gpu_shader.h"
#include "ukive/graphics/gpu/gpu_types.h"


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
        ImageEffectGPU();
        ~ImageEffectGPU();

        bool initialize() override;
        void destroy() override;

        bool generate(Canvas* c) override;
        bool draw(Canvas* c) override;
        bool addInput(OffscreenBuffer* content) override;
        bool addInput(const GPtr<GPUTexture>& texture) override;
        void clearInputs() override;
        bool setOutputFormat(
            unsigned int width,
            unsigned int height,
            GPUDataFormat format) override;
        GPtr<ImageFrame> getOutput() const override;
        GPtr<GPUTexture> getOutputTexture() const override;
        bool setPixelShader(const std::u16string& name) override;
        bool setParameterSize(uint32_t size) override;
        void setParameterUpdateHandler(const ParameterUpdateHandler& h) override;
        bool updateParameters() override;

        bool setSize(int width, int height, GPUDataFormat format);
        bool setVertexShader(const std::u16string& name);

    private:
        struct ConstBuffer {
            utl::mat4f wvo;
        };

        struct VertexData {
            utl::pt3f position;
            utl::vec2f texcoord;

            VertexData() {}
            VertexData(const utl::pt3f& pos, const utl::vec2f& uv)
                : position(pos), texcoord(uv) {}
        };

        bool createTexture(
            GPUDataFormat format,
            GPtr<GPUTexture>& tex,
            GPtr<GPURenderTarget>& rtv,
            GPtr<GPUShaderResource>& srv);
        void render();

        int width_;
        int height_;
        bool is_initialized_ = false;
        GPUDataFormat format_ = GPUDataFormat::UNKNOWN;
        ParameterUpdateHandler param_update_handler_;

        utl::mat4f wvo_matrix_;
        utl::mat4f world_matrix_;
        utl::mat4f view_matrix_;
        utl::mat4f ortho_matrix_;

        std::vector<GPtr<GPUShaderResource>> org_srvs_;

        GPtr<GPUTexture> target_tex2d_;
        GPtr<GPURenderTarget> target_rtv_;
        GPtr<GPUShaderResource> target_srv_;

        GPtr<GPUBuffer> vert_buffer_;
        GPtr<GPUBuffer> index_buffer_;
        GPtr<GPUBuffer> vconst_buffer_;
        GPtr<GPUBuffer> pconst_buffer_;
        GPtr<GPUInputLayout> input_layout_;
        GPtr<GPUShader> ps_;
        GPtr<GPUShader> vs_;
        std::map<std::u16string, GPtr<GPUShader>> cached_shaders_;

        Viewport viewport_;
        GPtr<GPURasterizerState> rasterizer_state_;

        GPtr<ImageFrame> cache_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_EFFECTS_IMAGE_EFFECT_DX_H_