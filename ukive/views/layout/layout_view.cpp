// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "layout_view.h"

#include <queue>

#include "utils/log.h"

#include "ukive/views/layout_info/layout_info.h"
#include "ukive/graphics/canvas.h"
#include "ukive/resources/dimension_utils.h"
#include "ukive/window/window.h"

#include "ukive/views/input_event_delegate.h"

namespace ukive {

    LayoutView::LayoutView(Context c)
        : LayoutView(c, {}) {}

    LayoutView::LayoutView(Context c, AttrsRef attrs)
        : View(c, attrs) {}

    LayoutView::~LayoutView() {
        utl::STLDeleteElements(&views_);
    }

    void LayoutView::setHookInputEventDelegate(
        OnHookInputEventDelegate* d)
    {
        iie_delegate_ = d;
    }

    LayoutInfo* LayoutView::makeExtraLayoutInfo() const {
        return nullptr;
    }

    LayoutInfo* LayoutView::makeExtraLayoutInfo(AttrsRef attrs) const {
        return nullptr;
    }

    bool LayoutView::isValidExtraLayoutInfo(LayoutInfo* lp) const {
        return true;
    }

    int LayoutView::getWrappedWidth() {
        int wrapped_width = 0;
        for (auto view : views_) {
            if (view->getVisibility() != VANISHED) {
                int child_width = view->getDeterminedSize().width() + view->getLayoutMargin().hori();
                if (child_width > wrapped_width) {
                    wrapped_width = child_width;
                }
            }
        }

        return wrapped_width;
    }

    int LayoutView::getWrappedHeight() {
        int wrapped_height = 0;
        for (auto view : views_) {
            if (view->getVisibility() != VANISHED) {
                int child_height = view->getDeterminedSize().height() + view->getLayoutMargin().vert();
                if (child_height > wrapped_height) {
                    wrapped_height = child_height;
                }
            }
        }

        return wrapped_height;
    }

    Size LayoutView::getWrappedSize(const SizeInfo& info) {
        int final_width = 0;
        int final_height = 0;

        switch (info.width().mode) {
        case SizeInfo::CONTENT:
            final_width = getWrappedWidth();
            final_width = (std::min)(final_width + getPadding().hori(), info.width().val);
            break;

        case SizeInfo::FREEDOM:
            final_width = getWrappedWidth();
            final_width = final_width + getPadding().hori();
            break;

        case SizeInfo::DEFINED:
        default:
            final_width = info.width().val;
            break;
        }

        switch (info.height().mode) {
        case SizeInfo::CONTENT:
            final_height = getWrappedHeight();
            final_height = (std::min)(final_height + getPadding().vert(), info.height().val);
            break;

        case SizeInfo::FREEDOM:
            final_height = getWrappedHeight();
            final_height = final_height + getPadding().vert();
            break;

        case SizeInfo::DEFINED:
        default:
            final_height = info.height().val;
            break;
        }

        return Size(final_width, final_height);
    }

    bool LayoutView::isLayoutView() const {
        return true;
    }

    void LayoutView::addView(View* v, bool req_layout) {
        addView(views_.size(), v, req_layout);
    }

    void LayoutView::addView(size_t index, View* v, bool req_layout) {
        if (!v) {
            DLOG(Log::ERR) << "You cannot add a null View to LayoutView.";
            return;
        }

        if (index > views_.size()) {
            index = views_.size();
        }

        if (v->getParent()) {
            DLOG(Log::ERR) << "v already have a parent!";
            return;
        }

        auto li = v->getExtraLayoutInfo();
        if (!li) {
            li = makeExtraLayoutInfo();
        }

        if (!isValidExtraLayoutInfo(li)) {
            li = makeExtraLayoutInfo();
        }

        v->setParent(this);
        if (li != v->getExtraLayoutInfo()) {
            v->setExtraLayoutInfo(li);
        }

        if (index == views_.size()) {
            views_.push_back(v);
        } else {
            views_.insert(views_.begin() + index, v);
        }

        ubassert(!v->isAttachedToWindow());
        if (isAttachedToWindow() && !v->isAttachedToWindow()) {
            ubassert(getWindow());
            v->dispatchAttachedToWindow(getWindow());
        }

        if (req_layout) {
            requestLayout();
        }
        requestDraw();
    }

