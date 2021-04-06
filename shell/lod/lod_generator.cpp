// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "lod_generator.h"

#include <cmath>
#include <fstream>

#include "utils/log.h"
#include "utils/number.hpp"
#include "utils/files/file_utils.h"

#include "ukive/app/application.h"
#include "ukive/resources/resource_manager.h"

#include "shell/lod/qtree_node.h"
#include "shell/lod/terrain_configure.h"


namespace shell {

    LodGenerator::LodGenerator(float edgeLength, int maxLevel) {
        DCHECK(edgeLength > 0 && maxLevel >= 1);

        coe_rough_ = 2.f;
        coe_distance_ = 30.f;

        max_level_ = maxLevel;
        row_vertex_count_ = int(std::pow(2, max_level_)) + 1;
        vertex_count_ = row_vertex_count_ * row_vertex_count_;

        flags_ = new char[vertex_count_];
        std::memset(flags_, 0, vertex_count_);

        auto res_mgr = ukive::Application::getResourceManager();
        auto altitude_file_name = res_mgr->getResRootPath() / u"altitude.raw";

        std::ifstream reader(altitude_file_name, std::ios::binary);
        if (reader.fail()) {
           LOG(Log::FATAL) << "LodGenerator-Constructor(): read file failed.";
           return;
        }
        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        auto charSize = utl::num_cast<size_t>(std::streamoff(reader.tellg()));
        reader.seekg(cpos);

        altitude_ = new char[charSize];
        reader.read(altitude_, charSize);

        vertices_ = new TerrainVertexData[vertex_count_];
        for (int i = 0; i < vertex_count_; ++i) {
            int row = i / row_vertex_count_;
            int column = i % row_vertex_count_;

            int altitudeRow = int(ALTITUDE_MAP_SIZE / float(row_vertex_count_)*row);
            int altitudeColumn = int(ALTITUDE_MAP_SIZE / float(row_vertex_count_)*column);

            int altitude = altitude_[
                (ALTITUDE_MAP_SIZE - 1 - altitudeRow) * ALTITUDE_MAP_SIZE + altitudeColumn];
            if (altitude < 0)
                altitude += 255;

            vertices_[i].position = ukv3d::Point3F(
                edgeLength*column / (row_vertex_count_ - 1),
                float(altitude) * 2, edgeLength - edgeLength*row / (row_vertex_count_ - 1));
        }

        index_count_ = 0;
        max_index_count_ = (row_vertex_count_ - 1)*(row_vertex_count_ - 1) * 2 * 3;

        indices_ = new int[max_index_count_];

        generateQuadTree();
        determineRoughAndBound(root_queue_);
    }

    LodGenerator::~LodGenerator() {
        if (root_queue_) {
            QTreeNode* curQueue = root_queue_;
            QTreeNode* nextQueue = nullptr;

            do {
                while (curQueue) {
                    QTreeNode* node = curQueue;
                    curQueue = curQueue->next;

                    for (int i = 0; i < 4; ++i) {
                        if (node->child[i]) {
                            enqueue(nextQueue, node->child[i]);
                        }
                    }

                    delete node;
                }

                curQueue = nextQueue;
                nextQueue = nullptr;
            } while (curQueue);
        }

        delete[] flags_;
        delete[] indices_;
        delete[] vertices_;
        delete[] altitude_;
    }


    inline void LodGenerator::enqueue(QTreeNode*& queue, QTreeNode* node) {
        node->next = queue;
        queue = node;
    }

    inline QTreeNode* LodGenerator::dequeue(QTreeNode*& queue) {
        if (queue) {
            QTreeNode* node = queue;
            queue = queue->next;
            return node;
        }

        return nullptr;
    }

    inline void LodGenerator::clearQueue(QTreeNode*& queue) {
        while (queue) {
            QTreeNode* node = queue;
            queue = queue->next;
            delete node;
        }
    }


