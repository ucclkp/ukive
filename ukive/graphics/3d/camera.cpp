// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "camera.h"

#include <algorithm>

#include "utils/math/algebra/special_matrix.hpp"

#define NEAR_PLANE 10.0f
#define FAR_PLANE 100000.0f
#define PIDIV4  3.14159265359 / 4


namespace ukv3d {

    Camera::Camera(int width, int height)
        : width_(width >= 1 ? width : 1),
          height_(height >= 1 ? height : 1)
    {
        //摄像机位置。
        pos_ = { 0.0f, 0.0f, -10.0f };
        //摄像机看向的位置。
        look_at_ = { 0.0f, 0.0f, 0.0f };

        up_ = { 0.0f, 1.0f, 0.0f };
        right_ = { 1.0f, 0.0f, 0.0f };
        look_ = { 0.0f, 0.0f, 1.0f };

        z_vector_ = { 0.0f, 0.0f, 1.0f};
        y_vector_ = { 0.0f, 1.0f, 0.0f};

        auto upVec = utl::vec3d(up_);
        auto posVec = utl::pt3d(pos_);
        auto lookAtVec = utl::pt3d(look_at_);

        radius_ = (lookAtVec - posVec).length();

        world_matrix_.identity();

        {
            auto m = utl::math::camera4x4(posVec, posVec - lookAtVec, upVec);
            view_matrix_ = utl::mat4f(m.T());
        }
        {
            double fov_height = 2.0 * NEAR_PLANE * std::tan(PIDIV4 / 2.0);
            double fov_width = width_ / double(height_) * fov_height;

            auto m = utl::math::orthoProj4x4<double>(
                -fov_width / 2.0, fov_width / 2.0, -fov_height / 2.0, fov_height / 2.0, NEAR_PLANE, FAR_PLANE)
                * utl::math::persp4x4<double>(NEAR_PLANE, FAR_PLANE);
            // DirectX 的正规矩阵是左手的，且 Z 深度设置为了 [0, 1]。
            m(2, 2) *= -1;
            m(2, 3) *= -0.5;

            projection_matrix_ = utl::mat4f(m.T());
        }
        {
            auto m = utl::math::orthoProj4x4<double>(
                -width_ / 2.f, width_ / 2.f, -height_ / 2.f, height_ / 2.f, NEAR_PLANE, FAR_PLANE);
            // DirectX 的正规矩阵是左手的，且 Z 深度设置为了 [0, 1]，且近平面和 XOY 平面重合。
            utl::mat4d adj{
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, -0.5f, FAR_PLANE / 2.f + NEAR_PLANE,
                0, 0, 0, 1 };
            m.mul(adj);

            ortho_matrix_ = utl::mat4f(m.T());
        }
    }

    Camera::~Camera() {}

    void Camera::resize(int width, int height) {
        width_ = (std::max)(width, 1);
        height_ = (std::max)(height, 1);

        {
            double fov_height = 2.0 * NEAR_PLANE * std::tan(PIDIV4 / 2.0);
            double fov_width = width_ / double(height_) * fov_height;

            auto m = utl::math::orthoProj4x4<double>(-fov_width / 2.0, fov_width / 2.0, -fov_height / 2.0, fov_height / 2.0, NEAR_PLANE, FAR_PLANE)
                * utl::math::persp4x4<double>(NEAR_PLANE, FAR_PLANE);
            m(2, 2) *= -1;
            m(2, 3) *= -0.5;

            projection_matrix_ = utl::mat4f(m.T());
        }
        {
            auto m = utl::math::orthoProj4x4<double>(-width_/2.f, width_/2.f, -height_/2.f, height_/2.f, NEAR_PLANE, FAR_PLANE);
            utl::mat4d adj{
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, -0.5f, FAR_PLANE / 2.f + NEAR_PLANE,
                0, 0, 0, 1 };
            m.mul(adj);

            ortho_matrix_ = utl::mat4f(m.T());
        }
    }

    void Camera::moveCamera(float dx, float dy) {
        utl::vec3d frontVec{ look_.x(), 0, look_.z() };
        frontVec.nor();

        auto upVec = utl::vec3d(up_);
        auto rightVec = utl::vec3d(right_);
        auto posVec = utl::pt3d(pos_);
        auto lookAtVec = utl::pt3d(look_at_);

        auto deltaVec = frontVec * dy + rightVec * dx;

        posVec = posVec + deltaVec;
        lookAtVec = lookAtVec + deltaVec;

        auto m = utl::math::camera4x4(posVec, posVec - lookAtVec, upVec);
        view_matrix_ = utl::mat4f(m.T());
        pos_ = utl::pt3f(posVec);
        look_at_ = utl::pt3f(lookAtVec);
    }

    void Camera::scaleCamera(float factor) {
        auto upVec = utl::vec3d(up_);
        auto lookVec = utl::vec3d(look_);
        auto lookAtVec = utl::pt3d(look_at_);

        radius_ *= factor;
        auto posVec = lookAtVec + lookVec * -radius_;

        auto m = utl::math::camera4x4(posVec, posVec - lookAtVec, upVec);
        view_matrix_ = utl::mat4f(m.T());
        pos_ = utl::pt3f(posVec);
    }