    void LayoutView::removeView(View* v, bool del, bool req_layout) {
        if (!v) {
            DLOG(Log::WARNING) << "You cannot remove a null view from LayoutView.";
            return;
        }

        bool attached = isAttachedToWindow();
        for (auto it = views_.begin(); it != views_.end(); ++it) {
            if ((*it) == v) {
                isolateChild(v, attached, del);
                views_.erase(it);

                if (req_layout) {
                    requestLayout();
                }
                requestDraw();
                return;
            }
        }
    }

    void LayoutView::removeView(size_t index, bool del, bool req_layout) {
        if (index >= views_.size()) {
            DLOG(Log::WARNING) << "Invalid index of View";
            return;
        }

        bool attached = isAttachedToWindow();
        isolateChild(views_[index], attached, del);
        views_.erase(views_.begin() + index);

        if (req_layout) {
            requestLayout();
        }
        requestDraw();
    }

    void LayoutView::removeAllViews(bool del, bool req_layout) {
        bool attached = isAttachedToWindow();
        if (!views_.empty()) {
            for (auto child : views_) {
                isolateChild(child, attached, del);
            }

            views_.clear();

            if (attached) {
                if (req_layout) {
                    requestLayout();
                }
                requestDraw();
            }
        }
    }

    void LayoutView::isolateChild(View* child, bool attached, bool del) {
        child->discardFocus();
        child->discardMouseCapture();
        child->discardTouchCapture();
        child->discardPendingOperations();
        child->resetLayoutStatus();

        if (child->isAttachedToWindow() && attached) {
            child->dispatchDetachFromWindow();
        }
        child->setParent(nullptr);

        if (del) {
            delete child;
        }
    }

    size_t LayoutView::getChildCount() const {
        return views_.size();
    }

    View* LayoutView::getChildById(int id) const {
        for (auto view : views_) {
            if (view->getId() == id) {
                return view;
            }
        }

        return nullptr;
    }

    View* LayoutView::getChildAt(size_t index) const {
        return views_.at(index);
    }

    View* LayoutView::findView(int id) {
        if (getId() == id) {
            return this;
        }

        for (auto view : views_) {
            auto child = view->findView(id);
            if (child) {
                return child;
            }
        }

        return nullptr;
    }

    void LayoutView::invalidateHookStatus() {
        is_hooked_ = false;
    }

    void LayoutView::dispatchDraw(Canvas* canvas) {
        drawChildren(canvas);
    }

    void LayoutView::dispatchAncestorVisibilityChanged(View* ancestor, int visibility) {
        super::dispatchAncestorVisibilityChanged(ancestor, visibility);

        for (auto view : views_) {
            view->dispatchAncestorVisibilityChanged(ancestor, visibility);
        }
    }

    void LayoutView::dispatchAncestorEnableChanged(View* ancestor, bool enabled) {
        super::dispatchAncestorEnableChanged(ancestor, enabled);

        for (auto view : views_) {
            view->dispatchAncestorEnableChanged(ancestor, enabled);
        }
    }

    void LayoutView::dispatchDiscardFocus() {
        for (auto view : views_) {
            view->discardFocus();
        }
    }

    void LayoutView::dispatchDiscardPendingOperations() {
        for (auto view : views_) {
            view->discardPendingOperations();
        }
    }

