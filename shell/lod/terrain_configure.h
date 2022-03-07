// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_LOD_TERRAIN_CONFIGURE_H_
#define SHELL_LOD_TERRAIN_CONFIGURE_H_

#include <memory>

#include "utils/math/algebra/point.hpp"

#include "ukive/graphics/gptr.hpp"


namespace ukive {
    class GPUBuffer;
    class GPUShader;
    class GPUContext;
    class GPUInputLayout;
}

namespace shell {

    struct TerrainVertexData {
        utl::pt3f position;
        utl::vec2f texcoord;

        TerrainVertexData() {}
    };


    class TerrainConfigure {
    public:
        TerrainConfigure();
        ~TerrainConfigure();

        void init();
        void active(ukive::GPUContext* context);
        void close();

        void setMatrix(ukive::GPUContext* context, const utl::mat4f& matrix);

    private:
        struct MatrixConstBuffer {
            utl::mat4f wvp;
        };

        ukive::GPtr<ukive::GPUBuffer> const_buffer_;
        ukive::GPtr<ukive::GPUShader> pixel_shader_;
        ukive::GPtr<ukive::GPUShader> vertex_shader_;
        ukive::GPtr<ukive::GPUInputLayout> input_layout_;
    };

}

#endif  // SHELL_LOD_TERRAIN_CONFIGURE_H_