    inline int LodGenerator::calInnerStep(QTreeNode* node) {
        return int((row_vertex_count_ - 1) / std::pow(2, node->level + 1));
    }

    inline int LodGenerator::calNeighborStep(QTreeNode* node) {
        return int((row_vertex_count_ - 1) / std::pow(2, node->level));
    }

    inline int LodGenerator::calChildStep(QTreeNode* node) {
        return int((row_vertex_count_ - 1) / std::pow(2, node->level + 2));
    }


    void LodGenerator::generateQuadTree() {
        int level = 0;
        QTreeNode* curQueue = nullptr;
        QTreeNode* nextQueue = nullptr;

        root_queue_ = new QTreeNode();
        generateRootNodeData(root_queue_);

        curQueue = root_queue_;

        while (level < max_level_ - 1) {
            QTreeNode* iterator = curQueue;
            while (iterator) {
                QTreeNode* parent = iterator;
                iterator = iterator->next;

                for (int i = 0; i < 4; ++i) {
                    QTreeNode* child = new QTreeNode();
                    generateChildNodeData(parent, child, level + 1, i);

                    parent->child[i] = child;
                    enqueue(nextQueue, child);
                }
            }

            curQueue = nextQueue;
            nextQueue = nullptr;

            ++level;
        }
    }

    void LodGenerator::generateRootNodeData(QTreeNode* root) {
        root->level = 0;
        root->indexX = (row_vertex_count_ - 1) / 2;
        root->indexY = (row_vertex_count_ - 1) / 2;
    }

    void LodGenerator::generateChildNodeData(
        QTreeNode* parent, QTreeNode* child, int level, int position)
    {
        child->level = level;

        int pIndexX = parent->indexX;
        int pIndexY = parent->indexY;

        int step = calChildStep(parent);

        switch (position)
        {
        case 0:
            child->indexX = pIndexX - step;
            child->indexY = pIndexY - step;
            break;
        case 1:
            child->indexX = pIndexX + step;
            child->indexY = pIndexY - step;
            break;
        case 2:
            child->indexX = pIndexX - step;
            child->indexY = pIndexY + step;
            break;
        case 3:
            child->indexX = pIndexX + step;
            child->indexY = pIndexY + step;
            break;
        }
    }


