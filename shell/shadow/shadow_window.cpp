// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shadow_window.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/canvas.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/views/button.h"
#include "ukive/graphics/effects/shadow_effect.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/animation/interpolator.h"

#define RADIUS 4
#define BACKGROUND_SIZE 100


namespace shell {

    ShadowWindow::ShadowWindow()
        : ce_button_(nullptr) {}

    ShadowWindow::~ShadowWindow() {}

    void ShadowWindow::onCreated() {
        Window::onCreated();

        setBackgroundColor(ukive::Color::White);

        createImages();

        using Rlp = ukive::RestraintLayoutInfo;

        auto layout = new ukive::RestraintLayout(getContext());
        layout->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_FILL);
        setContentView(layout);

        ce_button_ = new ukive::Button(getContext());
        ce_button_->setText(u"Shadow Effect");
        ce_button_->setTextAlignment(ukive::TextLayout::Alignment::CENTER);
        ce_button_->setParagraphAlignment(ukive::TextLayout::Alignment::CENTER);
        ce_button_->setTextSize(getContext().dp2pxi(12));
        ce_button_->setTextWeight(ukive::TextLayout::FontWeight::BOLD);
        ce_button_->setShadowRadius(18);
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
        //animator_.start();
        //startVSync();
    }

    void ShadowWindow::onPreDrawCanvas(ukive::Canvas* canvas) {
        Window::onPreDrawCanvas(canvas);

        canvas->save();
        canvas->translate(-RADIUS, -RADIUS);

        canvas->drawImage(100, 10, shadow_img_);

        canvas->restore();

        canvas->drawImage(100, 10, content_img_.get());
    }

    void ShadowWindow::onDestroy() {
        animator_.stop();

        Window::onDestroy();
    }

    bool ShadowWindow::onInputEvent(ukive::InputEvent* e) {
        return Window::onInputEvent(e);
    }

    void ShadowWindow::onContextChanged(const ukive::Context& context) {
        switch (context.getChanged()) {
        case ukive::Context::DEV_LOST:
        {
            shadow_img_ = nullptr;
            content_img_.reset();
            shadow_effect_->destroy();
            shadow_effect_.reset();
            break;
        }

        case ukive::Context::DEV_RESTORE:
        {
            createImages();
            break;
        }

        default:
            break;
        }
    }

    void ShadowWindow::onAnimationProgress(ukive::Animator* animator) {
        shadow_effect_->setRadius(int(animator->getCurValue()));
        shadow_effect_->generate(getCanvas());

        shadow_img_ = shadow_effect_->getOutput();
        requestDraw();
    }

    void ShadowWindow::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        animator_.update(start_time, display_freq);

        if (animator_.isRunning()) {
            requestVSync();
        } else {
            stopVSync();
        }
    }

    void ShadowWindow::createImages() {
        ukive::Canvas canvas(
            BACKGROUND_SIZE, BACKGROUND_SIZE,
            getCanvas()->getBuffer()->getImageOptions());
        canvas.beginDraw();
        canvas.clear();
        canvas.fillRect(ukive::RectF(0, 0, BACKGROUND_SIZE, BACKGROUND_SIZE), ukive::Color::Green400);
        //canvas.fillCircle(BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 4.f, ukive::Color::Blue200);
        canvas.endDraw();
        content_img_ = std::shared_ptr<ukive::ImageFrame>(canvas.extractImage());

        shadow_effect_.reset(ukive::ShadowEffect::create(getContext()));
        shadow_effect_->initialize();
        shadow_effect_->setRadius(RADIUS);
        shadow_effect_->setContent(static_cast<ukive::OffscreenBuffer*>(canvas.getBuffer()));

        shadow_effect_->generate(getCanvas());

        shadow_img_ = shadow_effect_->getOutput();
    }

}