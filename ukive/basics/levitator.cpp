// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "levitator.h"

#include "ukive/diagnostic/input_tracker.h"
#include "ukive/event/input_event.h"
#include "ukive/views/layout/root_layout.h"
#include "ukive/window/window.h"


namespace ukive {

    void translatePosInfo(
        ShadeParams& params, const Levitator::PosInfo& info)
    {
        params.gravity = info.corner;
        params.pos_padding = info.pp;
        params.is_max_visible = info.is_max_visible;
        params.is_evaded = info.is_evaded;
        params.limit.set(info.limit_range);
        params.limit.set(info.limit_view);
        params.relative = info.rel_view;
    }

    void translateSnapInfo(
        ShadeParams& params, const Levitator::SnapInfo& info)
    {
        params.is_max_visible = info.is_max_visible;
        params.is_discretized = info.is_discretized;
    }

}

namespace ukive {

    Levitator::Levitator()
        : width_(View::LS_AUTO),
          height_(View::LS_AUTO),
          shadow_radius_(0),
          outside_touchable_(false),
          dismiss_by_touch_outside_(false),
          background_(nullptr),
          content_view_(nullptr),
          frame_view_(nullptr),
          is_showing_(false)
    {}

    Levitator::~Levitator() {
        if (frame_view_) {
            if (is_showing_) {
                dismiss();
            }
            delete frame_view_;
        } else {
            if (background_) {
                delete background_;
            }
            delete content_view_;
        }
    }

    void Levitator::createFrameView(Context c) {
        frame_view_ = new LevFrameView(this, c);
        frame_view_->setShadowRadius(shadow_radius_);
        frame_view_->setBackground(background_, false);
        frame_view_->setReceiveOutsideInputEvent(!outside_touchable_);
        frame_view_->setLayoutMargin(
            margin_.start(), margin_.top(), margin_.end(), margin_.bottom());
        frame_view_->addView(content_view_);
    }

    void Levitator::leaveFromInside() {
        if (!frame_view_ || !window_) {
            return;
        }

        bool hit = false;
        auto liv = window_->getLastInputView();
        View* cur = liv;
        while (cur) {
            if (cur == frame_view_) {
                hit = true;
                break;
            }
            cur = cur->getParent();
        }

        if (!hit) return;

        InputEvent ev;
        ev.setEvent(InputEvent::EV_LEAVE);
        ev.setIsNoDispatch(true);
        INPUT_TRACK_PRINT("Levitator", &e);
        if (liv) {
            liv->dispatchInputEvent(&ev);
        }
        window_->setLastInputView(nullptr);
    }

    void Levitator::leaveFromOutside() {
        if (!frame_view_ || !window_) {
            return;
        }

        bool hit = false;
        auto liv = window_->getLastInputView();
        View* cur = liv;
        while (cur) {
            if (cur == frame_view_) {
                hit = true;
                break;
            }
            cur = cur->getParent();
        }

        if (hit) return;

        InputEvent ev;
        ev.setEvent(InputEvent::EV_LEAVE);
        ev.setIsNoDispatch(true);
        INPUT_TRACK_PRINT("Levitator", &e);
        if (liv) {
            liv->dispatchInputEvent(&ev);
        }
        window_->setLastInputView(nullptr);
    }

    void Levitator::setLayoutWidth(int width) {
        width_ = width;
        if (frame_view_) {
            auto& size = frame_view_->getLayoutSize();
            frame_view_->setLayoutSize(width_, size.height());
        }
    }

    void Levitator::setLayoutHeight(int height) {
        height_ = height;
        if (frame_view_) {
            auto& size = frame_view_->getLayoutSize();
            frame_view_->setLayoutSize(size.width(), height_);
        }
    }

    void Levitator::setLayoutSize(int width, int height) {
        width_ = width;
        height_ = height;
        if (frame_view_) {
            frame_view_->setLayoutSize(width_, height_);
        }
    }

    void Levitator::setMinimumSize(int min_w, int min_h) {
        min_width_ = min_w;
        min_height_ = min_h;
        if (frame_view_) {
            frame_view_->setMinimumWidth(width_);
            frame_view_->setMinimumHeight(height_);
        }
    }

    void Levitator::setLayoutMargin(const Margin& margin) {
        margin_ = margin;
        if (frame_view_) {
            frame_view_->setLayoutMargin(
                margin.start(), margin.top(), margin.end(), margin.bottom());
        }
    }

    void Levitator::setShadowRadius(int radius) {
        shadow_radius_ = radius;
        if (frame_view_) {
            frame_view_->setShadowRadius(shadow_radius_);
        }
    }

    void Levitator::setBackground(Element* element) {
        if (frame_view_) {
            frame_view_->setBackground(element, false);
        }

        if (background_) {
            delete background_;
        }
        background_ = element;
    }

