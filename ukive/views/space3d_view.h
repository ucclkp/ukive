// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_SPACE3D_VIEW_H_
#define UKIVE_VIEWS_SPACE3D_VIEW_H_

#include "ukive/graphics/gptr.hpp"
#include "ukive/views/view.h"


namespace ukv3d {
    class Scene;
}

namespace ukive {

    class GPUTexture;
    class GPURenderTarget;
    class ImageFrame;

    class Space3DView : public View {
    public:
        Space3DView(Context c, ukv3d::Scene* scene);
        ~Space3DView();

    protected:
        Size onDetermineSize(const SizeInfo& info) override;
        void onBoundsChanged(const Rect& new_bounds, const Rect& old_bounds) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

        void onContextChanged(Context::Type type, const Context& context) override;

    private:
        bool createResources(int width, int height);
        void releaseResources();

        ukv3d::Scene* scene_;
        GPtr<ImageFrame> content_img_;
        GPtr<GPUTexture> content_surface_;
        GPtr<GPURenderTarget> render_target_view_;
    };

}

#endif  // UKIVE_VIEWS_SPACE3D_VIEW_H_