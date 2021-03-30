// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "camera.h"

#include <algorithm>

#define NEAR_PLANE 10.0f
#define FAR_PLANE 100000.0f
#define PIDIV4  3.14159265359 / 4


namespace ukv3d {

    Camera::Camera(int width, int height)
        : width_(width >= 1 ? width : 1),
          height_(height >= 1 ? height : 1)
    {
        //摄像机位置。
        pos_.set(0.0f, 0.0f, -10.0f);
        //摄像机看向的位置。
        look_at_.set(0.0f, 0.0f, 0.0f);

        up_.set(0.0f, 1.0f, 0.0f);
        right_.set(1.0f, 0.0f, 0.0f);
        look_.set(0.0f, 0.0f, 1.0f);

        z_vector_.set(0.0f, 0.0f, 1.0f);
        y_vector_.set(0.0f, 1.0f, 0.0f);

        auto upVec = Vector3D(up_);
        auto posVec = Point3D(pos_);
        auto lookAtVec = Point3D(look_at_);

        radius_ = (lookAtVec - posVec).length();

        world_matrix_.identity();

        {
            auto m = Matrix4x4D::camera(posVec, posVec - lookAtVec, upVec);
            Matrix4x4D tm;
            m.transport(&tm);
            view_matrix_ = Matrix4x4F(tm);
        }
        {
            double fov_height = 2.0 * NEAR_PLANE * std::tan(PIDIV4 / 2.0);
            double fov_width = width_ / double(height_) * fov_height;

            auto m = Matrix4x4D::orthoProj(
                -fov_width / 2.0, fov_width / 2.0, -fov_height / 2.0, fov_height / 2.0, NEAR_PLANE, FAR_PLANE)
                * Matrix4x4D::persp(NEAR_PLANE, FAR_PLANE);
            // DirectX 的正规矩阵是左手的，且 Z 深度设置为了 [0, 1]。
            m.m33 *= -1;
            m.m34 *= -0.5;

            Matrix4x4D tm;
            m.transport(&tm);
            projection_matrix_ = Matrix4x4F(tm);
        }
        {
            auto m = Matrix4x4D::orthoProj(
                -width_ / 2.f, width_ / 2.f, -height_ / 2.f, height_ / 2.f, NEAR_PLANE, FAR_PLANE);
            // DirectX 的正规矩阵是左手的，且 Z 深度设置为了 [0, 1]，且近平面和 XOY 平面重合。
            Matrix4x4D adj(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, -0.5f, FAR_PLANE / 2.f + NEAR_PLANE,
                0, 0, 0, 1);
            m.mul(adj);

            Matrix4x4D tm;
            m.transport(&tm);
            ortho_matrix_ = Matrix4x4F(tm);
        }
    }

    Camera::~Camera() {}

    void Camera::resize(int width, int height) {
        width_ = std::max(width, 1);
        height_ = std::max(height, 1);

        {
            double fov_height = 2.0 * NEAR_PLANE * std::tan(PIDIV4 / 2.0);
            double fov_width = width_ / double(height_) * fov_height;

            auto m = Matrix4x4D::orthoProj(-fov_width / 2.0, fov_width / 2.0, -fov_height / 2.0, fov_height / 2.0, NEAR_PLANE, FAR_PLANE)
                * Matrix4x4D::persp(NEAR_PLANE, FAR_PLANE);
            m.m33 *= -1;
            m.m34 *= -0.5;

            Matrix4x4D tm;
            m.transport(&tm);
            projection_matrix_ = Matrix4x4F(tm);
        }
        {
            auto m = Matrix4x4D::orthoProj(-width_/2.f, width_/2.f, -height_/2.f, height_/2.f, NEAR_PLANE, FAR_PLANE);
            Matrix4x4D adj(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, -0.5f, FAR_PLANE / 2.f + NEAR_PLANE,
                0, 0, 0, 1);
            m.mul(adj);

            Matrix4x4D tm;
            m.transport(&tm);
            ortho_matrix_ = Matrix4x4F(tm);
        }
    }

    void Camera::moveCamera(float dx, float dy) {
        Vector3D frontVec(look_.x, 0, look_.z);
        frontVec.nor();

        auto upVec = Vector3D(up_);
        auto rightVec = Vector3D(right_);
        auto posVec = Point3D(pos_);
        auto lookAtVec = Point3D(look_at_);

        auto deltaVec = frontVec * dy + rightVec * dx;

        posVec = posVec + deltaVec;
        lookAtVec = lookAtVec + deltaVec;

        auto m = Matrix4x4D::camera(posVec, posVec - lookAtVec, upVec);
        Matrix4x4D tm;
        m.transport(&tm);

        view_matrix_ = Matrix4x4F(tm);
        pos_ = Point3F(posVec);
        look_at_ = Point3F(lookAtVec);
    }

    void Camera::scaleCamera(float factor) {
        auto upVec = Vector3D(up_);
        auto lookVec = Vector3D(look_);
        auto lookAtVec = Point3D(look_at_);

        radius_ *= factor;
        auto posVec = lookAtVec + lookVec * -radius_;

        auto m = Matrix4x4D::camera(posVec, posVec - lookAtVec, upVec);
        Matrix4x4D tm;
        m.transport(&tm);

        view_matrix_ = Matrix4x4F(tm);
        pos_ = Point3F(posVec);
    }