    void Camera::circuleCamera2(float dxAngle, float dyAngle) {
        auto yVec = utl::vec3d(y_vector_);
        auto rightVec = utl::hvec4d{ right_.x(), right_.y(), right_.z(), 0 };
        auto posVec = utl::pt3d(pos_);
        auto lookVec = utl::hvec4d{ look_.x(), look_.y(), look_.z(), 0 };

        auto rotateMatrix = utl::math::rotateAxis4x4<double>(yVec, -dxAngle);
        lookVec = lookVec.mul(rotateMatrix);
        rightVec = rightVec.mul(rotateMatrix);
        rightVec.nor();

        auto lookAtVec = posVec + lookVec.reduce<3>() * radius_;

        rotateMatrix = utl::math::rotateAxis4x4<double>(rightVec.reduce<3>().T(), -dyAngle);

        lookVec = lookVec.mul(rotateMatrix);
        lookAtVec = posVec + lookVec.reduce<3>() * radius_;

        auto upVec = lookVec.reduce<3>() ^ rightVec.reduce<3>();
        upVec.nor();
        lookVec.nor();

        {
            auto m = utl::math::camera4x4(posVec, posVec - lookAtVec, upVec.T());
            view_matrix_ = utl::mat4f(m.T());
        }

        up_ = utl::vec3f(upVec.T());
        right_ = utl::vec3f(rightVec.reduce<3>().T());
        look_ = utl::vec3f(lookVec.reduce<3>().T());
        look_at_ = utl::pt3f(lookAtVec);
    }


    void Camera::moveWorld(float dx, float dy) {
        utl::vec3d frontVec{ look_.x(), 0, look_.z() };
        frontVec.nor();
        frontVec = frontVec * dy;

        auto rightVec = utl::vec3d(right_);
        rightVec = rightVec * dx;

        auto transMatrix = utl::math::translate4x4<double>(
            frontVec.x() + rightVec.x(), 0, frontVec.z() + rightVec.z());

        auto worldMatrix = utl::mat4d(world_matrix_);
        worldMatrix = worldMatrix * transMatrix;
        world_matrix_ = utl::mat4f(worldMatrix);
    }

    void Camera::scaleWorld(int direction) {
        float scaleFactor;

        if (direction > 0)
            scaleFactor = 1.1f;
        else
            scaleFactor = 0.9f;

        auto scaleMatrix
            = utl::math::scale4x4<double>(scaleFactor, scaleFactor, scaleFactor);

        auto worldMatrix = utl::mat4d(world_matrix_);
        worldMatrix = worldMatrix * scaleMatrix;
        world_matrix_ = utl::mat4f(worldMatrix);
    }

    void Camera::rotateWorld(float dxAngle, float dyAngle) {
        auto rotateMatrix = utl::math::rotateY4x4<double>(dxAngle);

        auto worldMatrix = utl::mat4d(world_matrix_);
        worldMatrix = worldMatrix * rotateMatrix;
        world_matrix_ = utl::mat4f(worldMatrix);
    }

    void Camera::setCameraPosition(float x, float y, float z) {
        auto upVec = utl::vec3d(up_);
        auto lookAtVec = utl::pt3d(look_at_);
        utl::pt3d posVec{ x, y, z };
        auto lookVec = lookAtVec - posVec;

        radius_ = lookVec.length();

        // TODO:此处假设在变换前后 up 不变。当 up 在变换后改变，需要额外变量确定 right。
        auto rightVec = upVec ^ lookVec;
        upVec = lookVec ^ rightVec;

        upVec.nor();
        lookVec.nor();
        rightVec.nor();

        {
            auto m = utl::math::camera4x4(posVec, posVec - lookAtVec, upVec);
            view_matrix_ = utl::mat4f(m.T());
        }

        up_ = utl::vec3f(upVec);
        right_ = utl::vec3f(rightVec);
        pos_ = utl::pt3f(posVec);
        look_ = utl::vec3f(lookVec);
    }

    void Camera::setCameraLookAt(float x, float y, float z) {
        auto upVec = utl::vec3d(up_);
        auto posVec = utl::pt3d(pos_);
        utl::pt3d lookAtVec{ x, y, z };
        auto lookVec = lookAtVec - posVec;

        radius_ = lookVec.length();

        // TODO:此处假设在变换前后 up 不变。当 up 在变换后改变，需要额外变量确定 right。
        auto rightVec = upVec ^ lookVec;
        upVec = lookVec ^ rightVec;

        upVec.nor();
        lookVec.nor();
        rightVec.nor();

        {
            auto m = utl::math::camera4x4(posVec, posVec - lookAtVec, upVec);
            view_matrix_ = utl::mat4f(m.T());
        }

        up_ = utl::vec3f(upVec);
        right_ = utl::vec3f(rightVec);
        look_ = utl::vec3f(lookVec);
        look_at_ = utl::pt3f(lookAtVec);
    }

    const utl::pt3f* Camera::getCameraPos() const {
        return &pos_;
    }

    const utl::pt3f* Camera::getCameraLookAt() const {
        return &look_at_;
    }

    const utl::vec3f* Camera::getCameraUp() const {
        return &up_;
    }

    const utl::mat4f* Camera::getWorldMatrix() const {
        return &world_matrix_;
    }

    const utl::mat4f* Camera::getViewMatrix() const {
        return &view_matrix_;
    }

    const utl::mat4f* Camera::getProjectionMatrix() const {
        return &projection_matrix_;
    }

    const utl::mat4f* Camera::getOrthoMatrix() const {
        return &ortho_matrix_;
    }

    void Camera::getWVPMatrix(utl::mat4f* wvp) const {
        auto world = utl::mat4d(world_matrix_);
        auto view = utl::mat4d(view_matrix_);
        auto projection = utl::mat4d(projection_matrix_);

        *wvp = utl::mat4f(world * view * projection);
    }

    void Camera::getWVOMatrix(utl::mat4f* wvo) const {
        auto world = utl::mat4d(world_matrix_);
        auto view = utl::mat4d(view_matrix_);
        auto ortho = utl::mat4d(ortho_matrix_);

        *wvo = utl::mat4f(world * view * ortho);
    }

}