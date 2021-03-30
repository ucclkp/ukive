// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_CAMERA_H_
#define UKIVE_GRAPHICS_3D_CAMERA_H_

#include "ukive/graphics/3d/matrix.hpp"


namespace ukv3d {

    class Camera {
    public:
        Camera(int width, int height);
        ~Camera();

        void resize(int width, int height);

        void moveCamera(float dx, float dy);
        void scaleCamera(float factor);
        void circuleCamera2(float dxAngle, float dyAngle);

        void moveWorld(float dx, float dy);
        void scaleWorld(int direction);
        void rotateWorld(float dxAngle, float dyAngle);

        // 该方法有缺陷，请看方法内注释。
        void setCameraPosition(float x, float y, float z);
        // 该方法有缺陷，请看方法内注释。
        void setCameraLookAt(float x, float y, float z);

        const Point3F* getCameraPos() const;
        const Point3F* getCameraLookAt() const;
        const Vector3F* getCameraUp() const;

        const Matrix4x4F* getWorldMatrix() const;
        const Matrix4x4F* getViewMatrix() const;
        const Matrix4x4F* getProjectionMatrix() const;
        const Matrix4x4F* getOrthoMatrix() const;

        void getWVPMatrix(Matrix4x4F* wvp) const;
        void getWVOMatrix(Matrix4x4F* wvo) const;

    private:
        Point3F pos_;
        Point3F look_at_;
        Vector3F up_;
        Vector3F right_;
        Vector3F look_;
        Vector3F z_vector_;
        Vector3F y_vector_;

        Matrix4x4F world_matrix_;
        Matrix4x4F view_matrix_;
        Matrix4x4F projection_matrix_;
        Matrix4x4F ortho_matrix_;

        int width_;
        int height_;
        float radius_;
    };

}

#endif  // UKIVE_GRAPHICS_3D_CAMERA_H_