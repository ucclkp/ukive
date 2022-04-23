// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "view.h"

#include <cmath>

#include "utils/log.h"
#include "utils/strings/int_conv.hpp"
#include "utils/time_utils.h"
#include "utils/weak_bind.hpp"

#include "ukive/diagnostic/ui_tracker.h"
#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/animation/view_animator.h"
#include "ukive/text/input_method_connection.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/input_event_delegate.h"
#include "ukive/views/layout_info/layout_info.h"
#include "ukive/views/layout/layout_view.h"
#include "ukive/window/window.h"
#include "ukive/app/application.h"
#include "ukive/graphics/effects/shadow_effect.h"
#include "ukive/resources/dimension_utils.h"
#include "ukive/system/ui_utils.h"

#include "necro/layout_constants.h"


namespace ukive {

    View::View(Context c)
        : View(c, {}) {}

    View::View(Context c, AttrsRef attrs)
        : cur_ev_(std::make_shared<InputEvent>()),
          outline_(OUTLINE_RECT),
          scroll_x_(0),
          scroll_y_(0),
          visibility_(SHOW),
          shadow_radius_(0),
          has_focus_(false),
          is_enabled_(true),
          is_attached_to_window_(false),
          is_pressed_(false),
          is_focusable_(false),
          is_receive_outside_input_event_(false),
          is_mouse_down_(false),
          is_touch_down_(false),
          context_(c),
          parent_(nullptr),
          input_connection_(nullptr)
    {
        bool has_id = false;
        auto it = attrs.find(necro::kAttrViewId);
        if (it != attrs.end()) {
            if (!utl::stoi(it->second, &id_)) {
                LOG(Log::WARNING) << "Cannot convert View id: " << it->second;
            } else {
                has_id = true;
            }
        }
        if (!has_id) {
            id_ = Application::getViewID();
        }

        resolveAttrLayoutSize(c, attrs, &layout_size_);
        resolveAttrMargin(c, attrs, necro::kAttrLayoutMargin, &layout_margin_);
        resolveAttrPadding(c, attrs, necro::kAttrViewPadding, &padding_);

        int shadow_radius = int(resolveAttrDimension(c, attrs, necro::kAttrViewShadowRadius, 0));
        setShadowRadius(shadow_radius);

        setClickable(resolveAttrBool(attrs, necro::kAttrViewClickable, false));
    }

    View::~View() {
        if (input_connection_) {
            input_connection_->popEditor();
            delete input_connection_;
        }

        resetBackground();
        resetForeground();
    }

    ViewAnimator* View::animate() {
        if (!animator_) {
            animator_ = std::make_unique<ViewAnimator>(this);
        }

        return animator_.get();
    }

    ViewAnimatorParams* View::animeParams() {
        return &anime_params_;
    }

    void View::setId(int id) {
        id_ = id;
    }

    void View::setTag(int tag) {
        tag_ = tag;
    }

    void View::setCategory(int category) {
        category_ = category;
    }

    void View::setScrollX(int x) {
        if (scroll_x_ != x) {
            scroll_x_ = x;
            requestDraw();
        }
    }

    void View::setScrollY(int y) {
        if (scroll_y_ != y) {
            scroll_y_ = y;
            requestDraw();
        }
    }

    void View::setVisibility(int visibility) {
        if (visibility == visibility_) {
            return;
        }

        bool need_layout =
            visibility == VANISHED || visibility_ == VANISHED;

        visibility_ = visibility;

        if (visibility_ != SHOW) {
            discardFocus();
            discardMouseCapture();
            discardTouchCapture();
            discardPendingOperations();
            resetLastInputView();
        }

        if (need_layout) {
            requestLayout();
        }
        requestDraw();
        onVisibilityChanged(visibility);
        dispatchAncestorVisibilityChanged(this, visibility);
    }

    void View::setEnabled(bool enable) {
        if (enable == is_enabled_) {
            return;
        }

        is_enabled_ = enable;

        if (!is_enabled_) {
            discardFocus();
            discardMouseCapture();
            discardTouchCapture();
            discardPendingOperations();
            resetLastInputView();
        }

        updateBackgroundState();
        updateForegroundState();
        requestDraw();

        onEnableChanged(enable);
        dispatchAncestorEnableChanged(this, enable);
    }

    void View::setBackground(Element* element, bool owned) {
        if (bg_element_ == element) {
            owned_bg_ = owned;
            return;
        }

        ubassert(!element || !element->isAttachedToWindow());
        if (bg_element_) {
            if (isAttachedToWindow()) {
                ubassert(bg_element_->isAttachedToWindow());
                bg_element_->notifyDetachedFromWindow();
            } else {
                ubassert(!bg_element_->isAttachedToWindow());
            }
        }

        resetBackground();

        bg_element_ = element;
        if (bg_element_) {
            bg_element_->setCallback(this);
        }
        owned_bg_ = owned;

        if (bg_element_) {
            if (isAttachedToWindow()) {
                ubassert(getWindow());
                ubassert(!bg_element_->isAttachedToWindow());
                bg_element_->notifyAttachedToWindow(getWindow());
            } else {
                ubassert(!bg_element_->isAttachedToWindow());
            }
        }

        updateBackgroundState();
        requestDraw();
    }

    void View::setForeground(Element* element, bool owned) {
        if (fg_element_ == element) {
            owned_fg_ = owned;
            return;
        }

        ubassert(!element || !element->isAttachedToWindow());
        if (fg_element_) {
            if (isAttachedToWindow()) {
                ubassert(fg_element_->isAttachedToWindow());
                fg_element_->notifyDetachedFromWindow();
            } else {
                ubassert(!fg_element_->isAttachedToWindow());
            }
        }

        resetForeground();

        fg_element_ = element;
        if (fg_element_) {
            fg_element_->setCallback(this);
        }
        owned_fg_ = owned;

        if (fg_element_) {
            if (isAttachedToWindow()) {
                ubassert(getWindow());
                ubassert(!fg_element_->isAttachedToWindow());
                fg_element_->notifyAttachedToWindow(getWindow());
            } else {
                ubassert(!fg_element_->isAttachedToWindow());
            }
        }

        updateForegroundState();
        requestDraw();
    }

