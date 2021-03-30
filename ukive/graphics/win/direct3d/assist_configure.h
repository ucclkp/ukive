// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_DIRECT3D_ASSIST_CONFIGURE_H_
#define UKIVE_GRAPHICS_WIN_DIRECT3D_ASSIST_CONFIGURE_H_

#include <memory>

#include "ukive/graphics/3d/matrix.hpp"


namespace ukive {

    class GPUBuffer;
    class GPUShader;
    class GPUContext;
    class GPUInputLayout;

    struct AssistVertexData {
        ukv3d::Point3F position;
        ukv3d::Point4F color;
        ukv3d::Vector2F texcoord;

        AssistVertexData()
            : color(ukv3d::Point4F(0, 0, 0, 1)) {}
    };


    class AssistConfigure {
    public:
        AssistConfigure();
        ~AssistConfigure();

        void init();
        void active(GPUContext* context);
        void close();

        void setMatrix(GPUContext* context, const ukv3d::Matrix4x4F& matrix);

    private:
        struct AssistConstBuffer {
            ukv3d::Matrix4x4F wvp;
        };

        std::unique_ptr<GPUBuffer> assist_cb_;
        std::unique_ptr<GPUShader> assist_vs_;
        std::unique_ptr<GPUShader> assist_ps_;
        std::unique_ptr<GPUInputLayout> assist_il_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_DIRECT3D_ASSIST_CONFIGURE_H_