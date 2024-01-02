// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "effect_window.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/graphics/colors/color.h"
#include "ukive/graphics/canvas.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/views/button.h"
#include "ukive/graphics/effects/shadow_effect.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/animation/interpolator.h"

#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/display.h"
#include "ukive/graphics/images/lc_image_factory.h"
#include "ukive/graphics/images/lc_image.h"
#include "ukive/resources/resource_manager.h"

#include "ukive/graphics/colors/ucmm.h"
#include "ukive/graphics/colors/color_manager.h"

#define BACKGROUND_SIZE 100


namespace shell {

    ukive::Color color_;

    EffectWindow::EffectWindow()
        {}

    EffectWindow::~EffectWindow() {}

    void EffectWindow::onCreated() {
        Window::onCreated();

        setBackgroundColor(ukive::Color::Green500);

        image_effect_.reset(new ukive::win::ImageEffectGPU(getContext()));
        image_effect_->initialize();
        image_effect_->setSize(400, 400, false);

        using namespace std::chrono_literals;
        animator_.setValueRange(0, 256);
        animator_.setListener(this);
        animator_.setDuration(4000ms);
        animator_.setInterpolator(new ukive::LinearInterpolator());
        //animator_.start();
        //startVSync();

        auto rm = ukive::Application::getResourceManager();
        //auto modified_img_path = rm->getResRootPath() / u"AdobeRGB_2.jpg";
        auto modified_img_path = rm->getResRootPath() / u"wcg_test.png";
        //image_img_ = ukive::ImageFrame::decodeFile(getCanvas(), modified_img_path.u16string());

        ukive::Color src(0.5f, 0.5f, 0.5f, 1);

        ukive::Color dst;
        auto cm = ukive::Application::getColorManager();
        cm->convertColor(src, &dst);

        color_ = dst;

        {
            ukive::Color dst;
            std::unique_ptr<ukive::ColorManager> cm(ukive::ColorManager::create());
            cm->convertColor(ukive::Color(0.25f, 0.89f, 0.47f), &dst);

            std::u16string icc_path;
            if (ukive::ColorManager::getDefaultProfile(&icc_path))
            {
                ukive::icc::ICCProfile pf;
                pf.load(icc_path);

                ukive::Color tc;
                ukive::UCMM cmm;
                cmm.sRGBToTarget(
                    ukive::UCMM::Intent::Perceptual,
                    ukive::Color(0.25f, 0.89f, 0.47f),
                    pf, &tc);

                int sdg = 0;
            }

            image_img_ = ukive::ImageFrame::decodeFile(
                getCanvas(), u"E:\\Test\\HDR.jxr", ukive::ImageOptions(ukive::ImagePixelFormat::HDR));

        }
    }

    void EffectWindow::onPreDrawCanvas(ukive::Canvas* canvas) {
        Window::onPreDrawCanvas(canvas);

        canvas->save();

        //image_effect_->draw(canvas);

        /*canvas->drawLine({ 10, 10.5f }, { 10.5f, 20.5f }, 1, ukive::Color::Black);

        canvas->translate(50.5f, 50.5f);
        canvas->fillCircle({}, 0.4f, ukive::Color::Red500);

        canvas->translate(5.2f, 0.2f);
        canvas->fillCircle({}, 2, ukive::Color::Red500);*/

        canvas->restore();

        canvas->drawImage(100, 10, image_img_.get());

        //canvas->fillRect(ukive::RectF(0, 0, 1000, 1000), color_);
    }

    void EffectWindow::onDestroy() {
        animator_.stop();

        Window::onDestroy();
    }

    bool EffectWindow::onInputEvent(ukive::InputEvent* e) {
        return Window::onInputEvent(e);
    }

    void EffectWindow::onContextChanged(
        ukive::Context::Type type, const ukive::Context& context)
    {
        switch (type) {
        case ukive::Context::DEV_LOST:
        {
            break;
        }

        case ukive::Context::DEV_RESTORE:
        {
            break;
        }

        default:
            break;
        }
    }

    void EffectWindow::onAnimationProgress(ukive::Animator* animator) {
        requestDraw();
    }

    void EffectWindow::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        if (!animator_.update(start_time, display_freq)) {
            stopVSync();
        }
    }

}