    void LodGenerator::determineRoughAndBound(QTreeNode* node)
    {
        //使用stack和while循环代替递归，防止调用栈溢出。

        //用于保存递归上下文的结构体。
        struct Snapshot
        {
            QTreeNode* node;

            float d0, d1, d2, d3, d4;
            float d5, d6, d7, d8;
            float nodeSize;
            float maxD;
            ukv3d::Point3F minC;
            ukv3d::Point3F maxC;

            int stage;
            Snapshot* behind;
        };

        //栈顶指针。
        Snapshot* recursionStack = nullptr;

        Snapshot* current = new Snapshot();
        current->node = node;
        current->stage = 0;
        current->behind = nullptr;

        recursionStack = current;

        //返回值
        float diff;
        ukv3d::Point3F mincoord;
        ukv3d::Point3F maxcoord;

        while (recursionStack)
        {
            //栈pop.
            current = recursionStack;
            recursionStack = recursionStack->behind;

            switch (current->stage)
            {
            case 0:  //在第一次递归之前。
            {
                int innerStep = calInnerStep(current->node);
                TerrainVertexData *vData[9];

                vData[0] = &vertices_[
                    (current->node->indexY - innerStep)*row_vertex_count_ + current->node->indexX - innerStep];
                vData[1] = &vertices_[
                    (current->node->indexY - innerStep)*row_vertex_count_ + current->node->indexX];
                vData[2] = &vertices_[
                    (current->node->indexY - innerStep)*row_vertex_count_ + current->node->indexX + innerStep];
                vData[3] = &vertices_[
                    current->node->indexY*row_vertex_count_ + current->node->indexX - innerStep];
                vData[4] = &vertices_[
                    current->node->indexY*row_vertex_count_ + current->node->indexX];
                vData[5] = &vertices_[
                    current->node->indexY*row_vertex_count_ + current->node->indexX + innerStep];
                vData[6] = &vertices_[
                    (current->node->indexY + innerStep)*row_vertex_count_ + current->node->indexX - innerStep];
                vData[7] = &vertices_[
                    (current->node->indexY + innerStep)*row_vertex_count_ + current->node->indexX];
                vData[8] = &vertices_[
                    (current->node->indexY + innerStep)*row_vertex_count_ + current->node->indexX + innerStep];

                float minX, minY, minZ;
                float maxX, maxY, maxZ;
                for (int i = 0; i < 9; ++i)
                {
                    float x = vData[i]->position.x;
                    float y = vData[i]->position.y;
                    float z = vData[i]->position.z;

                    if (i == 0) {
                        minX = maxX = x;
                        minY = maxY = y;
                        minZ = maxZ = z;
                    } else {
                        if (x < minX) minX = x;
                        if (x > maxX) maxX = x;

                        if (y < minY) minY = y;
                        if (y > maxY) maxY = y;

                        if (z < minZ) minZ = z;
                        if (z > maxZ) maxZ = z;
                    }
                }

                current->minC = ukv3d::Point3F(minX, minY, minZ);
                current->maxC = ukv3d::Point3F(maxX, maxY, maxZ);

                float topHor = (vData[0]->position.y + vData[2]->position.y) / 2.f;
                float rightHor = (vData[2]->position.y + vData[8]->position.y) / 2.f;
                float bottomHor = (vData[8]->position.y + vData[6]->position.y) / 2.f;
                float leftHor = (vData[6]->position.y + vData[0]->position.y) / 2.f;

                current->d0 = std::abs(vData[1]->position.y - topHor);
                current->d1 = std::abs(vData[5]->position.y - rightHor);
                current->d2 = std::abs(vData[7]->position.y - bottomHor);
                current->d3 = std::abs(vData[3]->position.y - leftHor);
                current->d4 = std::abs(vData[4]->position.y - (topHor + rightHor + bottomHor + leftHor) / 4.f);

                current->nodeSize = (
                    vData[2]->position - vData[0]->position).length();

                if (current->node->child[0]) {
                    current->stage = 1;
                    current->behind = recursionStack;
                    recursionStack = current;

                    //递归，第一个子节点。
                    Snapshot* next = new Snapshot();
                    next->node = current->node->child[0];
                    next->stage = 0;
                    next->behind = recursionStack;
                    recursionStack = next;
                } else {
                    current->maxD = current->d0;

                    if (current->d1 > current->maxD) current->maxD = current->d1;
                    if (current->d2 > current->maxD) current->maxD = current->d2;
                    if (current->d3 > current->maxD) current->maxD = current->d3;
                    if (current->d4 > current->maxD) current->maxD = current->d4;

                    current->stage = 4;
                    current->behind = recursionStack;
                    recursionStack = current;
                }

                break;
            }

            case 1:  //第一次调用返回后。
            {
                current->d5 = diff;

                if (mincoord.x < current->minC.x) current->minC.x = mincoord.x;
                if (mincoord.y < current->minC.y) current->minC.y = mincoord.y;
                if (mincoord.z < current->minC.z) current->minC.z = mincoord.z;

                if (maxcoord.x > current->maxC.x) current->maxC.x = maxcoord.x;
                if (maxcoord.y > current->maxC.y) current->maxC.y = maxcoord.y;
                if (maxcoord.z > current->maxC.z) current->maxC.z = maxcoord.z;

                current->stage = 2;
                current->behind = recursionStack;
                recursionStack = current;

                //递归，第二个子节点。
                Snapshot* next = new Snapshot();
                next->node = current->node->child[1];
                next->stage = 0;
                next->behind = recursionStack;
                recursionStack = next;

                break;
            }

            case 2:  //第二次调用返回后。
            {
                current->d6 = diff;

                if (mincoord.x < current->minC.x) current->minC.x = mincoord.x;
                if (mincoord.y < current->minC.y) current->minC.y = mincoord.y;
                if (mincoord.z < current->minC.z) current->minC.z = mincoord.z;

                if (maxcoord.x > current->maxC.x) current->maxC.x = maxcoord.x;
                if (maxcoord.y > current->maxC.y) current->maxC.y = maxcoord.y;
                if (maxcoord.z > current->maxC.z) current->maxC.z = maxcoord.z;

                current->stage = 3;
                current->behind = recursionStack;
                recursionStack = current;

                //递归，第三个子节点。
                Snapshot* next = new Snapshot();
                next->node = current->node->child[2];
                next->stage = 0;
                next->behind = recursionStack;
                recursionStack = next;

                break;
            }

            case 3:  //第三次调用返回后。
            {
                current->d7 = diff;

                if (mincoord.x < current->minC.x) current->minC.x = mincoord.x;
                if (mincoord.y < current->minC.y) current->minC.y = mincoord.y;
                if (mincoord.z < current->minC.z) current->minC.z = mincoord.z;

                if (maxcoord.x > current->maxC.x) current->maxC.x = maxcoord.x;
                if (maxcoord.y > current->maxC.y) current->maxC.y = maxcoord.y;
                if (maxcoord.z > current->maxC.z) current->maxC.z = maxcoord.z;

                current->stage = 4;
                current->behind = recursionStack;
                recursionStack = current;

                //递归，第四个子节点。
                Snapshot *next = new Snapshot();
                next->node = current->node->child[3];
                next->stage = 0;
                next->behind = recursionStack;
                recursionStack = next;

                break;
            }

            case 4:   //第四次调用返回后。
            {
                if (current->node->child[0]) {
                    current->d8 = diff;
                    current->maxD = current->d0;

                    if (mincoord.x < current->minC.x) current->minC.x = mincoord.x;
                    if (mincoord.y < current->minC.y) current->minC.y = mincoord.y;
                    if (mincoord.z < current->minC.z) current->minC.z = mincoord.z;

                    if (maxcoord.x > current->maxC.x) current->maxC.x = maxcoord.x;
                    if (maxcoord.y > current->maxC.y) current->maxC.y = maxcoord.y;
                    if (maxcoord.z > current->maxC.z) current->maxC.z = maxcoord.z;

                    if (current->d1 > current->maxD) current->maxD = current->d1;
                    if (current->d2 > current->maxD) current->maxD = current->d2;
                    if (current->d3 > current->maxD) current->maxD = current->d3;
                    if (current->d4 > current->maxD) current->maxD = current->d4;
                    if (current->d5 > current->maxD) current->maxD = current->d5;
                    if (current->d6 > current->maxD) current->maxD = current->d6;
                    if (current->d7 > current->maxD) current->maxD = current->d7;
                    if (current->d8 > current->maxD) current->maxD = current->d8;
                }

                diff = current->maxD;
                mincoord = current->minC;
                maxcoord = current->maxC;

                current->node->rough = current->maxD / current->nodeSize;
                current->node->mincoord = current->minC;
                current->node->maxcoord = current->maxC;

                delete current;
                break;
            }
            }
        }
    }


