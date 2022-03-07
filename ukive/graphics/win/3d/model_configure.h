// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_3D_MODEL_CONFIGURE_H_
#define UKIVE_GRAPHICS_WIN_3D_MODEL_CONFIGURE_H_

#include <memory>

#include "utils/math/algebra/point.hpp"

#include "ukive/graphics/gptr.hpp"


namespace ukive {

    class GPUBuffer;
    class GPUShader;
    class GPUContext;
    class GPUInputLayout;

    struct ModelVertexData {
        utl::pt3f position;
        utl::pt4f color;
        utl::vec3f normal;
        utl::vec2f texcoord;

        ModelVertexData()
            : color({ 0, 0, 0, 1 }) {}
    };


    class ModelConfigure {
    public:
        ModelConfigure();
        ~ModelConfigure();

        void init();
        void active(GPUContext* context);
        void close();

        void setMatrix(GPUContext* context, const utl::mat4f& matrix);

    private:
        struct MatrixConstBuffer {
            utl::mat4f wvp;
        };

        struct PhongLightConstBuffer {
            utl::mat4f wvp;
        };

        GPtr<GPUBuffer> panel_cb_;
        GPtr<GPUShader> panel_vs_;
        GPtr<GPUShader> panel_ps_;
        GPtr<GPUInputLayout> panel_il_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_3D_MODEL_CONFIGURE_H_