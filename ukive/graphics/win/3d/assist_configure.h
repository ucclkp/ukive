// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_3D_ASSIST_CONFIGURE_H_
#define UKIVE_GRAPHICS_WIN_3D_ASSIST_CONFIGURE_H_

#include "utils/math/algebra/point.hpp"

#include "ukive/graphics/gptr.hpp"


namespace ukive {

    class GPUBuffer;
    class GPUShader;
    class GPUContext;
    class GPUInputLayout;

    struct AssistVertexData {
        utl::pt3f position;
        utl::pt4f color;
        utl::vec2f texcoord;

        AssistVertexData()
            : color({ 0, 0, 0, 1 }) {}
    };


    class AssistConfigure {
    public:
        AssistConfigure();
        ~AssistConfigure();

        void init();
        void active(GPUContext* context);
        void close();

        void setMatrix(GPUContext* context, const utl::mat4f& matrix);

    private:
        struct AssistConstBuffer {
            utl::mat4f wvp;
        };

        GPtr<GPUBuffer> assist_cb_;
        GPtr<GPUShader> assist_vs_;
        GPtr<GPUShader> assist_ps_;
        GPtr<GPUInputLayout> assist_il_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_3D_ASSIST_CONFIGURE_H_