    void Camera::circuleCamera2(float dxAngle, float dyAngle) {
        auto yVec = Vector3D(y_vector_);
        auto rightVec = Vector4D(right_.x, right_.y, right_.z, 0);
        auto posVec = Point3D(pos_);
        auto lookVec = Vector4D(look_.x, look_.y, look_.z, 0);

        auto rotateMatrix = Matrix4x4D::rotateAxis(yVec, -dxAngle);
        lookVec = lookVec.mul(rotateMatrix);
        rightVec = rightVec.mul(rotateMatrix);
        rightVec.nor();

        auto lookAtVec = posVec + lookVec.v3() * radius_;

        rotateMatrix = Matrix4x4D::rotateAxis(rightVec.v3(), -dyAngle);

        lookVec = lookVec.mul(rotateMatrix);
        lookAtVec = posVec + lookVec.v3() * radius_;

        auto upVec = lookVec.v3() ^ rightVec.v3();
        upVec.nor();
        lookVec.nor();

        {
            auto m = Matrix4x4D::camera(posVec, posVec - lookAtVec, upVec);
            Matrix4x4D tm;
            m.transport(&tm);
            view_matrix_ = Matrix4x4F(tm);
        }

        up_ = Vector3F(upVec);
        right_ = Vector3F(rightVec.v3());
        look_ = Vector3F(lookVec.v3());
        look_at_ = Point3F(lookAtVec);
    }


    void Camera::moveWorld(float dx, float dy) {
        Vector3D frontVec(look_.x, 0, look_.z);
        frontVec.nor();
        frontVec = frontVec * dy;

        auto rightVec = Vector3D(right_);
        rightVec = rightVec * dx;

        auto transMatrix = Matrix4x4D::translate(
            frontVec.x + rightVec.x, 0, frontVec.z + rightVec.z);

        auto worldMatrix = Matrix4x4D(world_matrix_);
        worldMatrix = worldMatrix * transMatrix;
        world_matrix_ = Matrix4x4F(worldMatrix);
    }

    void Camera::scaleWorld(int direction) {
        float scaleFactor;

        if (direction > 0)
            scaleFactor = 1.1f;
        else
            scaleFactor = 0.9f;

        auto scaleMatrix
            = Matrix4x4D::scale(scaleFactor, scaleFactor, scaleFactor);

        auto worldMatrix = Matrix4x4D(world_matrix_);
        worldMatrix = worldMatrix * scaleMatrix;
        world_matrix_ = Matrix4x4F(worldMatrix);
    }

    void Camera::rotateWorld(float dxAngle, float dyAngle) {
        auto rotateMatrix = Matrix4x4D::rotateY(dxAngle);

        auto worldMatrix = Matrix4x4D(world_matrix_);
        worldMatrix = worldMatrix * rotateMatrix;
        world_matrix_ = Matrix4x4F(worldMatrix);
    }

    void Camera::setCameraPosition(float x, float y, float z) {
        auto upVec = Vector3D(up_);
        auto lookAtVec = Point3D(look_at_);
        Point3D posVec(x, y, z);
        auto lookVec = lookAtVec - posVec;

        radius_ = lookVec.length();

        // TODO:此处假设在变换前后 up 不变。当 up 在变换后改变，需要额外变量确定 right。
        auto rightVec = upVec ^ lookVec;
        upVec = lookVec ^ rightVec;

        upVec.nor();
        lookVec.nor();
        rightVec.nor();

        {
            auto m = Matrix4x4D::camera(posVec, posVec - lookAtVec, upVec);
            Matrix4x4D tm;
            m.transport(&tm);
            view_matrix_ = Matrix4x4F(tm);
        }

        up_ = Vector3F(upVec);
        right_ = Vector3F(rightVec);
        pos_ = Point3F(posVec);
        look_ = Vector3F(lookVec);
    }

    void Camera::setCameraLookAt(float x, float y, float z) {
        auto upVec = Vector3D(up_);
        auto posVec = Point3D(pos_);
        Point3D lookAtVec(x, y, z);
        auto lookVec = lookAtVec - posVec;

        radius_ = lookVec.length();

        // TODO:此处假设在变换前后 up 不变。当 up 在变换后改变，需要额外变量确定 right。
        auto rightVec = upVec ^ lookVec;
        upVec = lookVec ^ rightVec;

        upVec.nor();
        lookVec.nor();
        rightVec.nor();

        {
            auto m = Matrix4x4D::camera(posVec, posVec - lookAtVec, upVec);
            Matrix4x4D tm;
            m.transport(&tm);
            view_matrix_ = Matrix4x4F(tm);
        }

        up_ = Vector3F(upVec);
        right_ = Vector3F(rightVec);
        look_ = Vector3F(lookVec);
        look_at_ = Point3F(lookAtVec);
    }

    const Point3F* Camera::getCameraPos() const {
        return &pos_;
    }

    const Point3F* Camera::getCameraLookAt() const {
        return &look_at_;
    }

    const Vector3F* Camera::getCameraUp() const {
        return &up_;
    }

    const Matrix4x4F* Camera::getWorldMatrix() const {
        return &world_matrix_;
    }

    const Matrix4x4F* Camera::getViewMatrix() const {
        return &view_matrix_;
    }

    const Matrix4x4F* Camera::getProjectionMatrix() const {
        return &projection_matrix_;
    }

    const Matrix4x4F* Camera::getOrthoMatrix() const {
        return &ortho_matrix_;
    }

    void Camera::getWVPMatrix(Matrix4x4F* wvp) const {
        auto world = Matrix4x4D(world_matrix_);
        auto view = Matrix4x4D(view_matrix_);
        auto projection = Matrix4x4D(projection_matrix_);

        *wvp = Matrix4x4F(world * view * projection);
    }

    void Camera::getWVOMatrix(Matrix4x4F* wvo) const {
        auto world = Matrix4x4D(world_matrix_);
        auto view = Matrix4x4D(view_matrix_);
        auto ortho = Matrix4x4D(ortho_matrix_);

        *wvo = Matrix4x4F(world * view * ortho);
    }

}