    bool LodGenerator::checkNodeCanDivide(QTreeNode* node)
    {
        int step = calNeighborStep(node);

        bool leftAdjacent = false;
        bool topAdjacent = false;
        bool rightAdjacent = false;
        bool bottomAdjacent = false;

        if (node->indexX - step < 0
            || flags_[node->indexY*row_vertex_count_ + node->indexX - step] != 0) {
            leftAdjacent = true;
        }

        if (node->indexY - step < 0
            || flags_[(node->indexY - step)*row_vertex_count_ + node->indexX] != 0) {
            topAdjacent = true;
        }

        if (node->indexX + step > row_vertex_count_ - 1
            || flags_[node->indexY*row_vertex_count_ + node->indexX + step] != 0) {
            rightAdjacent = true;
        }

        if (node->indexY + step > row_vertex_count_ - 1
            || flags_[(node->indexY + step)*row_vertex_count_ + node->indexX] != 0) {
            bottomAdjacent = true;
        }

        return (leftAdjacent && topAdjacent && rightAdjacent && bottomAdjacent);
    }

    bool LodGenerator::assessNodeCanDivide(
        QTreeNode* node, ukv3d::Point3F viewPosition)
    {
        int innerStep = calInnerStep(node);

        int centerIndex = node->indexY*row_vertex_count_ + node->indexX;
        int leftTopIndex = (node->indexY - innerStep)*row_vertex_count_ + node->indexX - innerStep;
        int rightTopIndex = (node->indexY - innerStep)*row_vertex_count_ + node->indexX + innerStep;

        auto nCenter = vertices_[centerIndex].position;

        float distance = (
            nCenter - viewPosition).length();

        float nodeSize = (
            vertices_[rightTopIndex].position - vertices_[leftTopIndex].position).length();

        return (distance / (nodeSize * node->rough * coe_distance_ * coe_rough_)) < 1.f;
    }