    void View::setPadding(int lead, int top, int trail, int bottom) {
        Padding new_padding(lead, top, trail, bottom);
        if (padding_ == new_padding) {
            return;
        }

        padding_ = new_padding;

        requestLayout();
        requestDraw();
    }

    void View::setPressed(bool pressed) {
        if (is_pressed_ == pressed) {
            return;
        }

        is_pressed_ = pressed;
        requestDraw();
    }

    void View::setCurrentCursor(Cursor cursor) {
        window_->setCurrentCursor(cursor);
    }

    void View::setClickable(bool clickable) {
        if (is_clickable_ != clickable) {
            is_clickable_ = clickable;
        }
    }

    void View::setDoubleClickable(bool dbclkable) {
        if (is_dbclkable_ != dbclkable) {
            is_dbclkable_ = dbclkable;
            if (!dbclkable) {
                wait_for_dbclk_ = false;
            }
        }
    }

    void View::setFocusable(bool focusable) {
        if (is_focusable_ == focusable) {
            return;
        }

        is_focusable_ = focusable;

        if (!focusable) {
            discardFocus();
        }
    }

    void View::setTouchCapturable(bool capturable) {
        if (is_touch_capturable_ == capturable) {
            return;
        }

        is_touch_capturable_ = capturable;
    }

    void View::setShadowRadius(int radius) {
        if (radius == shadow_radius_) {
            return;
        }

        if (radius > 0.f) {
            if (!shadow_effect_) {
                shadow_effect_.reset(ShadowEffect::create(getContext()));
                shadow_effect_->initialize();
            }
            shadow_effect_->setRadius(radius * 2);
        } else {
            shadow_effect_.reset();
        }

        shadow_radius_ = radius;

        requestLayout();
        requestDraw();
    }

    void View::setReceiveOutsideInputEvent(bool receive) {
        is_receive_outside_input_event_ = receive;
    }

    void View::setParent(LayoutView* parent) {
        parent_ = parent;
    }

    Size View::getPreferredSize(
        const SizeInfo& info, int pref_width, int pref_height)
    {
        int final_width, final_height;

        switch (info.width.mode) {
        case SizeInfo::CONTENT:
            final_width = (std::min)(info.width.val, pref_width + padding_.hori());
            break;

        case SizeInfo::DEFINED:
            final_width = info.width.val;
            break;

        case SizeInfo::FREEDOM:
        default:
            final_width = pref_width + padding_.hori();
            break;
        }

        switch (info.height.mode) {
        case SizeInfo::CONTENT:
            final_height = (std::min)(info.height.val, pref_height + padding_.vert());
            break;

        case SizeInfo::DEFINED:
            final_height = info.height.val;
            break;

        case SizeInfo::FREEDOM:
        default:
            final_height = pref_height + padding_.vert();
            break;
        }

        return Size(final_width, final_height);
    }

    void View::offsetVertical(int dy) {
        bounds_.top += dy;
        bounds_.bottom += dy;

        requestDraw();
    }

    void View::offsetHorizontal(int dx) {
        bounds_.left += dx;
        bounds_.right += dx;

        requestDraw();
    }

    void View::setMinimumWidth(int width) {
        min_size_.width = width;
    }

    void View::setMinimumHeight(int height) {
        min_size_.height = height;
    }

    void View::setOnClickListener(OnClickListener* l) {
        click_listener_ = l;
    }

    void View::setOnInputEventDelegate(OnInputEventDelegate* d) {
        ie_delegate_ = d;
    }

    void View::setOutline(Outline outline) {
        if (outline == outline_) {
            return;
        }

        outline_ = outline;
    }

    void View::setLayoutSize(int width, int height) {
        bool changed = layout_size_.width != width ||
            layout_size_.height != height;
        if (!changed) {
            return;
        }

        layout_size_.set(width, height);

        requestLayout();
        requestDraw();
    }

    void View::setLayoutMargin(int start, int top, int end, int bottom) {
        bool changed = layout_margin_.start != start ||
            layout_margin_.top != top ||
            layout_margin_.end != end ||
            layout_margin_.bottom != bottom;
        if (!changed) {
            return;
        }

        layout_margin_.set(start, top, end, bottom);

        requestLayout();
        requestDraw();
    }

    void View::setExtraLayoutInfo(LayoutInfo* info) {
        if (layout_info_.get() != info) {
            layout_info_.reset(info);
        }

        requestLayout();
        requestDraw();
    }

    int View::getId() const {
        return id_;
    }

    int View::getTag() const {
        return tag_;
    }

    int View::getCategory() const {
        return category_;
    }

    int View::getScrollX() const {
        return scroll_x_;
    }

    int View::getScrollY() const {
        return scroll_y_;
    }

    int View::getLeft() const {
        return bounds_.left;
    }

    int View::getTop() const {
        return bounds_.top;
    }

    int View::getRight() const {
        return bounds_.right;
    }

    int View::getBottom() const {
        return bounds_.bottom;
    }

    int View::getWidth() const {
        return bounds_.width();
    }

    int View::getHeight() const {
        return bounds_.height();
    }

    int View::getShadowRadius() const {
        return shadow_radius_;
    }

    int View::getVisibility() const {
        return visibility_;
    }

    int View::getOutline() const {
        return outline_;
    }

    const Size& View::getMinimumSize() const {
        return min_size_;
    }

    const Size& View::getDeterminedSize() const {
        return determined_size_;
    }

    Padding& View::getPadding() {
        return padding_;
    }

    const Padding& View::getPadding() const {
        return padding_;
    }

    OnClickListener* View::getClickListener() const {
        return click_listener_;
    }

    OnInputEventDelegate* View::getInputEventDelegate() const {
        return ie_delegate_;
    }

    LayoutView* View::getParent() const {
        return parent_;
    }

    Size& View::getLayoutSize() {
        return layout_size_;
    }

    const Size& View::getLayoutSize() const {
        return layout_size_;
    }

    Margin& View::getLayoutMargin() {
        return layout_margin_;
    }

