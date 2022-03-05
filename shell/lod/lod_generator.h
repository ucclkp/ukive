// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_LOD_LOD_GENERATOR_H_
#define SHELL_LOD_LOD_GENERATOR_H_

#define ALTITUDE_MAP_SIZE 1024

#include "utils/math/algebra/point.hpp"


namespace shell {

    class QTreeNode;
    struct TerrainVertexData;

    class LodGenerator {
    public:
        LodGenerator(float edgeLength, int maxLevel);
        ~LodGenerator();

        void setCoefficient(float c1, float c2);

        int getLevel();
        float getCoef1();
        float getCoef2();

        void renderLodTerrain(
            utl::pt3f viewPosition, utl::mat4f wvpMatrix, int* indexBuffer);

        TerrainVertexData* getVertices();
        int getVertexCount();
        int getRowVertexCount();

        int* getIndices();
        int getIndexCount();
        int getMaxIndexCount();

    private:
        void enqueue(QTreeNode*& queue, QTreeNode* node);
        QTreeNode* dequeue(QTreeNode*& queue);
        void clearQueue(QTreeNode*& queue);

        int calInnerStep(QTreeNode* node);
        int calNeighborStep(QTreeNode* node);
        int calChildStep(QTreeNode* node);

        void generateQuadTree();
        void generateRootNodeData(QTreeNode* root);
        void generateChildNodeData(
            QTreeNode* parent, QTreeNode* child, int level, int position);

        void determineRoughAndBound(QTreeNode* node);

        bool checkNodeCanDivide(QTreeNode* node);
        bool assessNodeCanDivide(
            QTreeNode* node, utl::pt3f viewPosition);

        void drawNode(QTreeNode* node, int* indexBuffer);

        bool cullNodeWithBound(QTreeNode* node, utl::mat4f wvpMatrix);
        void constructNodeBound(QTreeNode* node, utl::pt3f* bound);

        int max_level_;
        int vertex_count_;
        int row_vertex_count_;

        float coe_rough_;
        float coe_distance_;
        QTreeNode* root_queue_;

        char* flags_;
        char* altitude_;
        TerrainVertexData* vertices_;

        int* indices_;
        int index_count_;
        int max_index_count_;
    };

}

#endif  // SHELL_LOD_LOD_GENERATOR_H_