    void LodGenerator::drawNode(QTreeNode* node, int* indexBuffer)
    {
        int step = calNeighborStep(node);

        if (!indexBuffer) {
            indexBuffer = indices_;
        }

        bool skipLeft = false;
        bool skipTop = false;
        bool skipRight = false;
        bool skipBottom = false;

        if (node->indexX - step < 0
            || flags_[node->indexY*row_vertex_count_ + node->indexX - step] == 0) {
            skipLeft = true;
        }

        if (node->indexY - step < 0
            || flags_[(node->indexY - step)*row_vertex_count_ + node->indexX] == 0) {
            skipTop = true;
        }

        if (node->indexX + step > row_vertex_count_ - 1
            || flags_[node->indexY*row_vertex_count_ + node->indexX + step] == 0) {
            skipRight = true;
        }

        if (node->indexY + step > row_vertex_count_ - 1
            || flags_[(node->indexY + step)*row_vertex_count_ + node->indexX] == 0) {
            skipBottom = true;
        }

        int nodeStep = calInnerStep(node);

        int centerIndex = node->indexY*row_vertex_count_ + node->indexX;
        int leftTopIndex = (node->indexY - nodeStep)*row_vertex_count_ + node->indexX - nodeStep;
        int rightTopIndex = (node->indexY - nodeStep)*row_vertex_count_ + node->indexX + nodeStep;
        int leftBottomIndex = (node->indexY + nodeStep)*row_vertex_count_ + node->indexX - nodeStep;
        int rightBottomIndex = (node->indexY + nodeStep)*row_vertex_count_ + node->indexX + nodeStep;

        if (!skipLeft) {
            int leftIndex = node->indexY*row_vertex_count_ + node->indexX - nodeStep;

            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = leftBottomIndex;
            indexBuffer[index_count_++] = leftIndex;

            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = leftIndex;
            indexBuffer[index_count_++] = leftTopIndex;
        } else {
            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = leftBottomIndex;
            indexBuffer[index_count_++] = leftTopIndex;
        }

        if (!skipTop) {
            int topIndex = (node->indexY - nodeStep)*row_vertex_count_ + node->indexX;

            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = leftTopIndex;
            indexBuffer[index_count_++] = topIndex;

            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = topIndex;
            indexBuffer[index_count_++] = rightTopIndex;
        } else {
            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = leftTopIndex;
            indexBuffer[index_count_++] = rightTopIndex;
        }

        if (!skipRight) {
            int rightIndex = node->indexY*row_vertex_count_ + node->indexX + nodeStep;

            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = rightTopIndex;
            indexBuffer[index_count_++] = rightIndex;

            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = rightIndex;
            indexBuffer[index_count_++] = rightBottomIndex;
        } else {
            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = rightTopIndex;
            indexBuffer[index_count_++] = rightBottomIndex;
        }

        if (!skipBottom) {
            int bottomIndex = (node->indexY + nodeStep)*row_vertex_count_ + node->indexX;

            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = rightBottomIndex;
            indexBuffer[index_count_++] = bottomIndex;

            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = bottomIndex;
            indexBuffer[index_count_++] = leftBottomIndex;
        } else {
            indexBuffer[index_count_++] = centerIndex;
            indexBuffer[index_count_++] = rightBottomIndex;
            indexBuffer[index_count_++] = leftBottomIndex;
        }
    }