    const Margin& View::getLayoutMargin() const {
        return layout_margin_;
    }

    LayoutInfo* View::getExtraLayoutInfo() const {
        return layout_info_.get();
    }

    Window* View::getWindow() const {
        return window_;
    }

    Context View::getContext() const {
        return context_;
    }

    Element* View::getBackground() const {
        return bg_element_;
    }

    Element* View::getForeground() const {
        return fg_element_;
    }

    Rect View::getBounds() const {
        return bounds_;
    }

    Rect View::getBoundsInRoot() const {
        auto bounds = getBounds();

        auto parent = parent_;
        while (parent) {
            auto p_bounds = parent->getBounds();
            auto dx = p_bounds.left - parent->getScrollX();
            auto dy = p_bounds.top - parent->getScrollY();
            parent = parent->getParent();
            if (parent) {
                bounds.offset(dx, dy);
            }
        }

        return bounds;
    }

    Rect View::getBoundsInWindow() const {
        auto bounds = getBounds();

        auto parent = parent_;
        while (parent) {
            auto p_bounds = parent->getBounds();
            bounds.offset(
                p_bounds.left - parent->getScrollX(),
                p_bounds.top - parent->getScrollY());
            parent = parent->getParent();
        }

        return bounds;
    }

    Rect View::getBoundsInScreen() const {
        auto w = getWindow();
        auto bound = getBoundsInWindow();
        if (!w) {
            return bound;
        }

        Point pt;
        pt.x() = bound.left;
        pt.y() = bound.top;

        w->convClientToScreen(&pt);

        int dx = pt.x() - bound.left;
        int dy = pt.y() - bound.top;

        bound.offset(dx, dy);

        return bound;
    }

    Rect View::getContentBounds() const {
        int content_width = bounds_.width() - padding_.hori();
        int content_height = bounds_.height() - padding_.vert();
        return Rect(
            padding_.start, padding_.top,
            content_width, content_height);
    }

    Rect View::getVisibleBounds() const {
        auto bounds = getBounds();

        int dx = 0;
        int dy = 0;
        auto parent = parent_;
        while (parent) {
            auto pc_bounds = parent->getContentBounds();
            dx += parent->getScrollX();
            dy += parent->getScrollY();
            pc_bounds.offset(dx, dy);
            bounds.same(pc_bounds);
            dx -= parent->bounds_.left;
            dy -= parent->bounds_.top;
            parent = parent->getParent();
        }

        return bounds;
    }

    Padding View::getBoundsExtension() const {
        int extend = 0;
        if (shadow_effect_) {
            extend = shadow_effect_->getRadius();
            if (extend) {
                ++extend;
            }
        }

        return Padding(extend, extend, extend, extend);
    }

    std::weak_ptr<void> View::getCanary() const {
        return cur_ev_;
    }

    void View::transformBounds(Rect* bounds) const {
        Matrix2x3F matrix;
        anime_params_.generateMatrix(bounds->left, bounds->top, &matrix);

        PointF lt, tr, rb, bl;
        matrix.transformRect(RectF(*bounds), &lt, &tr, &rb, &bl);

        bounds->left = int(std::floor((std::min)({  lt.x(), tr.x(), rb.x(), bl.x() })));
        bounds->top = int(std::floor((std::min)({   lt.y(), tr.y(), rb.y(), bl.y() })));
        bounds->right = int(std::ceil((std::max)({  lt.x(), tr.x(), rb.x(), bl.x() })));
        bounds->bottom = int(std::ceil((std::max)({ lt.y(), tr.y(), rb.y(), bl.y() })));
    }

    bool View::isEnabled() const {
        return is_enabled_;
    }

    bool View::isAttachedToWindow() const {
        return is_attached_to_window_;
    }

    bool View::isPressed() const {
        return is_pressed_;
    }

    bool View::hasFocus() const {
        return has_focus_;
    }

    bool View::canGetFocus() const {
        return is_focusable_ && visibility_ == SHOW && is_enabled_;
    }

    bool View::isClickable() const {
        return is_clickable_;
    }

    bool View::isDoubleClickable() const {
        return is_dbclkable_;
    }

    bool View::isFocusable() const {
        return is_focusable_;
    }

    bool View::isTouchCapturable() const {
        return is_touch_capturable_;
    }

    bool View::isLayouted() const {
        return is_layouted_;
    }

    bool View::isLocalPointerInThis(InputEvent* e) const {
        switch (outline_) {
        case OUTLINE_OVAL:
        {
            float a = bounds_.width() / 2.f;
            float b = bounds_.height() / 2.f;
            return std::pow(e->getX() / a - 1, 2) + std::pow(e->getY() / b - 1, 2) <= 1;
        }

        case OUTLINE_RECT:
        default:
            return Rect(0, 0, bounds_.width(), bounds_.height()).hit(e->getX(), e->getY());
        }
    }

    bool View::isLocalPointerInThisVisible(InputEvent* e) const {
        auto bounds = getVisibleBounds();
        bounds.offset(-bounds_.left, -bounds_.top);

        switch (outline_) {
        case OUTLINE_OVAL:
        {
            float a = bounds.width() / 2.f;
            float b = bounds.height() / 2.f;
            return std::pow(e->getX() / a - 1, 2) + std::pow(e->getY() / b - 1, 2) <= 1 &&
                bounds.hit(e->getX(), e->getY());
        }

        case OUTLINE_RECT:
        default:
            return bounds.hit(e->getX(), e->getY());
        }
    }

    bool View::isParentPointerInThis(InputEvent* e) const {
        switch (outline_) {
        case OUTLINE_OVAL:
        {
            float a = bounds_.width() / 2.f;
            float b = bounds_.height() / 2.f;
            return std::pow((e->getX() - bounds_.left) / a - 1, 2)
                + std::pow((e->getY() - bounds_.top) / b - 1, 2) <= 1;
        }

        case OUTLINE_RECT:
        default:
            return bounds_.hit(e->getX(), e->getY());
        }
    }

    bool View::isReceiveOutsideInputEvent() const {
        return is_receive_outside_input_event_;
    }

