// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "inner_window.h"

#include "ukive/event/input_event.h"
#include "ukive/views/layout/root_layout.h"
#include "ukive/window/window.h"


namespace ukive {

    InnerWindow::InnerWindow()
        : width_(View::LS_AUTO),
          height_(View::LS_AUTO),
          shadow_radius_(0),
          outside_touchable_(false),
          dismiss_by_touch_outside_(false),
          background_(nullptr),
          content_view_(nullptr),
          decor_view_(nullptr),
          is_showing_(false)
    {
    }

    InnerWindow::~InnerWindow() {
        if (decor_view_) {
            if (is_showing_) {
                dismiss();
            }
            delete decor_view_;
        } else {
            if (background_) {
                delete background_;
            }
            delete content_view_;
        }
    }

    void InnerWindow::createDecorView(Context c) {
        decor_view_ = new InnerDecorView(this, c);
        decor_view_->setShadowRadius(shadow_radius_);
        decor_view_->setBackground(background_, false);
        decor_view_->setReceiveOutsideInputEvent(!outside_touchable_);

        content_view_->setLayoutSize(width_, height_);
        decor_view_->addView(content_view_);
    }

    void InnerWindow::setWidth(int width) {
        width_ = width;
        if (decor_view_) {
            auto& size = decor_view_->getLayoutSize();
            decor_view_->setLayoutSize(width_, size.height);
        }
    }

    void InnerWindow::setHeight(int height) {
        height_ = height;
        if (decor_view_) {
            auto& size = decor_view_->getLayoutSize();
            decor_view_->setLayoutSize(size.width, height_);
        }
    }

    void InnerWindow::setSize(int width, int height) {
        width_ = width;
        height_ = height;
        if (decor_view_) {
            decor_view_->setLayoutSize(width_, height_);
        }
    }

    void InnerWindow::setShadowRadius(int radius) {
        shadow_radius_ = radius;
        if (decor_view_) {
            decor_view_->setShadowRadius(shadow_radius_);
        }
    }

    void InnerWindow::setBackground(Element* element) {
        if (decor_view_) {
            decor_view_->setBackground(element, false);
        }

        if (background_) {
            delete background_;
        }
        background_ = element;
    }

    void InnerWindow::setOutsideTouchable(bool touchable) {
        outside_touchable_ = touchable;
    }

    void InnerWindow::setDismissByTouchOutside(bool enable) {
        dismiss_by_touch_outside_ = enable;
    }

    void InnerWindow::setContentView(View* v) {
        if (!v) {
            return;
        }
        content_view_ = v;
    }

    void InnerWindow::setEventListener(OnInnerWindowEventListener* l) {
        listener_ = l;
    }

    int InnerWindow::getWidth() const {
        return width_;
    }

    int InnerWindow::getHeight() const {
        return height_;
    }

    int InnerWindow::getShadowRadius() const {
        return shadow_radius_;
    }

    Element* InnerWindow::getBackground() const {
        return background_;
    }

    bool InnerWindow::isOutsideTouchable() const {
        return outside_touchable_;
    }

    bool InnerWindow::isDismissByTouchOutside() const {
        return dismiss_by_touch_outside_;
    }

    View* InnerWindow::getContentView() const {
        return content_view_;
    }

    View* InnerWindow::getDecorView() const {
        return decor_view_;
    }

    bool InnerWindow::isShowing() const {
        return is_showing_;
    }

    void InnerWindow::show(Window* w, int x, int y) {
        if (!content_view_) {
            return;
        }
        if (is_showing_) {
            update(x, y);
            return;
        }

        if (decor_view_) {
            decor_view_->removeAllViews(false);
            delete decor_view_;
        }

        auto c = w->getContext();

        createDecorView(c);

        is_marked_as_dismissing_ = false;

        decor_view_->setLayoutSize(width_, height_);
        decor_view_->setLayoutMargin(x, y, c.dp2pxi(8), c.dp2pxi(8));

        w->getRootLayout()->addShade(decor_view_, {}, GV_NONE);

        window_ = w;
        is_showing_ = true;
    }

    void InnerWindow::show(View* anchor, int gravity) {
        if (!content_view_ || !anchor) {
            return;
        }

        if (is_showing_) {
            update(anchor, gravity);
            return;
        }

        if (decor_view_) {
            decor_view_->removeAllViews(false);
            delete decor_view_;
        }

        auto w = anchor->getWindow();
        if (!w) {
            return;
        }

        auto c = w->getContext();
        createDecorView(c);

        is_marked_as_dismissing_ = false;

        decor_view_->setLayoutSize(width_, height_);
        decor_view_->setLayoutMargin(
            c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8));

        w->getRootLayout()->addShade(
            decor_view_, anchor->getBoundsInRoot(), gravity);

        window_ = w;
        is_showing_ = true;
    }

    void InnerWindow::update(int x, int y) {
        if (!decor_view_ || !is_showing_) {
            return;
        }
        auto c = decor_view_->getContext();
        decor_view_->setLayoutMargin(x, y, c.dp2pxi(8), c.dp2pxi(8));
    }

    void InnerWindow::update(View* anchor, int gravity) {
        if (!decor_view_ || !is_showing_ || !anchor) {
            return;
        }

        auto w = anchor->getWindow();
        if (!w) {
            return;
        }

        w->getRootLayout()->updateShade(
            decor_view_, anchor->getBoundsInRoot(), gravity);
    }

    void InnerWindow::markDismissing() {
        is_marked_as_dismissing_ = true;
    }

    void InnerWindow::dismiss() {
        if (decor_view_ && is_showing_) {
            window_->getRootLayout()->removeShade(decor_view_);
        }
        is_showing_ = false;
        window_ = nullptr;
    }

    InnerWindow::InnerDecorView::InnerDecorView(InnerWindow* inner, Context c)
        : SimpleLayout(c),
          inner_window_(inner) {}

    InnerWindow::InnerDecorView::~InnerDecorView() {
    }

    Size InnerWindow::InnerDecorView::onDetermineSize(const SizeInfo& info) {
        return super::onDetermineSize(info);
    }

    void InnerWindow::InnerDecorView::onBeforeLayout(Rect* new_bounds, const Rect& old_bounds) {
        if (inner_window_->listener_) {
            inner_window_->listener_->onBeforeInnerWindowLayout(
                inner_window_, new_bounds, old_bounds);
        }
    }

    bool InnerWindow::InnerDecorView::dispatchInputEvent(InputEvent* e) {
        // InnerWindow 模拟一个独立的窗口，未消费的事件不应该继续传递。
        SimpleLayout::dispatchInputEvent(e);

        // 对于触摸事件，直接在此消费掉比较自然
        if (e->isTouchEvent()) {
            return true;
        }
        return !inner_window_->is_marked_as_dismissing_;
    }

    bool InnerWindow::InnerDecorView::onHookInputEvent(InputEvent* e) {
        if (e->isOutside()) {
            return !inner_window_->outside_touchable_;
        }
        return false;
    }

    bool InnerWindow::InnerDecorView::onInputEvent(InputEvent* e) {
        if (e->isOutside()) {
            if (e->getEvent() == InputEvent::EVM_DOWN ||
                e->getEvent() == InputEvent::EVM_UP ||
                e->getEvent() == InputEvent::EVT_DOWN ||
                e->getEvent() == InputEvent::EVT_UP)
            {
                if (inner_window_->dismiss_by_touch_outside_) {
                    if (inner_window_->listener_) {
                        inner_window_->listener_->onRequestDismissByTouchOutside(inner_window_);
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