    bool LodGenerator::cullNodeWithBound(QTreeNode *node, ukv3d::Matrix4x4F wvpMatrix)
    {
        bool result = false;
        ukv3d::Vector4D plane[6];

        ukv3d::Vector4D col0(wvpMatrix.m11, wvpMatrix.m21, wvpMatrix.m31, wvpMatrix.m41);
        ukv3d::Vector4D col1(wvpMatrix.m12, wvpMatrix.m22, wvpMatrix.m32, wvpMatrix.m42);
        ukv3d::Vector4D col2(wvpMatrix.m13, wvpMatrix.m23, wvpMatrix.m33, wvpMatrix.m43);
        ukv3d::Vector4D col3(wvpMatrix.m14, wvpMatrix.m24, wvpMatrix.m34, wvpMatrix.m44);

        plane[0] = col2;
        plane[1] = col3 - col2;
        plane[2] = col3 + col0;
        plane[3] = col3 - col0;
        plane[4] = col3 - col1;
        plane[5] = col3 + col1;

        ukv3d::Vector3D p, q;
        for (unsigned int i = 0; i < 6; i++) {
            plane[i].nor();

            ukv3d::Vector4D planeStore = plane[i];

            if (planeStore.x > 0) {
                q.x = node->maxcoord.x;
                p.x = node->mincoord.x;
            } else {
                p.x = node->maxcoord.x;
                q.x = node->mincoord.x;
            }

            if (planeStore.y > 0) {
                q.y = node->maxcoord.y;
                p.y = node->mincoord.y;
            } else {
                p.y = node->maxcoord.y;
                q.y = node->mincoord.y;
            }

            if (planeStore.z > 0) {
                q.z = node->maxcoord.z;
                p.z = node->mincoord.z;
            } else {
                p.z = node->maxcoord.z;
                q.z = node->mincoord.z;
            }

            auto dot = plane[i].v3() * q;
            if (dot + planeStore.w < 0) {
                result = true;
                break;
            }
        }

        return result;
    }

    void LodGenerator::constructNodeBound(QTreeNode *node, ukv3d::Point3F *bound)
    {
        int innerStep = calInnerStep(node);
        TerrainVertexData* vData[9];

        vData[0] = &vertices_[
            (node->indexY - innerStep)*row_vertex_count_ + node->indexX - innerStep];
        vData[1] = &vertices_[
            (node->indexY - innerStep)*row_vertex_count_ + node->indexX];
        vData[2] = &vertices_[
            (node->indexY - innerStep)*row_vertex_count_ + node->indexX + innerStep];
        vData[3] = &vertices_[
            node->indexY*row_vertex_count_ + node->indexX - innerStep];
        vData[4] = &vertices_[
            node->indexY*row_vertex_count_ + node->indexX];
        vData[5] = &vertices_[
            node->indexY*row_vertex_count_ + node->indexX + innerStep];
        vData[6] = &vertices_[
            (node->indexY + innerStep)*row_vertex_count_ + node->indexX - innerStep];
        vData[7] = &vertices_[
            (node->indexY + innerStep)*row_vertex_count_ + node->indexX];
        vData[8] = &vertices_[
            (node->indexY + innerStep)*row_vertex_count_ + node->indexX + innerStep];


        float minX = 0.f;
        float maxX = 0.f;
        float minY = 0.f;
        float maxY = 0.f;
        float minZ = 0.f;
        float maxZ = 0.f;

        for (int i = 0; i < 9; ++i) {
            if (i == 0) {
                minX = maxX = vData[i]->position.x;
                minY = maxY = vData[i]->position.y;
                minZ = maxZ = vData[i]->position.z;
            } else {
                float x = vData[i]->position.x;
                float y = vData[i]->position.y;
                float z = vData[i]->position.z;

                if (x < minX) minX = x;
                if (x > maxX) maxX = x;

                if (y < minY) minY = y;
                if (y > maxY) maxY = y;

                if (z < minZ) minZ = z;
                if (z > maxZ) maxZ = z;
            }
        }

        bound[0].x = minX;
    }