    void Levitator::setOutsideTouchable(bool touchable) {
        outside_touchable_ = touchable;
        if (frame_view_) {
            frame_view_->setReceiveOutsideInputEvent(touchable);
            leaveFromOutside();
        }
    }

    void Levitator::setInputEnabled(bool enabled) {
        if (enabled == input_enabled_) {
            return;
        }

        input_enabled_ = enabled;
        if (!enabled && !is_dismissing_) {
            leaveFromInside();
        }
    }

    void Levitator::setDismissByTouchOutside(bool enable) {
        dismiss_by_touch_outside_ = enable;
    }

    void Levitator::setContentView(View* v) {
        if (!v) {
            return;
        }

        if (content_view_ && content_view_ != v) {
            if (frame_view_) {
                frame_view_->removeAllViews();
                frame_view_->addView(v);
            } else {
                delete content_view_;
            }
        }

        content_view_ = v;
    }

    void Levitator::setEventListener(OnLeviatorEventListener* l) {
        listener_ = l;
    }

    int Levitator::getLayoutWidth() const {
        return width_;
    }

    int Levitator::getLayoutHeight() const {
        return height_;
    }

    int Levitator::getMinimumWidth() const {
        return min_width_;
    }

    int Levitator::getMinimumHeight() const {
        return min_height_;
    }

    int Levitator::getShadowRadius() const {
        return shadow_radius_;
    }

    Element* Levitator::getBackground() const {
        return background_;
    }

    bool Levitator::isOutsideTouchable() const {
        return outside_touchable_;
    }

    bool Levitator::isDismissByTouchOutside() const {
        return dismiss_by_touch_outside_;
    }

    bool Levitator::isInputEnabled() const {
        return input_enabled_;
    }

    View* Levitator::getContentView() const {
        return content_view_;
    }

    View* Levitator::getFrameView() const {
        return frame_view_;
    }

    bool Levitator::isShowing() const {
        return is_showing_;
    }

    bool Levitator::isDismissing() const {
        return is_dismissing_;
    }

    bool Levitator::show(Window* w, int x, int y, const PosInfo& info) {
        if (!content_view_ || !w) {
            return false;
        }
        if (is_showing_) {
            if (!update(x, y)) {
                return false;
            }
            is_dismissing_ = false;
            return true;
        }

        if (frame_view_) {
            frame_view_->removeAllViews(false);
            delete frame_view_;
        }

        auto c = w->getContext();

        createFrameView(c);

        is_dismissing_ = false;

        frame_view_->setLayoutSize(width_, height_);
        frame_view_->setMinimumWidth(min_width_);
        frame_view_->setMinimumHeight(min_height_);

        ShadeParams params;
        params.type = ShadeParams::LT_POS;
        params.pos = { x, y };
        translatePosInfo(params, info);
        w->getRootLayout()->addShade(frame_view_, params);

        window_ = w;
        is_showing_ = true;
        if (!outside_touchable_) {
            leaveFromOutside();
        }

        return true;
    }

    bool Levitator::show(View* anchor, int gravity, const SnapInfo& info) {
        if (!content_view_ || !anchor) {
            return false;
        }

        if (is_showing_) {
            if (!update(anchor, gravity)) {
                return false;
            }
            is_dismissing_ = false;
            return true;
        }

        if (frame_view_) {
            frame_view_->removeAllViews(false);
            delete frame_view_;
        }

        auto w = anchor->getWindow();
        if (!w) {
            return false;
        }

        auto c = w->getContext();
        createFrameView(c);

        is_dismissing_ = false;

        frame_view_->setLayoutSize(width_, height_);
        frame_view_->setMinimumWidth(min_width_);
        frame_view_->setMinimumHeight(min_height_);

        ShadeParams params;
        params.type = ShadeParams::LT_SNAP;
        params.gravity = gravity;
        params.anchor.set(anchor->getBoundsInRoot());
        params.anchor.set(anchor);
        translateSnapInfo(params, info);
        w->getRootLayout()->addShade(frame_view_, params);

        window_ = w;
        is_showing_ = true;
        if (!outside_touchable_) {
            leaveFromOutside();
        }

        return true;
    }

    bool Levitator::update(int x, int y) {
        if (!frame_view_ || !is_showing_ || !window_) {
            return false;
        }
        auto c = frame_view_->getContext();

        ShadeParams params;
        window_->getRootLayout()->getShadeParams(frame_view_, &params);
        if (params.type != ShadeParams::LT_POS) {
            params.type = ShadeParams::LT_POS;
            translatePosInfo(params, {});
        }

        params.pos = { x, y };
        return window_->getRootLayout()->updateShade(frame_view_, params);
    }

