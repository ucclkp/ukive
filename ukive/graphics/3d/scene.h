// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_SCENE_H_
#define UKIVE_GRAPHICS_3D_SCENE_H_


namespace ukive {
    class InputEvent;
    class Space3DView;
    class GPUContext;
    class GPURenderTarget;
}

namespace ukv3d {

    class Scene {
    public:
        virtual ~Scene() = default;

        virtual void onSceneCreate(ukive::Space3DView* view) {}
        virtual bool onSceneInput(ukive::InputEvent* e) { return false; }
        virtual void onSceneResize(int width, int height) {}
        virtual void onSceneRender(ukive::GPUContext* context, ukive::GPURenderTarget* rt) {}
        virtual void onSceneDestroy() {}

        virtual void onGraphicDeviceLost() {}
        virtual void onGraphicDeviceRestored() {}
    };

}

#endif  // UKIVE_GRAPHICS_3D_SCENE_H_