    void LodGenerator::renderLodTerrain(
        ukv3d::Point3F viewPosition, ukv3d::Matrix4x4F wvpMatrix, int* indexBuffer)
    {
        int level = 0;
        index_count_ = 0;
        QTreeNode* curQueue = nullptr;
        QTreeNode* nextQueue = nullptr;

        QTreeNode* rootNode = root_queue_;
        flags_[rootNode->indexY*row_vertex_count_ + rootNode->indexX] = 1;
        curQueue = rootNode;

        while (level <= max_level_ - 1)
        {
            QTreeNode* iterator = curQueue;
            while (iterator)
            {
                QTreeNode* parent = iterator;
                iterator = iterator->next;

                if (cullNodeWithBound(parent, wvpMatrix))
                {
                    int innerStep = calInnerStep(parent);
                    for (int i = -innerStep + 1; i < innerStep; ++i)
                    {
                        ::memset(&flags_[(parent->indexY + i)*row_vertex_count_
                            + parent->indexX - innerStep + 1], -1, innerStep * 2 - 2);
                    }
                    continue;
                }
                else if (level == max_level_ - 1)
                {
                    drawNode(parent, indexBuffer);
                }
                else if (checkNodeCanDivide(parent)
                    && assessNodeCanDivide(parent, viewPosition))
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        enqueue(nextQueue, parent->child[i]);

                        flags_[parent->child[i]->indexY*row_vertex_count_
                            + parent->child[i]->indexX] = 1;
                    }
                }
                else
                {
                    int step = calChildStep(parent);

                    flags_[(parent->indexY - step)*row_vertex_count_
                        + parent->indexX - step] = 0;
                    flags_[(parent->indexY - step)*row_vertex_count_
                        + parent->indexX + step] = 0;
                    flags_[(parent->indexY + step)*row_vertex_count_
                        + parent->indexX - step] = 0;
                    flags_[(parent->indexY + step)*row_vertex_count_
                        + parent->indexX + step] = 0;

                    drawNode(parent, indexBuffer);
                }
            }

            curQueue = nextQueue;
            nextQueue = nullptr;

            ++level;
        }
    }

    void LodGenerator::setCoefficient(float c1, float c2) {
        coe_rough_ = c1;
        coe_distance_ = c2;
    }

    int LodGenerator::getLevel() {
        return max_level_;
    }

    float LodGenerator::getCoef1() {
        return coe_rough_;
    }

    float LodGenerator::getCoef2() {
        return coe_distance_;
    }


    TerrainVertexData *LodGenerator::getVertices() {
        return vertices_;
    }

    int LodGenerator::getVertexCount() {
        return vertex_count_;
    }

    int LodGenerator::getRowVertexCount() {
        return row_vertex_count_;
    }

    int* LodGenerator::getIndices() {
        return indices_;
    }

    int LodGenerator::getIndexCount() {
        return index_count_;
    }

    int LodGenerator::getMaxIndexCount() {
        return max_index_count_;
    }

}