    bool LayoutView::dispatchPointerEvent(InputEvent* e) {
        bool hooked = false;
        bool consumed = false;
        std::weak_ptr<InputEvent> wptr = cur_ev_;

        e->offsetInputPos(-getX(), -getY());

        /**
         * 在一次输入流程中，拦截一次之后，其余事件将不会再进入
         * onHookInputEvent()
         */
        bool is_first_hooked = !is_hooked_;
        if (is_hooked_ || invokeOnHookInputEvent(e)) {
            if (e->getEvent() == InputEvent::EVT_UP &&
                !cur_ev_->hasTouchEvent(e))
            {
                return true;
            }

            prepareHookingStatus(e);

            hooked = true;
            consumed = sendInputEvent(e, true);
            if (wptr.expired() || !isAttachedToWindow()) {
                return consumed;
            }

            if (is_first_hooked) {
                /**
                 * 第一次拦截时，可以通过在 onInputEvent() 中返回 false 来
                 * 反悔
                 */
                if (consumed) {
                    if (e->getEvent() == InputEvent::EVT_DOWN) {
                        if (isTouchCapturable() &&
                            getWindow()->getTouchHolder() != this)
                        {
                            getWindow()->captureTouch(this);
                        }
                    }
                    updateHookingStatus(e);
                    return consumed;
                }

                hooked = false;
                is_hooked_ = false;
            } else {
                if (is_hooked_) {
                    if (e->getEvent() == InputEvent::EV_LEAVE_VIEW ||
                        e->getEvent() == InputEvent::EVM_UP ||
                        e->getEvent() == InputEvent::EVT_UP)
                    {
                        is_hooked_ = false;
                    }

                    return consumed;
                }
            }
        }

        // 从 View 列表的尾部开始遍历。因为最近添加的 View 在列表尾部，
        // 而最近添加的 View 可能会处于其他之前添加的 View 的上面（在绘制
        //  View 是从列表头开始的），这样一来与坐标相关的事件就可能发生在
        // 多个 View 交叠的区域，此时应该将该事件先发送至最上层的 View ，
        // 为此从 View 列表的尾部开始遍历。
        // 随后根据子 View 的 dispatchInputEvent() 方法的返回值来决定是否将
        // 该事件传递给下层的 View。
        for (size_t i = views_.size(); i > 0; --i) {
            auto child = views_[i - 1];
            if (child->getVisibility() != SHOW || !child->isEnabled()) {
                continue;
            }

            int mx = e->getX();
            int my = e->getY();
            e->offsetInputPos(getScrollX(), getScrollY());

            if (child->isParentPointerInThis(e)) {
                if (!consumed) {
                    consumed = child->dispatchInputEvent(e);
                }
            } else if (child->isReceiveOutsideInputEvent()) {
                if (!consumed) {
                    auto prev_target = getWindow()->getLastInputView();
                    e->setOutside(true);
                    consumed = child->dispatchInputEvent(e);
                    e->setOutside(false);
                    if (!wptr.expired() && isAttachedToWindow()) {
                        getWindow()->setLastInputView(prev_target);
                    }
                }
            }

            e->setX(mx);
            e->setY(my);

            if (wptr.expired() || !isAttachedToWindow()) {
                break;
            }
        }

        if (!consumed && !wptr.expired() && isAttachedToWindow()) {
            if (/*!e->isTouchEvent() && */!hooked) {
                consumed = sendInputEvent(e);
            }
        }

        return consumed;
    }

    bool LayoutView::dispatchKeyboardEvent(InputEvent* e) {
        if (invokeOnHookInputEvent(e)) {
            return invokeOnInputEvent(e);
        }
        // 键盘事件不参与分发。
        return false;
    }

    bool LayoutView::dispatchInputEvent(InputEvent* e) {
        bool consumed;
        if ((e->isMouseEvent() || e->isTouchEvent()) && !e->isNoDispatch()) {
            consumed = dispatchPointerEvent(e);
        } else if (e->isKeyboardEvent()) {
            consumed = dispatchKeyboardEvent(e);
        } else {
            if ((e->getEvent() == InputEvent::EVT_MOVE ||
                e->getEvent() == InputEvent::EVT_UP) &&
                !cur_ev_->hasTouchEvent(e))
            {
                return true;
            }

            // LayoutView 拦截事件后如果获得了鼠标焦点，则
            // 鼠标事件将走到这里而不是 dispatchPointerEvent()，
            // 所以需要在这里进行拦截变量的重置
            if (is_hooked_) {
                prepareHookingStatus(e);
                updateHookingStatus(e);
            }

            consumed = View::dispatchInputEvent(e);
        }

        return consumed;
    }