    void View::scrollTo(int x, int y) {
        if (scroll_x_ != x || scroll_y_ != y) {
            int oldScrollX = scroll_x_;
            int oldScrollY = scroll_y_;

            scroll_x_ = x;
            scroll_y_ = y;

            requestDraw();
            onScrollChanged(x, y, oldScrollX, oldScrollY);
        }
    }

    void View::scrollBy(int dx, int dy) {
        scrollTo(scroll_x_ + dx, scroll_y_ + dy);
    }

    void View::onElementRequestDraw(Element* d) {
        requestDraw();
    }

    void View::performClick() {
        if (click_listener_) {
            click_listener_->onClick(this);
        }
    }

    void View::performDoubleClick() {
        if (click_listener_) {
            click_listener_->onDoubleClick(this);
        }
    }

    void View::draw(Canvas* canvas, const Rect& dirty_rect) {
        UI_TRACK_RT_START;

        dirty_rect_ = dirty_rect;

        if (animator_) {
            animator_->onPreViewDraw();
        }
        onPreDraw();

        // 应用动画变量
        canvas->save();
        canvas->setOpacity(
            float(anime_params_.getAlpha()*canvas->getOpacity()));

        Matrix2x3F anim_m;
        anime_params_.generateMatrix(0, 0, &anim_m);
        canvas->concat(anim_m);

        bool has_bg = needDrawBackground();
        bool has_shadow = (has_bg && (shadow_radius_ > 0) && shadow_effect_);

        if (anime_params_.hasReveal()) {
            drawWithReveal(canvas, has_bg, has_shadow);
        } else {
            drawNormal(canvas, has_bg, has_shadow);
        }

        canvas->restore();

        onPostDraw();
        if (animator_) {
            animator_->onPostViewDraw();
        }

        UI_TRACK_RT_END;
    }

    void View::drawNormal(Canvas* c, bool has_bg, bool has_shadow) {
        if (has_shadow) {
            // 将背景绘制到 bg_img 上
            Canvas bg_off(
                getWidth(), getHeight(),
                c->getBuffer()->getImageOptions());
            bg_off.beginDraw();
            bg_off.clear();
            bg_off.setOpacity(c->getOpacity());
            drawBackground(&bg_off);
            bg_off.endDraw();
            auto buffer = static_cast<OffscreenBuffer*>(bg_off.getBuffer());

            if (!shadow_effect_->setContent(buffer) ||
                !shadow_effect_->draw(c))
            {
                drawBackground(c);
            }
        } else if (has_bg) {
            drawBackground(c);
        }

        drawContent(c);
    }

    void View::drawWithReveal(Canvas* c, bool has_bg, bool has_shadow) {
        if (has_shadow) {
            // 将背景绘制到 bg_img 上
            Canvas bg_off(
                getWidth(), getHeight(),
                c->getBuffer()->getImageOptions());
            bg_off.beginDraw();
            bg_off.clear();
            bg_off.setOpacity(c->getOpacity());
            drawBackground(&bg_off);
            bg_off.endDraw();
            auto bg_img = bg_off.extractImage();
            if (bg_img) {
                Canvas offscreen(
                    getWidth(), getHeight(),
                    c->getBuffer()->getImageOptions());
                offscreen.beginDraw();
                offscreen.clear();
                if (anime_params_.reveal().getType() == ViewRevealTVals::Type::Circle) {
                    auto r = anime_params_.reveal().getRV(this);
                    offscreen.fillCircle(
                        PointF(r.pos()), float(r.width()), bg_img.get());
                } else {
                    offscreen.fillRect(
                        RectF(anime_params_.reveal().getRV(this)), bg_img.get());
                }
                offscreen.endDraw();
                auto buffer = static_cast<OffscreenBuffer*>(offscreen.getBuffer());

                if (!shadow_effect_->setContent(buffer) ||
                    !shadow_effect_->draw(c))
                {
                    c->drawImage(bg_img.get());
                }
            } else {
                drawBackground(c);
            }
        } else if (has_bg) {
            drawBackground(c);
        }

        Canvas cur_c(
            getWidth(), getHeight(),
            c->getBuffer()->getImageOptions());
        cur_c.beginDraw();
        cur_c.clear();
        drawContent(&cur_c);
        cur_c.endDraw();
        auto c_img = cur_c.extractImage();
        if (c_img) {
            if (anime_params_.reveal().getType() == ViewRevealTVals::Type::Circle) {
                c->pushClip(RectF(0, 0, float(getWidth()), float(getHeight())));
                auto r = anime_params_.reveal().getRV(this);
                c->fillCircle(
                    PointF(r.pos()), float(r.width()), c_img.get());
                c->popClip();
            } else {
                c->fillRect(
                    RectF(anime_params_.reveal().getRV(this)), c_img.get());
            }
        }
    }

    void View::drawContent(Canvas* c) {
        // 裁剪出可用区
        c->pushClip(RectF(
            float(padding_.start), float(padding_.top),
            float(getWidth() - padding_.hori()),
            float(getHeight() - padding_.vert())));
        {
            c->save();
            c->translate(float(padding_.start), float(padding_.top));
            c->translate(-float(scroll_x_), -float(scroll_y_));

            // 绘制自身
            onDraw(c);

            c->restore();
        }
        {
            c->save();
            c->translate(-float(scroll_x_), -float(scroll_y_));

            // 绘制孩子，这里不要偏移 padding，因为 padding 信息
            // 已经包含在 bounds 里了，画布会根据 bounds 进行偏移的。
            dispatchDraw(c);

            c->restore();
        }
        {
            c->save();
            c->translate(float(padding_.start), float(padding_.top));

            // 绘制盖在孩子之上的内容
            onDrawOverChildren(c);

            c->restore();
        }
        c->popClip();

        c->pushClip(
            RectF(0, 0, float(getWidth()), float(getHeight())));
        {
            // 绘制前景
            drawForeground(c);
        }
        c->popClip();
    }

    bool View::needDrawBackground() {
        return (bg_element_ != nullptr
            && bg_element_->getOpacity() != Element::OPA_TRANSPARENT);
    }

