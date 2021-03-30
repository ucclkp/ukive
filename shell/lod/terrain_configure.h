// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_LOD_TERRAIN_CONFIGURE_H_
#define SHELL_LOD_TERRAIN_CONFIGURE_H_

#include <memory>

#include "ukive/graphics/3d/matrix.hpp"


namespace ukive {
    class GPUBuffer;
    class GPUShader;
    class GPUContext;
    class GPUInputLayout;
}

namespace shell {

    struct TerrainVertexData {
        ukv3d::Point3F position;
        ukv3d::Vector2F texcoord;

        TerrainVertexData() {}
    };


    class TerrainConfigure {
    public:
        TerrainConfigure();
        ~TerrainConfigure();

        void init();
        void active(ukive::GPUContext* context);
        void close();

        void setMatrix(ukive::GPUContext* context, const ukv3d::Matrix4x4F& matrix);

    private:
        struct MatrixConstBuffer {
            ukv3d::Matrix4x4F wvp;
        };

        std::unique_ptr<ukive::GPUBuffer> const_buffer_;
        std::unique_ptr<ukive::GPUShader> pixel_shader_;
        std::unique_ptr<ukive::GPUShader> vertex_shader_;
        std::unique_ptr<ukive::GPUInputLayout> input_layout_;
    };

}

#endif  // SHELL_LOD_TERRAIN_CONFIGURE_H_