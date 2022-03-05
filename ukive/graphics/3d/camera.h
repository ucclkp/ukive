// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_CAMERA_H_
#define UKIVE_GRAPHICS_3D_CAMERA_H_

#include "utils/math/algebra/point.hpp"


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

        const utl::pt3f* getCameraPos() const;
        const utl::pt3f* getCameraLookAt() const;
        const utl::vec3f* getCameraUp() const;

        const utl::mat4f* getWorldMatrix() const;
        const utl::mat4f* getViewMatrix() const;
        const utl::mat4f* getProjectionMatrix() const;
        const utl::mat4f* getOrthoMatrix() const;

        void getWVPMatrix(utl::mat4f* wvp) const;
        void getWVOMatrix(utl::mat4f* wvo) const;

    private:
        utl::pt3f pos_;
        utl::pt3f look_at_;
        utl::vec3f up_;
        utl::vec3f right_;
        utl::vec3f look_;
        utl::vec3f z_vector_;
        utl::vec3f y_vector_;

        utl::mat4f world_matrix_;
        utl::mat4f view_matrix_;
        utl::mat4f projection_matrix_;
        utl::mat4f ortho_matrix_;

        int width_;
        int height_;
        double radius_;
    };

}

#endif  // UKIVE_GRAPHICS_3D_CAMERA_H_