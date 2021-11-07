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

#include "ukive/graphics/colors/color_manager.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/display.h"
#include "ukive/graphics/images/lc_image_factory.h"
#include "ukive/graphics/images/lc_image.h"
#include "ukive/resources/resource_manager.h"


#define RADIUS 4
#define BACKGROUND_SIZE 100


namespace shell {

    ukive::Color color_;

    EffectWindow::EffectWindow()
        : ce_button_(nullptr) {}

    EffectWindow::~EffectWindow() {}

    void EffectWindow::onCreated() {
        Window::onCreated();

        setBackgroundColor(ukive::Color::White);

        createImages();

        /*using Rlp = ukive::RestraintLayoutInfo;

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
        layout->addView(ce_button_);*/

        using namespace std::chrono_literals;
        animator_.setValueRange(RADIUS, 256);
        animator_.setListener(this);
        animator_.setDuration(4000ms);
        animator_.setInterpolator(new ukive::LinearInterpolator());
        //animator_.start();
        //startVSync();

        auto rm = ukive::Application::getResourceManager();
        //auto modified_img_path = rm->getResRootPath() / u"AdobeRGB_2.jpg";
        auto modified_img_path = rm->getResRootPath() / u"wcg_test.png";
        image_img_ = ukive::ImageFrame::decodeFile(this, modified_img_path.u16string());

        ukive::Color src(0.5f, 0.5f, 0.5f, 1);

        ukive::Color dst;
        auto cm = ukive::Application::getColorManager();
        cm->convertColor(src, &dst);

        color_ = dst;
    }

    void EffectWindow::onPreDrawCanvas(ukive::Canvas* canvas) {
        Window::onPreDrawCanvas(canvas);

        /*canvas->save();
        canvas->translate(-RADIUS, -RADIUS);

        canvas->drawImage(100, 10, shadow_img_);

        canvas->restore();*/

        //canvas->drawImage(100, 10, content_img_.get());
        //canvas->drawImage(100, 10, image_img_);

        canvas->fillRect(ukive::RectF(0, 0, 1000, 1000), color_);
    }

    void EffectWindow::onDestroy() {
        animator_.stop();

        Window::onDestroy();
    }

    bool EffectWindow::onInputEvent(ukive::InputEvent* e) {
        return Window::onInputEvent(e);
    }

    void EffectWindow::onContextChanged(const ukive::Context& context) {
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

    void EffectWindow::onAnimationProgress(ukive::Animator* animator) {
        shadow_effect_->setRadius(int(animator->getCurValue()));
        shadow_effect_->generate(getCanvas());

        shadow_img_ = shadow_effect_->getOutput();
        requestDraw();
    }

    void EffectWindow::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        if (!animator_.update(start_time, display_freq)) {
            stopVSync();
        }
    }

    void EffectWindow::createImages() {
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