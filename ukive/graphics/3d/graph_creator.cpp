// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "graph_creator.h"

#include "ukive/graphics/3d/drawing_object_manager.h"


namespace ukv3d {

    GraphCreator::GraphCreator(DrawingObjectManager* mgr)
        :drawing_obj_mgr_(mgr) {}

    GraphCreator::~GraphCreator() {}

    void GraphCreator::calculateNormalVector(
        ukive::ModelVertexData* vertices, int vertexCount, int* indices, int indexCount) {

        int triangle_count = indexCount / 3;

        for (int i = 0; i < triangle_count; ++i) {
            int i0 = indices[i * 3 + 0];
            int i1 = indices[i * 3 + 1];
            int i2 = indices[i * 3 + 2];

            auto i0Vec = vertices[i0].position;
            auto e0 = vertices[i1].position - i0Vec;
            auto e1 = vertices[i2].position - i0Vec;
            auto face_normal = e0 ^ e1;

            vertices[i0].normal = vertices[i0].normal + face_normal;
            vertices[i1].normal = vertices[i1].normal + face_normal;
            vertices[i2].normal = vertices[i2].normal + face_normal;
        }

        for (int i = 0; i < vertexCount; ++i) {
            vertices[i].normal.nor();
        }
    }

    void GraphCreator::putWorldAxis(int tag, float length) {
        unsigned int vertex_count = 6;
        unsigned int index_count = 6;

        if (length < 10.0f) {
            length = 10.0f;
        }

        auto vertex_data = new ukive::AssistVertexData[vertex_count];
        vertex_data[0].position.set(-length, 0, 0);
        vertex_data[0].color.set(1, 0, 0, 1);
        vertex_data[1].position.set(length, 0, 0);
        vertex_data[1].color.set(1, 0, 0, 1);

        vertex_data[2].position.set(0, -length, 0);
        vertex_data[2].color.set(0, 1, 0, 1);
        vertex_data[3].position.set(0, length, 0);
        vertex_data[3].color.set(0, 1, 0, 1);

        vertex_data[4].position.set(0, 0, -length);
        vertex_data[4].color.set(0, 0, 1, 1);
        vertex_data[5].position.set(0, 0, length);
        vertex_data[5].color.set(0, 0, 1, 1);

        auto indices = new int[index_count] {
            0, 1, 2, 3, 4, 5
        };

        drawing_obj_mgr_->add(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::AssistVertexData), vertex_count, index_count, tag);
    }

    void GraphCreator::putLine(ukv3d::Point3F* point1, ukv3d::Point3F* point2, int tag) {
        unsigned int vertex_count = 2;
        unsigned int index_count = 2;

        auto vertex_data = new ukive::AssistVertexData[vertex_count];
        vertex_data[0].position = *point1;
        vertex_data[0].color.set(0.5f, 0, 0, 1);
        vertex_data[1].position = *point2;
        vertex_data[1].color.set(0.5f, 0, 0, 1);

        auto indices = new int[index_count] {
            0, 1
        };

        drawing_obj_mgr_->add(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::AssistVertexData), vertex_count, index_count, tag);
    }

    void GraphCreator::putMark(int tag, ukv3d::Point3F* mark, float length) {
        unsigned int vertex_count = 4;
        unsigned int index_count = 4;

        auto vertex_data = new ukive::AssistVertexData[vertex_count];
        vertex_data[0].position.set(mark->x - length, 0, mark->z);
        vertex_data[0].color.set(0.5f, 0, 0, 1);
        vertex_data[1].position.set(mark->x + length, 0, mark->z);
        vertex_data[1].color.set(0.5f, 0, 0, 1);
        vertex_data[2].position.set(mark->x, 0, mark->z - length);
        vertex_data[2].color.set(0.5f, 0, 0, 1);
        vertex_data[3].position.set(mark->x, 0, mark->z + length);
        vertex_data[3].color.set(0.5f, 0, 0, 1);

        auto indices = new int[index_count] {
            0, 1, 2, 3
        };

        drawing_obj_mgr_->add(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::AssistVertexData), vertex_count, index_count, tag);
    }

    void GraphCreator::putBlock(int tag, const ukv3d::Point3F& posCenter, float radius) {
        unsigned int vertex_count = 8;
        unsigned int index_count = 36;

        auto vertex_data = new ukive::AssistVertexData[vertex_count];
        vertex_data[0].position.set(
            posCenter.x - radius, posCenter.y - radius, posCenter.z - radius);
        vertex_data[0].color.set(1, 1, 0, 1);

        vertex_data[1].position.set(
            posCenter.x + radius, posCenter.y - radius, posCenter.z - radius);
        vertex_data[1].color.set(1, 1, 0, 1);

        vertex_data[2].position.set(
            posCenter.x + radius, posCenter.y - radius, posCenter.z + radius);
        vertex_data[2].color.set(1, 1, 0, 1);

        vertex_data[3].position.set(
            posCenter.x - radius, posCenter.y - radius, posCenter.z + radius);
        vertex_data[3].color.set(1, 1, 0, 1);

        vertex_data[4].position.set(
            posCenter.x - radius, posCenter.y + radius, posCenter.z + radius);
        vertex_data[4].color.set(1, 1, 0, 1);

        vertex_data[5].position.set(
            posCenter.x - radius, posCenter.y + radius, posCenter.z - radius);
        vertex_data[5].color.set(1, 1, 0, 1);

        vertex_data[6].position.set(
            posCenter.x + radius, posCenter.y + radius, posCenter.z - radius);
        vertex_data[6].color.set(1, 1, 0, 1);

        vertex_data[7].position.set(
            posCenter.x + radius, posCenter.y + radius, posCenter.z + radius);
        vertex_data[7].color.set(1, 1, 0, 1);

        auto indices = new int[index_count] {
            0, 5, 1,
            1, 5, 6,
            1, 6, 2,
            2, 6, 7,
            2, 7, 3,
            3, 7, 4,
            3, 4, 0,
            0, 4, 5,
            3, 0, 2,
            2, 0, 1,
            5, 4, 6,
            6, 4, 7
        };

        drawing_obj_mgr_->add(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::AssistVertexData), vertex_count, index_count, tag);
    }

    void GraphCreator::putCube(int tag, float edgeLength) {
        unsigned int vertex_count = 8;
        unsigned int index_count = 36;

        float half = edgeLength / 2.f;

        auto vertex_data = new ukive::ModelVertexData[vertex_count];
        vertex_data[0].position.set(-half, -half, -half);
        vertex_data[0].color.set(1, 0, 0, 1);
        vertex_data[0].texcoord.set(0, 1);

        vertex_data[1].position.set(half, -half, -half);
        vertex_data[1].color.set(1, 0, 0, 1);
        vertex_data[1].texcoord.set(1, 1);

        vertex_data[2].position.set(half, -half, half);
        vertex_data[2].color.set(0, 1, 0, 1);
        vertex_data[2].texcoord.set(1, 0);

        vertex_data[3].position.set(-half, -half, half);
        vertex_data[3].color.set(0, 1, 0, 1);
        vertex_data[3].texcoord.set(0, 0);

        vertex_data[4].position.set(-half, half, half);
        vertex_data[4].color.set(0, 0, 1, 1);
        vertex_data[4].texcoord.set(0, 0);

        vertex_data[5].position.set(-half, half, -half);
        vertex_data[5].color.set(0, 0, 1, 1);
        vertex_data[5].texcoord.set(0, 1);

        vertex_data[6].position.set(half, half, -half);
        vertex_data[6].color.set(0, 0, 1, 1);
        vertex_data[6].texcoord.set(1, 1);

        vertex_data[7].position.set(half, half, half);
        vertex_data[7].color.set(0, 0, 1, 1);
        vertex_data[7].texcoord.set(1, 0);

        auto indices = new int[index_count] {
            0, 5, 1,
            1, 5, 6,
            1, 6, 2,
            2, 6, 7,
            2, 7, 3,
            3, 7, 4,
            3, 4, 0,
            0, 4, 5,
            3, 0, 2,
            2, 0, 1,
            5, 4, 6,
            6, 4, 7
        };

        calculateNormalVector(vertex_data, vertex_count, indices, index_count);

        drawing_obj_mgr_->add(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::ModelVertexData), vertex_count, index_count, tag);
    }

}