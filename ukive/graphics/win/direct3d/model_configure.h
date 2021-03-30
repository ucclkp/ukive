// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_DIRECT3D_MODEL_CONFIGURE_H_
#define UKIVE_GRAPHICS_WIN_DIRECT3D_MODEL_CONFIGURE_H_

#include <memory>

#include "ukive/graphics/3d/matrix.hpp"


namespace ukive {

    class GPUBuffer;
    class GPUShader;
    class GPUContext;
    class GPUInputLayout;

    struct ModelVertexData {
        ukv3d::Point3F position;
        ukv3d::Point4F color;
        ukv3d::Vector3F normal;
        ukv3d::Vector2F texcoord;

        ModelVertexData()
            : color(ukv3d::Point4F(0, 0, 0, 1)) {}
    };


    class ModelConfigure {
    public:
        ModelConfigure();
        ~ModelConfigure();

        void init();
        void active(GPUContext* context);
        void close();

        void setMatrix(GPUContext* context, const ukv3d::Matrix4x4F& matrix);

    private:
        struct MatrixConstBuffer {
            ukv3d::Matrix4x4F wvp;
        };

        struct PhongLightConstBuffer {
            ukv3d::Matrix4x4F wvp;
        };

        std::unique_ptr<GPUBuffer> panel_cb_;
        std::unique_ptr<GPUShader> panel_vs_;
        std::unique_ptr<GPUShader> panel_ps_;
        std::unique_ptr<GPUInputLayout> panel_il_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_DIRECT3D_MODEL_CONFIGURE_H_