    bool View::needDrawForeground() {
        return (fg_element_ != nullptr
            && fg_element_->getOpacity() != Element::OPA_TRANSPARENT);
    }

    void View::drawBackground(Canvas* canvas) {
        if (needDrawBackground()) {
            bg_element_->setBounds(0, 0, bounds_.width(), bounds_.height());
            bg_element_->draw(canvas);
        }
    }

    void View::drawForeground(Canvas* canvas) {
        if (needDrawForeground()) {
            fg_element_->setBounds(0, 0, bounds_.width(), bounds_.height());
            fg_element_->draw(canvas);
        }
    }

    bool View::sendInputEvent(InputEvent* e, bool cancel) {
        if (is_mouse_down_ && e->isTouchEvent()) {
            return true;
        }
        if (is_touch_down_ && e->isMouseEvent()) {
            return true;
        }

        /**
         * EV_LEAVE_VIEW 只能发送给最后接受输入的 View，这里不做判断，
         * 由调用方保证。
         */
        ubassert(
            e->getEvent() != InputEvent::EV_LEAVE_VIEW ||
            window_->getLastInputView() == this);

        std::weak_ptr<InputEvent> wptr = cur_ev_;
        bool consumed = processInputEvent(e);
        if (wptr.expired() || !isAttachedToWindow()) {
            return consumed;
        }

        updateLastInputView(e, consumed, cancel);
        return consumed;
    }

    bool View::invokeOnInputEvent(InputEvent* e) {
        if (ie_delegate_) {
            bool ret = false;
            if (ie_delegate_->onInputReceived(this, e, &ret)) {
                return ret;
            }
        }
        return onInputEvent(e);
    }

    void View::updateLastInputView(InputEvent* e, bool consumed, bool cancel) {
        /**
         * 这里用于更新最后接受输入的 View，但只限于接收到以下几个事件时更新，
         * 以确保操作的合理性。原则上只能于一次输入流程的开始事件时更新。
         */
        if (e->getEvent() == InputEvent::EVM_DOWN ||
            e->getEvent() == InputEvent::EVM_MOVE ||
            e->getEvent() == InputEvent::EVM_SCROLL_ENTER)
        {
            auto prev_target = window_->getLastInputView();
            if (prev_target == this) {
                return;
            }

            bool need_null = false;
            if (prev_target) {
                bool send_leave = true;
                /**
                 * 对于鼠标事件，如果当前事件已消费，则直接向前一个接受了事件的 View 发送
                 * 离开事件，以确保在 View 重叠时的鼠标操作行为是符合预期的。
                 * 例如鼠标在 ScrollView 中移动，如果存在消费了鼠标移动事件的子 View，就
                 * 向 ScrollView 发送离开事件。
                 */
                if (!consumed && !cancel) {
                    InputEvent prev(*e);
                    auto cur_bounds = getBoundsInRoot();
                    auto prev_bounds = prev_target->getBoundsInRoot();
                    prev.offsetInputPos(
                        cur_bounds.left - prev_bounds.left,
                        cur_bounds.top - prev_bounds.top);
                    send_leave = !prev_target->isLocalPointerInThisVisible(&prev);
                }

                if (send_leave) {
                    need_null = true;
                    /*LOG(Log::INFO) << "========= SEND_LEAVE| this["
                        << typeid(*this).name() << "] prev[" << (prev_target ? typeid(*prev_target).name() : "null")
                        << "] consumed: " << consumed << " cancel: " << cancel;*/

                    InputEvent ev(*e);
                    ev.setEvent(InputEvent::EV_LEAVE_VIEW);
                    ev.setIsNoDispatch(true);
                    ev.setCancelled(cancel);
                    std::weak_ptr<InputEvent> wptr = cur_ev_;
                    prev_target->dispatchInputEvent(&ev);
                    if (wptr.expired()) {
                        return;
                    }
                }
            }

            if (consumed) {
                window_->setLastInputView(this);
            } else if (need_null) {
                window_->setLastInputView(nullptr);
            }
        } else if (e->getEvent() == InputEvent::EVT_DOWN ||
            e->getEvent() == InputEvent::EVT_MOVE)
        {
            auto prev_target = window_->getLastInputView();
            if (prev_target == this) {
                return;
            }

            bool need_null = false;
            if (prev_target) {
                bool send_leave = true;
                /**
                 * 对于触摸事件，因为一连串的操作是以按下作为开始，抬起作为结束，
                 * 并不需要离开事件，如果发送离开事件，反而会造成问题。
                 * 例如在 ScrollView 中的一个按钮上按下，ScrollView 的 onHookInputEvent
                 * 会比按钮先接收到按下事件，之后按钮接收到按下事件，会向 ScrollView 发送离开事件，
                 * 这将清除 ScrollView 的触摸状态，导致行为错误。
                 */
                if (!cancel) {
                    InputEvent prev(*e);
                    auto cur_bounds = getBoundsInRoot();
                    auto prev_bounds = prev_target->getBoundsInRoot();
                    prev.offsetInputPos(
                        cur_bounds.left - prev_bounds.left,
                        cur_bounds.top - prev_bounds.top);
                    send_leave = !prev_target->isLocalPointerInThisVisible(&prev);
                }

                if (send_leave) {
                    need_null = true;
                    LOG(Log::INFO) << "========= SEND_LEAVE| this["
                        << typeid(*this).name() << "] prev[" << (prev_target ? typeid(*prev_target).name() : "null")
                        << "] consumed: " << consumed << " cancel: " << cancel;

                    InputEvent ev(*e);
                    ev.setEvent(InputEvent::EV_LEAVE_VIEW);
                    ev.setIsNoDispatch(true);
                    ev.setCancelled(cancel);
                    std::weak_ptr<InputEvent> wptr = cur_ev_;
                    prev_target->dispatchInputEvent(&ev);
                    if (wptr.expired()) {
                        return;
                    }
                }
            }

            if (consumed) {
                window_->setLastInputView(this);
            } else if (need_null) {
                window_->setLastInputView(nullptr);
            }
        }
    }

