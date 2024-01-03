// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shadow_window.h"

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

#include "ukive/graphics/win/effects/gaussian_blur_effect_dx.h"

#define RADIUS 24
#define BACKGROUND_SIZE 100


namespace shell {

    ShadowWindow::ShadowWindow()
        : ce_button_(nullptr) {}

    ShadowWindow::~ShadowWindow() {}

    void ShadowWindow::onCreated() {
        Window::onCreated();

        createShadowImages();

        using Rlp = ukive::RestraintLayoutInfo;

        auto layout = new ukive::RestraintLayout(getContext());
        layout->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_FILL);
        setContentView(layout);

        ce_button_ = new ukive::Button(getContext());
        ce_button_->setText(u"Shadow Effect");
        ce_button_->setHoriAlignment(ukive::TextLayout::Alignment::CENTER);
        ce_button_->setVertAlignment(ukive::TextLayout::Alignment::CENTER);
        ce_button_->setTextSize(getContext().dp2pxi(12));
        ce_button_->setTextWeight(ukive::TextLayout::FontWeight::BOLD);
        ce_button_->setShadowRadius(64);
        ce_button_->setLayoutSize(getContext().dp2pxi(100), getContext().dp2pxi(50));

        auto ce_button_lp = Rlp::Builder()
            .start(layout->getId()).top(layout->getId())
            .end(layout->getId()).bottom(layout->getId()).build();
        ce_button_->setExtraLayoutInfo(ce_button_lp);
        layout->addView(ce_button_);

        using namespace std::chrono_literals;
        animator_.setValueRange(RADIUS, 256);
        animator_.setListener(this);
        animator_.setDuration(4000ms);
        animator_.setInterpolator(new ukive::LinearInterpolator());
        /*animator_.start();
        startVSync();*/
    }

    void ShadowWindow::onPreDrawCanvas(ukive::Canvas* canvas) {
        Window::onPreDrawCanvas(canvas);

        if (shadow_img_) {
            canvas->save();
            canvas->translate(-RADIUS, -RADIUS);
            canvas->drawImage(100, 100, shadow_img_.get());
            canvas->restore();
            canvas->drawImage(100, 100, content_img_.get());
        }

        if (gaussblur_img_) {
            canvas->save();
            canvas->translate(-RADIUS, -RADIUS);
            canvas->drawImage(300, 100, gaussblur_img_.get());
            canvas->restore();
            canvas->drawImage(300, 100, content_img_.get());
        }
    }

    void ShadowWindow::onDestroy() {
        animator_.stop();

        Window::onDestroy();
    }

    bool ShadowWindow::onInputEvent(ukive::InputEvent* e) {
        return Window::onInputEvent(e);
    }

    void ShadowWindow::onContextChanged(
        ukive::Context::Type type, const ukive::Context& context)
    {
        switch (type) {
        case ukive::Context::DEV_LOST:
        {
            shadow_img_ = nullptr;
            content_img_.reset();
            shadow_effect_->destroy();
            shadow_effect_.reset();

            gaussblur_effect_->destroy();
            gaussblur_effect_.reset();
            break;
        }

        case ukive::Context::DEV_RESTORE:
        {
            createShadowImages();
            break;
        }

        default:
            break;
        }
    }

    void ShadowWindow::onAnimationProgress(ukive::Animator* animator) {
        /*shadow_effect_->setRadius(int(animator->getCurValue()));
        shadow_effect_->generate(getCanvas());
        shadow_img_ = shadow_effect_->getOutput();
        requestDraw();*/
    }

    void ShadowWindow::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        if (!animator_.update(start_time, display_freq)) {
            stopVSync();
        }
    }

    void ShadowWindow::createShadowImages() {
        ukive::Canvas canvas(
            BACKGROUND_SIZE, BACKGROUND_SIZE,
            getCanvas()->getBuffer()->getImageOptions());
        canvas.beginDraw();
        canvas.clear();
        canvas.fillRect(ukive::RectF(0, 0, BACKGROUND_SIZE, BACKGROUND_SIZE / 2), ukive::Color::Green400);
        canvas.fillRect(ukive::RectF(0, BACKGROUND_SIZE / 2, BACKGROUND_SIZE, BACKGROUND_SIZE / 2), ukive::Color::Red500);
        //canvas.fillCircle(BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 4.f, ukive::Color::Blue200);
        canvas.endDraw();
        content_img_ = canvas.extractImage();

        bool ret;
        shadow_effect_.reset(ukive::ShadowEffect::create(getContext()));
        ret = shadow_effect_->initialize(); ubassert(ret);
        ret = shadow_effect_->setRadius(RADIUS); ubassert(ret);
        ret = shadow_effect_->setContent(static_cast<ukive::OffscreenBuffer*>(canvas.getBuffer())); ubassert(ret);
        ret = shadow_effect_->generate(getCanvas()); ubassert(ret);
        shadow_img_ = shadow_effect_->getOutput(); ubassert(!!shadow_img_);

        /*gaussblur_effect_.reset(new ukive::win::GaussianBlurEffectGPU(getContext()));
        ret = gaussblur_effect_->initialize(); ubassert(ret);
        ret = gaussblur_effect_->setRadius(RADIUS); ubassert(ret);
        ret = gaussblur_effect_->setContent(static_cast<ukive::OffscreenBuffer*>(canvas.getBuffer())); ubassert(ret);
        ret = gaussblur_effect_->generate(getCanvas()); ubassert(ret);
        gaussblur_img_ = gaussblur_effect_->getOutput(); ubassert(!!gaussblur_img_);*/
    }

}