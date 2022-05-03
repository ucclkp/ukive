// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "space3d_view.h"

#include <algorithm>

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/window/window.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/3d/scene.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/images/image_options.h"


namespace ukive {

    Space3DView::Space3DView(Context c, ukv3d::Scene* scene)
        : View(c),
          scene_(scene)
    {
        setFocusable(true);
        setMinimumWidth(1);
        setMinimumHeight(1);

        scene_->onSceneCreate(this);
    }

    Space3DView::~Space3DView() {
        scene_->onSceneDestroy();
    }

    Size Space3DView::onDetermineSize(const SizeInfo& info) {
        return getPreferredSize(info, 0, 0);
    }

    void Space3DView::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        if (content_img_) {
            auto gpu_context = Application::getGraphicDeviceManager()->getGPUContext();

            gpu_context->setRenderTargets(1, &render_target_view_, nullptr);

            Color d2d_color = getWindow()->getBackgroundColor();
            float bg_color[4] = { d2d_color.r, d2d_color.g, d2d_color.b, d2d_color.a };
            gpu_context->clearRenderTarget(render_target_view_.get(), bg_color);

            scene_->onSceneRender(gpu_context.get(), render_target_view_.get());

            canvas->drawImage(content_img_.get());
        }
    }

    bool Space3DView::onInputEvent(InputEvent* e) {
        bool result = View::onInputEvent(e);
        result |= scene_->onSceneInput(e);
        return result;
    }

    void Space3DView::onBoundsChanged(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        View::onBoundsChanged(new_bounds, old_bounds);

        if (new_bounds.size() == old_bounds.size()) {
            return;
        }

        int content_width = new_bounds.width() - getPadding().hori();
        int content_height = new_bounds.height() - getPadding().vert();

        releaseResources();
        createResources(content_width, content_height);

        scene_->onSceneResize(content_width, content_height);
    }

    void Space3DView::onContextChanged(Context::Type type, const Context& context) {
        View::onContextChanged(type, context);

        switch (type) {
        case Context::DEV_LOST:
        {
            // 通知场景，要把设备资源释放掉
            scene_->onGraphicDeviceLost();
            releaseResources();
            break;
        }

        case Context::DEV_RESTORE:
        {
            auto bounds(getContentBounds());

            // 重新创建设备资源
            createResources(bounds.width(), bounds.height());

            scene_->onGraphicDeviceRestored();
            scene_->onSceneResize(bounds.width(), bounds.height());
            break;
        }

        default:
            break;
        }
    }

    bool Space3DView::createResources(int width, int height) {
        auto gpu_device = Application::getGraphicDeviceManager()->getGPUDevice();

        // RTT
        GPUTexture::Desc tex_desc;
        tex_desc.format = GPUDataFormat::B8G8B8R8_UNORM;
        tex_desc.width = width;
        tex_desc.height = height;
        tex_desc.depth = 1;
        tex_desc.mip_levels = 1;
        tex_desc.res_type = GPUResource::RES_RENDER_TARGET | GPUResource::RES_SHADER_RES;
        tex_desc.is_dynamic = false;
        tex_desc.dim = GPUTexture::Dimension::_2D;
        content_surface_ = gpu_device->createTexture(tex_desc, nullptr);
        if (!content_surface_) {
            LOG(Log::WARNING) << "Failed to create 2d texture";
            return false;
        }

        render_target_view_ = gpu_device->createRenderTarget(content_surface_.get());
        if (!render_target_view_) {
            LOG(Log::WARNING) << "Failed to create render target view";
            return false;
        }

        content_img_ = getWindow()->getCanvas()->createImage(content_surface_);
        if (!content_img_) {
            LOG(Log::WARNING) << "Failed to create content image";
            return false;
        }
        return true;
    }

    void Space3DView::releaseResources() {
        content_img_.reset();
        content_surface_.reset();
        render_target_view_.reset();
    }

}