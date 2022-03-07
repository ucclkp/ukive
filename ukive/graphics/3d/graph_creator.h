// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_GRAPH_CREATOR_H_
#define UKIVE_GRAPHICS_3D_GRAPH_CREATOR_H_

#include "ukive/graphics/win/3d/assist_configure.h"
#include "ukive/graphics/win/3d/model_configure.h"

#include "utils/math/algebra/point.hpp"


namespace ukv3d {

    class DrawingObjectManager;

    class GraphCreator {
    public:
       explicit GraphCreator(DrawingObjectManager* mgr);
       ~GraphCreator();

        void calculateNormalVector(
            ukive::ModelVertexData* vertices, int vertexCount, int* indices, int indexCount);

        void putLine(utl::pt3f* point1, utl::pt3f* point2, int tag);
        void putCube(int tag, float edgeLength);
        void putWorldAxis(int tag, float length);
        void putMark(int tag, utl::pt3f* mark, float length);
        void putBlock(int tag, const utl::pt3f& posCenter, float radius);

    private:
        DrawingObjectManager* drawing_obj_mgr_;
    };

}

#endif  // UKIVE_GRAPHICS_3D_GRAPH_CREATOR_H_