    void View::updateBackgroundState() {
        bool need_redraw = false;
        if (isEnabled()) {
            if (bg_element_) {
                need_redraw = bg_element_->setState(Element::STATE_NONE);
            }
        } else {
            if (bg_element_) {
                need_redraw = bg_element_->setState(Element::STATE_DISABLED);
            }
        }

        if (need_redraw) {
            requestDraw();
        }
    }

    void View::updateForegroundState() {
        bool need_redraw = false;
        if (isEnabled()) {
            if (fg_element_) {
                need_redraw = fg_element_->setState(Element::STATE_NONE);
            }
        } else {
            if (fg_element_) {
                need_redraw = fg_element_->setState(Element::STATE_DISABLED);
            }
        }

        if (need_redraw) {
            requestDraw();
        }
    }

    void View::resetBackground() {
        if (bg_element_) {
            if (owned_bg_) {
                delete bg_element_;
            } else {
                if (bg_element_->getCallback() == this) {
                    bg_element_->setCallback(nullptr);
                }
            }
        }
    }

    void View::resetForeground() {
        if (fg_element_) {
            if (owned_fg_) {
                delete fg_element_;
            } else {
                if (fg_element_->getCallback() == this) {
                    fg_element_->setCallback(nullptr);
                }
            }
        }
    }

    void View::processPointerUp() {
        if (!click_listener_) {
            return;
        }

        bool need_perform_click = true;
        if (is_dbclkable_) {
            if (!wait_for_dbclk_) {
                wait_for_dbclk_ = true;
                first_clk_ts = utl::TimeUtils::upTimeMillis();
            } else {
                if (utl::TimeUtils::upTimeMillis() - first_clk_ts <= getDoubleClickTime()) {
                    wait_for_dbclk_ = false;
                    need_perform_click = false;
                    performDoubleClick();
                } else {
                    first_clk_ts = utl::TimeUtils::upTimeMillis();
                }
            }
        }

        if (is_clickable_ && need_perform_click) {
            performClick();
        }
    }

    bool View::processInputEvent(InputEvent* e) {
        // 对于鼠标事件的过滤
        if (e->getEvent() == InputEvent::EVM_UP && !is_mouse_down_) {
            return false;
        }

        // 对于触摸事件的过滤
        if (e->isTouchEvent() &&
            e->getEvent() != InputEvent::EVT_DOWN &&
            e->getEvent() != InputEvent::EV_LEAVE_VIEW &&
            !is_touch_down_)
        {
            return false;
        }

        InputEvent* ev;
        if (e->isTouchEvent()) {
            cur_ev_->combineTouchEvent(e);
            ev = cur_ev_.get();
        } else {
            ev = e;
        }

        std::weak_ptr<InputEvent> wptr = cur_ev_;

        bool consumed = invokeOnInputEvent(ev);

        if (wptr.expired() || !isAttachedToWindow()) {
            return consumed;
        }

        if (e->isTouchEvent()) {
            cur_ev_->clearTouchUp();
        }

        bool should_refresh = false;
        switch (ev->getEvent()) {
        case InputEvent::EVM_DOWN:
            if (consumed) {
                is_mouse_down_ = true;
                window_->captureMouse(this);
                if (is_focusable_) {
                    requestFocus();
                }

                if (e->getMouseKey() == InputEvent::MK_LEFT) {
                    setPressed(true);
                    if (fg_element_) {
                        fg_element_->setHotspot(e->getX(), e->getY());
                        should_refresh = fg_element_->setState(Element::STATE_PRESSED);
                    }
                    if (bg_element_) {
                        bg_element_->setHotspot(e->getX(), e->getY());
                        should_refresh = bg_element_->setState(Element::STATE_PRESSED);
                    }
                    if (should_refresh) {
                        requestDraw();
                    }
                }
            }
            return consumed;

        case InputEvent::EVT_DOWN:
            if (consumed) {
                is_touch_down_ = true;
                if (is_focusable_) {
                    requestFocus();
                }

                setPressed(true);
                if (fg_element_) {
                    fg_element_->setHotspot(e->getX(), e->getY());
                    should_refresh = fg_element_->setState(Element::STATE_PRESSED);
                }
                if (bg_element_) {
                    bg_element_->setHotspot(e->getX(), e->getY());
                    should_refresh = bg_element_->setState(Element::STATE_PRESSED);
                }

                if (should_refresh) {
                    requestDraw();
                }
            } else {
                cur_ev_->clearTouch();
            }
            return consumed;

        case InputEvent::EVT_MULTI_DOWN:
            return true;

        case InputEvent::EVM_MOVE:
            if (consumed) {
                if (!isPressed()) {
                    if (fg_element_) {
                        //fg_element_->setHotspot(e->getX(), e->getY());
                        should_refresh = fg_element_->setState(Element::STATE_HOVERED);
                    }
                    if (bg_element_) {
                        //bg_element_->setHotspot(e->getX(), e->getY());
                        should_refresh = bg_element_->setState(Element::STATE_HOVERED);
                    }
                }
                if (should_refresh) {
                    requestDraw();
                }
            }
            return consumed;

        case InputEvent::EVT_MOVE:
            if (consumed) {
                ubassert(is_touch_down_);
                /**
                 * 对于触摸事件，一般来说存在移动阈值，超过阈值才算是真正操作
                 * 这个 View，所以触摸捕获不能放在按下事件里，需要延后，放在这里。
                 */
                if (is_touch_capturable_ &&
                    window_->getTouchHolder() != this)
                {
                    window_->captureTouch(this);
                }
            }
            return consumed;

        case InputEvent::EVM_SCROLL_ENTER:
            if (fg_element_) {
                should_refresh = fg_element_->setState(Element::STATE_HOVERED);
            }
            if (bg_element_) {
                should_refresh = bg_element_->setState(Element::STATE_HOVERED);
            }
            if (should_refresh) {
                requestDraw();
            }
            return consumed;

        case InputEvent::EVM_UP:
            window_->releaseMouse();
            if (window_->getMouseHolderRef() == 0) {
                is_mouse_down_ = false;
            }
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                bool pressed = isPressed();
                setPressed(false);

                if (isLocalPointerInThisVisible(e)) {
                    if (fg_element_) {
                        fg_element_->setHotspot(e->getX(), e->getY());
                        should_refresh = fg_element_->setState(Element::STATE_HOVERED);
                    }
                    if (bg_element_) {
                        bg_element_->setHotspot(e->getX(), e->getY());
                        should_refresh = bg_element_->setState(Element::STATE_HOVERED);
                    }

                    if (pressed) {
                        processPointerUp();
                        if (wptr.expired() || !isAttachedToWindow()) {
                            return consumed;
                        }
                    }
                } else {
                    if (fg_element_) {
                        should_refresh = fg_element_->setState(Element::STATE_NONE);
                    }
                    if (bg_element_) {
                        should_refresh = bg_element_->setState(Element::STATE_NONE);
                    }
                }
            }
            if (should_refresh) {
                requestDraw();
            }
            return consumed;

        case InputEvent::EVT_MULTI_UP:
            return true;

        case InputEvent::EVT_UP:
        {
            is_touch_down_ = false;
            bool pressed = isPressed();
            setPressed(false);

            if (window_->getTouchHolder() == this) {
                window_->releaseTouch(true);
            }

            if (fg_element_) {
                fg_element_->setHotspot(e->getX(), e->getY());
                should_refresh = fg_element_->setState(Element::STATE_HOVERED);
                should_refresh |= fg_element_->setState(Element::STATE_NONE);
            }
            if (bg_element_) {
                bg_element_->setHotspot(e->getX(), e->getY());
                should_refresh = bg_element_->setState(Element::STATE_HOVERED);
                should_refresh |= bg_element_->setState(Element::STATE_NONE);
            }

            if (isLocalPointerInThisVisible(e) && pressed) {
                processPointerUp();
                if (wptr.expired() || !isAttachedToWindow()) {
                    return consumed;
                }
            }

            if (should_refresh) {
                requestDraw();
            }
            return true;
        }

        case InputEvent::EV_LEAVE_VIEW:
            if (e->isMouseEvent() && is_mouse_down_) {
                window_->releaseMouse();
                is_mouse_down_ = false;
            }
            if (e->isTouchEvent() && is_touch_down_) {
                window_->releaseTouch(true);
                is_touch_down_ = false;
            }
            setPressed(false);
            if (fg_element_) {
                should_refresh = fg_element_->setState(Element::STATE_NONE);
            }
            if (bg_element_) {
                should_refresh = bg_element_->setState(Element::STATE_NONE);
            }
            if (should_refresh) {
                requestDraw();
            }
            return consumed;

        default:
            break;
        }