    bool Levitator::update(int x, int y, const PosInfo& info) {
        if (!frame_view_ || !is_showing_ || !window_) {
            return false;
        }
        auto c = frame_view_->getContext();

        ShadeParams params;
        window_->getRootLayout()->getShadeParams(frame_view_, &params);
        params.type = ShadeParams::LT_POS;
        params.pos = { x, y };
        translatePosInfo(params, info);
        return window_->getRootLayout()->updateShade(frame_view_, params);
    }

    bool Levitator::update(View* anchor, int gravity) {
        if (!frame_view_ || !is_showing_ || !anchor) {
            return false;
        }

        auto w = anchor->getWindow();
        if (!w) {
            return false;
        }

        ShadeParams params;
        w->getRootLayout()->getShadeParams(frame_view_, &params);
        if (params.type != ShadeParams::LT_SNAP) {
            params.type = ShadeParams::LT_SNAP;
            translateSnapInfo(params, {});
        }
        params.gravity = gravity;
        params.anchor.set(anchor->getBoundsInRoot());
        params.anchor.set(anchor);
        return w->getRootLayout()->updateShade(frame_view_, params);
    }

    bool Levitator::update(View* anchor, int gravity, const SnapInfo& info) {
        if (!frame_view_ || !is_showing_ || !anchor) {
            return false;
        }

        auto w = anchor->getWindow();
        if (!w) {
            return false;
        }

        ShadeParams params;
        w->getRootLayout()->getShadeParams(frame_view_, &params);
        params.type = ShadeParams::LT_SNAP;
        params.gravity = gravity;
        params.anchor.set(anchor->getBoundsInRoot());
        params.anchor.set(anchor);
        translateSnapInfo(params, info);
        return w->getRootLayout()->updateShade(frame_view_, params);
    }

    void Levitator::dismissing() {
        is_dismissing_ = true;
    }

    void Levitator::dismiss() {
        if (frame_view_ && is_showing_) {
            auto root_layout = window_->getRootLayout();
            if (root_layout) {
                root_layout->removeShade(frame_view_);
            }
        }
        is_showing_ = false;
        window_ = nullptr;
    }

    Levitator::LevFrameView::LevFrameView(Levitator* lev, Context c)
        : SimpleLayout(c),
          levitator_(lev) {}

    Levitator::LevFrameView::~LevFrameView() {
    }

    Size Levitator::LevFrameView::onDetermineSize(const SizeInfo& info) {
        return super::onDetermineSize(info);
    }

    void Levitator::LevFrameView::onBeforeLayout(Rect* new_bounds, const Rect& old_bounds) {
        if (levitator_->listener_) {
            levitator_->listener_->onBeforeInnerWindowLayout(
                levitator_, new_bounds, old_bounds);
        }
    }

    void Levitator::LevFrameView::onLayout(const Rect& new_bounds, const Rect& old_bounds) {
        auto bounds = getContentBounds();

        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                const auto& size = child->getDeterminedSize();
                const auto& margin = child->getLayoutMargin();

                auto new_size = size;
                new_size.same(Size(bounds.width() - margin.hori(), bounds.height() - margin.vert()));
                new_size.join(Size(0, 0));

                int child_left = getPadding().start() + margin.start();
                int child_top = getPadding().top() + margin.top();

                child->layout(
                    Rect(child_left, child_top, new_size.width(), new_size.height()));
            }
        }
    }

    bool Levitator::LevFrameView::dispatchInputEvent(InputEvent* e) {
        if (!levitator_->input_enabled_) {
            return false;
        }

        // Levitator 模拟一个独立的窗口，未消费的事件不应该继续传递。
        SimpleLayout::dispatchInputEvent(e);

        // 对于触摸事件，直接在此消费掉比较自然
        if (e->isTouchEvent()) {
            return true;
        }
        return !levitator_->is_dismissing_;
    }

    bool Levitator::LevFrameView::onHookInputEvent(InputEvent* e) {
        if (e->isOutside()) {
            return !levitator_->outside_touchable_;
        }
        return false;
    }

    bool Levitator::LevFrameView::onInputEvent(InputEvent* e) {
        if (e->isOutside()) {
            if (e->getEvent() == InputEvent::EVM_DOWN ||
                e->getEvent() == InputEvent::EVM_UP ||
                e->getEvent() == InputEvent::EVT_DOWN ||
                e->getEvent() == InputEvent::EVT_UP)
            {
                if (levitator_->dismiss_by_touch_outside_) {
                    if (levitator_->listener_) {
                        levitator_->listener_->onRequestDismissByTouchOutside(levitator_);
                    }
                    return false;
                }
                return true;
            }
            return true;
        }

        invalidateHookStatus();
        return false;
    }

}