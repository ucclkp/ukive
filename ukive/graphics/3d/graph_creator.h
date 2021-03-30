// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_GRAPH_CREATOR_H_
#define UKIVE_GRAPHICS_3D_GRAPH_CREATOR_H_

#include "ukive/graphics/win/direct3d/assist_configure.h"
#include "ukive/graphics/win/direct3d/model_configure.h"

#include "ukive/graphics/3d/point.hpp"


namespace ukv3d {

    class DrawingObjectManager;

    class GraphCreator {
    public:
       explicit GraphCreator(DrawingObjectManager* mgr);
       ~GraphCreator();

        void calculateNormalVector(
            ukive::ModelVertexData* vertices, int vertexCount, int* indices, int indexCount);

        void putLine(ukv3d::Point3F* point1, ukv3d::Point3F* point2, int tag);
        void putCube(int tag, float edgeLength);
        void putWorldAxis(int tag, float length);
        void putMark(int tag, ukv3d::Point3F* mark, float length);
        void putBlock(int tag, const ukv3d::Point3F& posCenter, float radius);

    private:
        DrawingObjectManager* drawing_obj_mgr_;
    };

}

#endif  // UKIVE_GRAPHICS_3D_GRAPH_CREATOR_H_