        return consumed;
    }

    void View::measure(const SizeInfo& info) {
        if (is_measured_) {
            bool is_defined_size =
                (info.width.mode == SizeInfo::DEFINED &&
                    info.height.mode == SizeInfo::DEFINED);

            if (!request_layout_ &&
                is_defined_size &&
                info == saved_size_info_)
            {
                return;
            }
        }

        determined_size_ = onDetermineSize(info);
        determined_size_.setToMax(min_size_);

        is_measured_ = true;
        need_layout_ = true;
        saved_size_info_ = info;
    }

    void View::layout(const Rect& bounds) {
        request_layout_ = false;

        auto old_bounds(bounds_);
        auto new_bounds(bounds);

        onBeforeLayout(&new_bounds, old_bounds);

        bool changed = old_bounds != new_bounds;
        if (changed) {
            // 先刷老的 bounds
            auto ext_bounds(old_bounds);
            ext_bounds.extend(getBoundsExtension());
            requestDrawRelParent(ext_bounds);

            onBoundsChanging(new_bounds, old_bounds);
            bounds_ = new_bounds;
            onBoundsChanged(new_bounds, old_bounds);

            // 再刷新的 bounds
            ext_bounds = new_bounds;
            ext_bounds.extend(getBoundsExtension());
            requestDrawRelParent(ext_bounds);
        }

        if (changed || need_layout_) {
            onLayout(new_bounds, old_bounds);
        }

        is_layouted_ = true;
        need_layout_ = false;

        onAfterLayout(new_bounds, old_bounds);
    }

    void View::requestDraw() {
        auto ext_bounds(bounds_);
        ext_bounds.extend(getBoundsExtension());
        requestDrawRelParent(ext_bounds);
    }

    void View::requestDraw(const Rect& rect) {
        Rect bounds(rect);
        bounds.offset(bounds_.left, bounds_.top);
        requestDrawRelParent(bounds);
    }

    void View::requestDrawRelParent(const Rect& rect) {
        if (rect.empty()) {
            return;
        }

        auto ext_bounds(bounds_);
        ext_bounds.extend(getBoundsExtension());

        Rect bounds(rect);
        bounds.same(ext_bounds);
        if (bounds.empty()) {
            return;
        }

        if (parent_) {
            int off_x = parent_->getLeft() - parent_->getScrollX();
            int off_y = parent_->getTop() - parent_->getScrollY();
            bounds.offset(off_x, off_y);

            transformBounds(&bounds);
            parent_->requestDrawRelParent(bounds);
        }
    }

    void View::requestLayout() {
        request_layout_ = true;

        if (parent_) {
            parent_->requestLayout();
        }
    }

    void View::requestFocus() {
        if (!canGetFocus() ||
            has_focus_ ||
            has_focus_requesting_)
        {
            return;
        }

        if (!isAttachedToWindow()) {
            has_focus_requesting_ = true;
            return;
        }

        // 先取消其他 View 的焦点。
        View* prev_holder = window_->getKeyboardHolder();
        if (prev_holder) {
            prev_holder->discardFocus();
        }

        // 获取焦点。
        window_->captureKeyboard(this);

        has_focus_ = true;
        onFocusChanged(true);
    }

    void View::discardFocus() {
        if (has_focus_requesting_) {
            has_focus_requesting_ = false;
        }

        if (!isAttachedToWindow()) {
            return;
        }

        if (has_focus_) {
            has_focus_ = false;
            window_->releaseKeyboard();
            onFocusChanged(false);
        }

        dispatchDiscardFocus();
    }

    void View::discardMouseCapture() {
        if (!window_) {
            return;
        }

        if (window_->getMouseHolder() == this) {
            window_->releaseMouse(true);
        }
    }

    void View::discardTouchCapture() {
        if (!window_) {
            return;
        }

        if (window_->getTouchHolder() == this) {
            window_->releaseTouch(true);
        }
    }

    void View::discardPendingOperations() {
        dispatchDiscardPendingOperations();
    }

    void View::resetLayoutStatus() {
        is_measured_ = false;
        is_layouted_ = false;
        need_layout_ = true;
        request_layout_ = false;
    }

    void View::resetLastInputView() {
        if (!window_) {
            return;
        }

        if (window_->getLastInputView() == this) {
            window_->setLastInputView(nullptr);
        }
    }

    View* View::findView(int id) {
        if (id_ == id) {
            return this;
        }
        return nullptr;
    }

    bool View::dispatchInputEvent(InputEvent* e) {
        e->offsetInputPos(-bounds_.left, -bounds_.top);
        return sendInputEvent(e);
    }

    void View::dispatchWindowFocusChanged(bool focus) {
        onWindowFocusChanged(focus);
    }

    void View::dispatchContextChanged(const Context& context) {
        onContextChanged(context);
    }

    void View::dispatchAncestorVisibilityChanged(View* ancestor, int visibility) {
        if (ancestor != this) {
            onAncestorVisibilityChanged(ancestor, visibility);
        }
    }

    void View::dispatchAncestorEnableChanged(View* ancestor, bool enabled) {
        if (ancestor != this) {
            onAncestorEnableChanged(ancestor, enabled);
        }
    }

    void View::dispatchAttachedToWindow(Window* w) {
        window_ = w;
        is_attached_to_window_ = true;

        if (input_connection_) {
            input_connection_->pushEditor();
        }

        if (bg_element_) {
            bg_element_->notifyAttachedToWindow(w);
        }
        if (fg_element_) {
            fg_element_->notifyAttachedToWindow(w);
        }

        if (has_focus_requesting_) {
            has_focus_requesting_ = false;
            requestFocus();
        }

        onAttachedToWindow(w);
    }

    void View::dispatchDetachFromWindow() {
        onDetachFromWindow();

        if (input_connection_) {
            input_connection_->discardFocus();
        }

        if (animator_) {
            animator_->cancel();
        }

        discardFocus();
        discardMouseCapture();
        discardTouchCapture();
        discardPendingOperations();
        resetLastInputView();
        updateBackgroundState();
        updateForegroundState();

        if (bg_element_) {
            bg_element_->notifyDetachedFromWindow();
        }
        if (fg_element_) {
            fg_element_->notifyDetachedFromWindow();
        }

        window_ = nullptr;
        is_attached_to_window_ = false;
    }

    bool View::isLayoutView() const {
        return false;
    }

    bool View::onInputEvent(InputEvent* e) {
        if (e->getEvent() == InputEvent::EVM_WHEEL) {
            return false;
        }
        return is_clickable_ || is_dbclkable_;
    }

    Size View::onDetermineSize(const SizeInfo& info) {
        return getPreferredSize(info, 0, 0);
    }

    bool View::onCheckIsTextEditor() {
        return false;
    }

    InputMethodConnection* View::onCreateInputConnection() {
        return nullptr;
    }

    void View::onAncestorVisibilityChanged(View* ancestor, int visibility) {
        if (visibility_ == SHOW && visibility != SHOW) {
            discardMouseCapture();
            discardTouchCapture();
            resetLastInputView();
        }
    }

    void View::onAncestorEnableChanged(View* ancestor, bool enabled) {
        if (is_enabled_ && !enabled) {
            discardMouseCapture();
            discardTouchCapture();
            resetLastInputView();
        }
    }

    void View::onFocusChanged(bool get_focus) {
        if (get_focus) {
            if (onCheckIsTextEditor()) {
                if (!input_connection_) {
                    input_connection_ = onCreateInputConnection();
                }

                if (input_connection_) {
                    input_connection_->initialization();
                    input_connection_->pushEditor();
                    input_connection_->requestFocus();
                }
            }
        } else {
            if (input_connection_) {
                input_connection_->discardFocus();
                input_connection_->terminateComposition();
            }
        }

        bool should_refresh = false;
        if (bg_element_) {
            should_refresh = bg_element_->setParentFocus(get_focus);
        }
        if (fg_element_) {
            should_refresh = fg_element_->setParentFocus(get_focus);
        }

        if (should_refresh) {
            requestDraw();
        }
    }

    void View::onWindowFocusChanged(bool window_focus) {
        if (!hasFocus()) {
            ubassert(getWindow()->getKeyboardHolder() != this);
            return;
        }

        if (window_focus) {
            if (onCheckIsTextEditor()) {
                if (!input_connection_) {
                    input_connection_ = onCreateInputConnection();
                }

                if (input_connection_) {
                    bool ret = input_connection_->initialization();
                    ubassert(ret);
                    input_connection_->pushEditor();
                    input_connection_->requestFocus();
                }
            }
        } else {
            if (input_connection_) {
                bool ret = input_connection_->terminateComposition();
                ubassert(ret);

                ret = input_connection_->discardFocus();
                ubassert(ret);
            }
        }
    }

    void View::onContextChanged(const Context& context) {
        switch (context.getChanged()) {
        case Context::DPI_CHANGED:
        {
            if (Application::getOptions().is_auto_dpi_scale) {
                if (shadow_effect_) {
                    shadow_effect_->destroy();
                    shadow_effect_.reset();
                }

                shadow_effect_.reset(ShadowEffect::create(getContext()));
                shadow_effect_->initialize();
                shadow_effect_->setRadius(shadow_radius_ * 2);
            }
            break;
        }

        default:
            break;
        }

        if (bg_element_) {
            bg_element_->notifyContextChanged(context);
        }
        if (fg_element_) {
            fg_element_->notifyContextChanged(context);
        }
    }

}
