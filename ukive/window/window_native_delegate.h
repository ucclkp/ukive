// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WINDOW_NATIVE_DELEGATE_H_
#define UKIVE_WINDOW_WINDOW_NATIVE_DELEGATE_H_

#include <string>

#include "ukive/graphics/cursor.h"
#include "ukive/graphics/size.hpp"
#include "ukive/window/context.h"
#include "ukive/window/window_listener.h"
#include "ukive/window/window_types.h"


namespace ukive {

    class DirtyRegion;
    class InputEvent;

    class WindowNativeDelegate {
    public:
        virtual ~WindowNativeDelegate() = default;

        virtual void onCreate() {}
        virtual void onCreated() {}
        virtual void onShow(bool show) {}
        virtual void onActivate(bool activate) {}
        virtual void onSetFocus() {}
        virtual void onKillFocus() {}
        virtual void onSetText(const std::u16string& text) {}
        virtual void onSetIcon() {}
        virtual void onLayout() {}
        virtual void onDraw(const DirtyRegion& region) {}
        virtual void onMove(int x, int y) {}
        virtual void onResize(int type, int width, int height) {}
        virtual void onMoving() {}
        virtual bool onResizing(Size* new_size) { return true; }
        virtual bool onClose() { return true; }
        virtual void onDestroy() {}
        virtual void onDestroyed() {}
        virtual bool onInputEvent(InputEvent* e) { return false; }
        virtual HitPoint onNCHitTest(int x, int y) { return HitPoint::CLIENT; }
        virtual void onWindowButtonChanged(WindowButton button) {}
        virtual bool onGetWindowIconName(
            std::u16string* icon_name, std::u16string* small_icon_name) const { return false; }

        virtual void onPostLayout() {}
        virtual void onPostRender() {}

        virtual Context onGetContext() const = 0;
        virtual void onUpdateContext(Context::Type type) {}
    };

}

#endif  // UKIVE_WINDOW_WINDOW_NATIVE_DELEGATE_H_