    void LayoutView::dispatchWindowFocusChanged(bool focus) {
        onWindowFocusChanged(focus);

        for (auto view : views_) {
            view->dispatchWindowFocusChanged(focus);
        }
    }

    void LayoutView::dispatchContextChanged(Context::Type type, const Context& context) {
        onContextChanged(type, context);

        for (auto view : views_) {
            view->dispatchContextChanged(type, context);
        }
    }

    void LayoutView::dispatchAttachedToWindow(Window* w) {
        super::dispatchAttachedToWindow(w);

        for (auto view : views_) {
            ubassert(!view->isAttachedToWindow());
            if (!view->isAttachedToWindow()) {
                view->dispatchAttachedToWindow(w);
            }
        }
    }

    void LayoutView::dispatchDetachFromWindow() {
        for (auto view : views_) {
            ubassert(view->isAttachedToWindow());
            if (view->isAttachedToWindow()) {
                view->dispatchDetachFromWindow();
            }
        }

        super::dispatchDetachFromWindow();
    }

    bool LayoutView::onHookInputEvent(InputEvent* e) {
        return false;
    }

    void LayoutView::prepareHookingStatus(InputEvent* e) {
        // 如果拦截时的事件是触摸事件，
        // 并且不是按下事件的话，将当前消息转为按下事件
        if (e->isTouchEvent() &&
            e->getEvent() == InputEvent::EVT_MOVE &&
            !cur_ev_->hasTouchEvent(e))
        {
            auto saved = e->getEvent();
            e->setEvent(InputEvent::EVT_DOWN);
            sendInputEvent(e, true);
            e->setEvent(saved);
        }
    }

    void LayoutView::updateHookingStatus(InputEvent* e) {
        if (e->getEvent() == InputEvent::EVM_DOWN ||
            e->getEvent() == InputEvent::EVT_DOWN)
        {
            is_hooked_ = true;
        } else if (e->getEvent() == InputEvent::EV_LEAVE_VIEW ||
            e->getEvent() == InputEvent::EVM_UP ||
            e->getEvent() == InputEvent::EVT_UP)
        {
            is_hooked_ = false;
        } else {
            is_hooked_ = true;
        }
    }

    bool LayoutView::invokeOnHookInputEvent(InputEvent* e) {
        if (iie_delegate_) {
            bool ret = false;
            if (iie_delegate_->onHookInputReceived(this, e, &ret)) {
                return ret;
            }
        }
        return onHookInputEvent(e);
    }

    void LayoutView::drawChild(Canvas* canvas, View* child) {
        auto bounds = child->getBounds();
        if (bounds.empty()) {
            return;
        }

        bounds.extend(child->getBoundsExtension());
        child->transformBounds(&bounds);

        if (child->isLayouted() &&
            child->getVisibility() == SHOW &&
            !bounds.empty() && bounds.intersect(dirty_rect_))
        {
            Rect dirty(dirty_rect_);
            dirty.same(bounds);
            dirty.offset(
                -child->getX() + getScrollX(),
                -child->getY() + getScrollY());

            canvas->save();
            canvas->translate(float(child->getX()), float(child->getY()));
            child->draw(canvas, dirty);
            canvas->restore();
        }
    }

    void LayoutView::drawChildren(Canvas* canvas) {
        for (auto view : views_) {
            drawChild(canvas, view);
        }
    }

    void LayoutView::determineChildSize(View* child, const SizeInfo& parent_info) {
        int hori_margin = child->getLayoutMargin().hori();
        int vert_margin = child->getLayoutMargin().vert();
        auto& ls = child->getLayoutSize();

        auto child_w = SizeInfo::getChildSizeInfo(
            parent_info.width(), hori_margin + getPadding().hori(), ls.width());

        auto child_h = SizeInfo::getChildSizeInfo(
            parent_info.height(), vert_margin + getPadding().vert(), ls.height());

        child->determineSize(SizeInfo(child_w, child_h));
    }

    void LayoutView::determineChildrenSize(const SizeInfo& parent_info) {
        for (auto child : views_) {
            if (child->getVisibility() != VANISHED) {
                determineChildSize(child, parent_info);
            }
        }
    }

}
