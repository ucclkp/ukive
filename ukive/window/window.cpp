// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/window/window.h"

#include "utils/log.h"
#include "utils/message/message_pump.h"
#include "utils/time_utils.h"

#include "ukive/app/application.h"
#include "ukive/window/window_native.h"
#include "ukive/window/window_dpi_utils.h"
#include "ukive/views/layout/root_layout.h"
#include "ukive/menu/context_menu.h"
#include "ukive/menu/context_menu_callback.h"
#include "ukive/text/text_action_menu_callback.h"
#include "ukive/text/text_action_menu.h"
#include "ukive/menu/menu.h"
#include "ukive/event/focus_manager.h"
#include "ukive/event/input_event.h"
#include "ukive/event/keyboard.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/cyro_render_target.h"
#include "ukive/diagnostic/statistic_drawer.h"
#include "ukive/views/size_info.h"
#include "ukive/window/window_listener.h"


namespace ukive {

    Window::Window()
        : impl_(WindowNative::create(this)),
          labour_cycler_(nullptr),
          root_layout_(nullptr),
          canvas_(nullptr),
          mouse_holder_(nullptr),
          touch_holder_(nullptr),
          focus_holder_(nullptr),
          focus_holder_backup_(nullptr),
          last_input_view_(nullptr),
          mouse_holder_ref_(0),
          touch_holder_ref_(0),
          background_color_(Color::White),
          is_startup_window_(false),
          min_width_(0),
          min_height_(0)
    {
        WindowManager::getInstance()->addWindow(this);
        WindowManager::getInstance()->addListener(this);
    }

    Window::~Window() {
    }

    bool Window::init(const InitParams& params) {
        // 已经创建过了，无法修改父窗口
        if (isCreated()) {
            return true;
        }

        // 父窗口还没创建
        if (params.parent && !params.parent->isCreated()) {
            return false;
        }

        WindowNative::InitParams native_params;
        if (params.parent) {
            native_params.parent = params.parent->getImpl();
        }
        native_params.frame_type = params.frame_type;

        if (!impl_->init(native_params)) {
            return false;
        }

        parent_ = params.parent;
        return true;
    }

    void Window::show() {
        impl_->show();
    }

    void Window::hide() {
        impl_->hide();
    }

    void Window::minimize() {
        impl_->minimize();
    }

    void Window::maximize() {
        impl_->maximize();
    }

    void Window::restore() {
        impl_->restore();
    }

    void Window::focus() {
        impl_->focus();
    }

    void Window::close() {
        impl_->close();
    }

    void Window::center() {
        impl_->center();
    }

    void Window::setTitle(const std::u16string_view& title) {
        impl_->setTitle(title);
    }

    void Window::setX(int x) {
        auto bounds = impl_->getBounds();
        scaleToNative(impl_.get(), &x);
        impl_->setBounds(x, bounds.y(), bounds.width(), bounds.height());
    }

    void Window::setY(int y) {
        auto bounds = impl_->getBounds();
        scaleToNative(impl_.get(), &y);
        impl_->setBounds(bounds.x(), y, bounds.width(), bounds.height());
    }

    void Window::setPosition(int x, int y) {
        auto bounds = impl_->getBounds();
        scaleToNative(impl_.get(), &x);
        scaleToNative(impl_.get(), &y);
        impl_->setBounds(x, y, bounds.width(), bounds.height());
    }

    void Window::setWidth(int width) {
        auto bounds = impl_->getBounds();
        scaleToNative(impl_.get(), &width);
        impl_->setBounds(bounds.x(), bounds.y(), width, bounds.height());
    }

    void Window::setHeight(int height) {
        auto bounds = impl_->getBounds();
        scaleToNative(impl_.get(), &height);
        impl_->setBounds(bounds.x(), bounds.y(), bounds.width(), height);
    }

    void Window::setBounds(int x, int y, int width, int height) {
        scaleToNative(impl_.get(), &x);
        scaleToNative(impl_.get(), &y);
        scaleToNative(impl_.get(), &width);
        scaleToNative(impl_.get(), &height);
        impl_->setBounds(x, y, width, height);
    }

    void Window::setMinWidth(int min_width) {
        min_width_ = min_width;
    }

    void Window::setMinHeight(int min_height) {
        min_height_ = min_height;
    }

    void Window::setCurrentCursor(Cursor cursor) {
        impl_->setCurrentCursor(cursor);
    }

    void Window::setContentView(int layout_id) {
        root_layout_->setContent(layout_id);
    }

    void Window::setContentView(View* content) {
        root_layout_->setContent(content);
    }

    void Window::setBackgroundColor(const Color& color) {
        background_color_ = color;
        requestDraw();
    }

    void Window::setFullscreen(bool enabled) {
        impl_->setFullscreen(enabled);
    }

    void Window::setResizable(bool enabled) {
        impl_->setResizable(enabled);
    }

    void Window::setMinimizable(bool enabled) {
        impl_->setMinimizable(enabled);
    }

    void Window::setMaximizable(bool enabled) {
        impl_->setMaximizable(enabled);
    }

    void Window::setClosable(bool enabled) {
        impl_->setClosable(enabled);
    }

    void Window::setCloseMethods(uint32_t methods) {
        impl_->setCloseMethods(methods);
    }

    void Window::setKeepOnTop(bool enabled) {
        impl_->setKeepOnTop(enabled);
    }

    void Window::setShowInTaskBar(bool enabled) {
        impl_->setShowInTaskBar(enabled);
    }

    void Window::setIgnoreMouseEvents(bool ignore) {
        impl_->setIgnoreMouseEvents(ignore);
    }

    void Window::setTranslucentType(TranslucentType type) {
        impl_->setTranslucentType(type);
    }

    void Window::setStartupWindow(bool enable) {
        is_startup_window_ = enable;
    }

    void Window::setLastInputView(View* v) {
        last_input_view_ = v;
    }

    void Window::setOwnership(bool myself) {
        is_own_by_myself_ = myself;
    }

    void Window::setPurpose(const Purpose& p) {
        purpose_ = p;
    }

    Rect Window::getBounds() const {
        auto bounds = impl_->getBounds();
        scaleFromNative(impl_.get(), &bounds);
        return bounds;
    }

    Rect Window::getContentBounds() const {
        auto bounds = impl_->getContentBounds();
        scaleFromNative(impl_.get(), &bounds);
        return bounds;
    }

    int Window::getMinWidth() const {
        return min_width_;
    }

    int Window::getMinHeight() const {
        return min_height_;
    }

    std::u16string Window::getTitle() const {
        return impl_->getTitle();
    }

    RootLayout* Window::getRootLayout() const {
        return root_layout_;
    }

    Color Window::getBackgroundColor() const {
        return background_color_;
    }

    utl::Cycler* Window::getCycler() const {
        return labour_cycler_;
    }

    Canvas* Window::getCanvas() const {
        return canvas_;
    }

    WindowFrameType Window::getFrameType() const {
        return impl_->getFrameType();
    }

    View* Window::getLastInputView() const {
        return last_input_view_;
    }

    View* Window::getContentView() const {
        return root_layout_->getContentView();
    }

    TitleBar* Window::getTitleBar() const {
        return root_layout_->getTitleBar();
    }

    WindowNative* Window::getImpl() const {
        return impl_.get();
    }

    bool Window::getOwnership() const {
        return is_own_by_myself_;
    }

    Context Window::getContext() const {
        return context_;
    }

    const Purpose& Window::getPurpose() const {
        return purpose_;
    }

    TranslucentType Window::getTranslucentType() const {
        return context_.getTranslucentType();
    }

    uint32_t Window::getCloseMethods() const {
        return impl_->getCloseMethods();
    }

    Window* Window::getParent() const {
        return parent_;
    }

    bool Window::isCreated() const {
        return impl_->isCreated();
    }

    bool Window::isShowing() const {
        return impl_->isShowing();
    }

    bool Window::isStartupWindow() const {
        return is_startup_window_;
    }

    bool Window::isMinimized() const {
        return impl_->isMinimized();
    }

    bool Window::isMaximized() const {
        return impl_->isMaximized();
    }

    bool Window::isTitleBarShowing() const {
        return root_layout_->isTitleBarShowing();
    }

    bool Window::isHDREnabled() const {
        return is_hdr_enabled_;
    }

    bool Window::isFullscreen() const {
        return impl_->isFullscreen();
    }

    bool Window::isResizable() const {
        return impl_->isResizable();
    }

    bool Window::isMinimizable() const {
        return impl_->isMinimizable();
    }

    bool Window::isMaximizable() const {
        return impl_->isMaximizable();
    }

    bool Window::isClosable() const {
        return impl_->isClosable();
    }

    bool Window::isKeepOnTop() const {
        return impl_->isKeepOnTop();
    }

    bool Window::isShowInTaskBar() const {
        return impl_->isShowInTaskBar();
    }

    bool Window::isIgnoreMouseEvents() const {
        return impl_->isIgnoreMouseEvents();
    }

    bool Window::hasSizeBorder() const {
        return impl_->hasSizeBorder();
    }

    void Window::showTitleBar() {
        root_layout_->showTitleBar();
    }

    void Window::hideTitleBar() {
        root_layout_->hideTitleBar();
    }

    void Window::removeTitleBar() {
        root_layout_->removeTitleBar();
    }

    void Window::addStatusChangedListener(OnWindowStatusChangedListener* l) {
        utl::addCallbackTo(status_changed_listeners_, l);
    }

    void Window::removeStatusChangedListener(OnWindowStatusChangedListener* l) {
        utl::removeCallbackFrom(status_changed_listeners_, l);
    }

    const std::vector<OnWindowStatusChangedListener*>& Window::getStatusChangedListeners() const {
        return status_changed_listeners_;
    }

    void Window::convScreenToClient(Point* p) const {
        Point pt(*p);
        scaleToNative(impl_.get(), &pt);
        impl_->convScreenToClient(&pt);
        scaleFromNative(impl_.get(), &pt);
        *p = pt;
    }

    void Window::convClientToScreen(Point* p) const {
        Point pt(*p);
        scaleToNative(impl_.get(), &pt);
        impl_->convClientToScreen(&pt);
        scaleFromNative(impl_.get(), &pt);
        *p = pt;
    }

    void Window::captureMouse(View* v) {
        if (!v || touch_holder_ref_) {
            return;
        }

        // 当已存在有捕获鼠标的 View 时，若此次调用该方法的 View
        // 与之前不同，此次调用将被忽略。在使用中应避免此种情况产生。
        if (mouse_holder_ref_ && v != mouse_holder_) {
            jour_de("abnormal capture mouse!!");
            return;
        }

        ++mouse_holder_ref_;

        // 该 View 第一次捕获鼠标。
        if (mouse_holder_ref_ == 1) {
            impl_->setMouseCapture();
            mouse_holder_ = v;
        }
    }

    void Window::releaseMouse(bool all) {
        if (mouse_holder_ref_ == 0) {
            return;
        }

        if (all) {
            mouse_holder_ref_ = 0;
        } else {
            --mouse_holder_ref_;
        }

        // 鼠标将被释放。
        if (mouse_holder_ref_ == 0) {
            impl_->releaseMouseCapture();
            mouse_holder_ = nullptr;
        }
    }

    void Window::captureTouch(View* v) {
        if (!v || mouse_holder_ref_) {
            return;
        }

        // 当已存在有捕获触摸的 View 时，若此次调用该方法的 View
        // 与之前不同，此次调用将被忽略。在使用中应避免此种情况产生。
        if (touch_holder_ref_ && v != touch_holder_) {
            jour_de("abnormal capture touch!!");
            return;
        }

        ++touch_holder_ref_;

        // 该 View 第一次捕获触摸。
        if (touch_holder_ref_ == 1) {
            impl_->setMouseCapture();
            touch_holder_ = v;
        }
    }

    void Window::releaseTouch(bool all) {
        if (touch_holder_ref_ == 0) {
            return;
        }

        if (all) {
            touch_holder_ref_ = 0;
        } else {
            --touch_holder_ref_;
        }

        // 鼠标将被释放。
        if (touch_holder_ref_ == 0) {
            impl_->releaseMouseCapture();
            touch_holder_ = nullptr;
        }
    }

    void Window::captureKeyboard(View* v) {
        focus_holder_ = v;
    }

    void Window::releaseKeyboard() {
        focus_holder_ = nullptr;
    }

    View* Window::getMouseHolder() const {
        return mouse_holder_;
    }

    int Window::getMouseHolderRef() const {
        return mouse_holder_ref_;
    }

    View* Window::getTouchHolder() const {
        return touch_holder_;
    }

    int Window::getTouchHolderRef() const {
        return touch_holder_ref_;
    }

    View* Window::getKeyboardHolder() const {
        return focus_holder_;
    }

    void Window::requestDraw() {
        if (isMinimized()) {
            return;
        }
        cur_dirty_region_.setOne(getContentBounds());

        auto dirty_rect(cur_dirty_region_);
        scaleToNative(impl_.get(), &dirty_rect.rect0);
        scaleToNative(impl_.get(), &dirty_rect.rect1);

        impl_->invalidate(dirty_rect);
    }

    void Window::requestDraw(const Rect& rect) {
        if (isMinimized() || rect.empty()) {
            return;
        }

        auto nor_rect = rect;
        nor_rect.same(getContentBounds());

        cur_dirty_region_.add(nor_rect);

        auto dirty_rect(cur_dirty_region_);
        scaleToNative(impl_.get(), &dirty_rect.rect0);
        scaleToNative(impl_.get(), &dirty_rect.rect1);

        impl_->invalidate(dirty_rect);
    }

    void Window::requestLayout() {
        impl_->requestLayout();
    }

    View* Window::findView(int id) const {
        return root_layout_->findView(id);
    }

    ContextMenu* Window::startContextMenu(
        ContextMenuCallback* callback, View* anchor, int gravity)
    {
        auto context_menu = new ContextMenu(this, callback);

        if (!callback->onCreateContextMenu(
            context_menu, context_menu->getMenu())) {
            delete context_menu;
            return nullptr;
        }

        callback->onPrepareContextMenu(
            context_menu, context_menu->getMenu());

        if (context_menu->getMenu()->getItemCount() == 0) {
            delete context_menu;
            return nullptr;
        }

        context_menu_.reset(context_menu);
        context_menu_->show(anchor, gravity);
        return context_menu;
    }

    TextActionMenu* Window::startTextActionMenu(TextActionMenuCallback* callback) {
        auto action_menu = new TextActionMenu(this, callback);
        if (!callback->onCreateActionMode(
            action_menu, action_menu->getMenu())) {
            delete action_menu;
            return nullptr;
        }

        callback->onPrepareActionMode(
            action_menu, action_menu->getMenu());

        if (action_menu->getMenu()->getItemCount() == 0) {
            delete action_menu;
            return nullptr;
        }

        text_action_menu_.reset(action_menu);
        text_action_menu_->show();

        return action_menu;
    }

    void Window::onCreate() {
    }

    void Window::onCreated() {
        labour_cycler_ = new utl::Cycler();

        root_layout_ = new RootLayout(context_);
        root_layout_->setId(0);
        root_layout_->setLayoutSize(View::LS_FILL, View::LS_FILL);

        float dpi = context_.getDefaultDpi() * context_.getAutoScale();

        buffer_ = WindowBuffer::create(this);
        ImageOptions options(dpi, dpi);
        //options.pixel_format = ImagePixelFormat::HDR;
        buffer_->onCreate(0, 0, options);

        rt_ = CyroRenderTarget::create();
        rt_->onCreate(buffer_);

        canvas_ = new Canvas(rt_);

        root_layout_->dispatchAttachedToWindow(this);
    }

    void Window::onShow(bool show) {
    }

    void Window::onActivate(bool activate) {
        //DLOG(Log::INFO) << "onActivate: " << reason;

        if (activate) {
            /*if (focus_holder_backup_)
            focus_holder_backup_->requestFocus();*/
        } else {
            /*if (focus_holder_)
            {
            InputEvent ev;
            ev.setEvent(InputEvent::EVENT_CANCEL);

            focus_holder_backup_ = focus_holder_;
            focus_holder_backup_->dispatchInputEvent(&ev);
            focus_holder_->discardFocus();
            }*/
        }
    }

    void Window::onSetFocus() {
        //DLOG(Log::INFO) << "onSetFocus";
        if (root_layout_) {
            root_layout_->dispatchWindowFocusChanged(true);
        }
    }

    void Window::onKillFocus() {
        //DLOG(Log::INFO) << "onKillFocus";
        if (!root_layout_) {
            return;
        }

        while (mouse_holder_ref_ > 0) {
            releaseMouse();
        }
        root_layout_->dispatchWindowFocusChanged(false);
    }

    void Window::onSetText(const std::u16string& text) {
        for (auto listener : status_changed_listeners_) {
            listener->onWindowTextChanged(text);
        }
    }

    void Window::onSetIcon() {
        for (auto listener : status_changed_listeners_) {
            listener->onWindowIconChanged();
        }
    }

    void Window::onLayout() {
        if (!impl_->isCreated()) {
            return;
        }

        auto bounds = getContentBounds();
        auto& layout_size = root_layout_->getLayoutSize();

        int x = bounds.x();
        int y = bounds.y();
        int width = bounds.width();
        int height = bounds.height();

        SizeInfo::Mode width_mode;
        SizeInfo::Mode height_mode;
        if (layout_size.width() < 0) {
            switch (layout_size.width()) {
            case View::LS_AUTO:
                width_mode = SizeInfo::CONTENT;
                break;
            case View::LS_FREE:
                width_mode = SizeInfo::FREEDOM;
                break;
            case View::LS_FILL:
            default:
                width_mode = SizeInfo::DEFINED;
                break;
            }
        } else {
            width = layout_size.width();
            width_mode = SizeInfo::DEFINED;
        }

        if (layout_size.height() < 0) {
            switch (layout_size.height()) {
            case View::LS_AUTO:
                height_mode = SizeInfo::CONTENT;
                break;
            case View::LS_FREE:
                height_mode = SizeInfo::FREEDOM;
                break;
            case View::LS_FILL:
            default:
                height_mode = SizeInfo::DEFINED;
                break;
            }
        } else {
            height = layout_size.height();
            height_mode = SizeInfo::DEFINED;
        }

        uint64_t micro = 0;
        bool enable_ui_debug = (debug_drawer_ && debug_drawer_->getMode() == StatisticDrawer::Mode::LAYOUT);
        if (enable_ui_debug) {
            micro = utl::TimeUtils::upTimeMicros();
        }

        SizeInfo size_info;
        size_info.setWidth(SizeInfo::Value(width, width_mode));
        size_info.setHeight(SizeInfo::Value(height, height_mode));
        root_layout_->determineSize(size_info);

        auto& determined_size = root_layout_->getDeterminedSize();

        root_layout_->layout(Rect(x, y, determined_size.width(), determined_size.height()));

        if (enable_ui_debug) {
            auto duration = utl::TimeUtils::upTimeMicros() - micro;
            debug_drawer_->addDuration(duration);
        }
    }

    void Window::onDraw(const DirtyRegion& region) {
        if (!impl_->isCreated() || region.empty()) {
            return;
        }

        if (canvas_) {
            if (debug_drawer_) {
                drawWithDebug(region);
            } else {
                draw(region);
            }
        }
    }

    void Window::draw(const DirtyRegion& region) {
        canvas_->beginDraw();

        drawRootView(canvas_, region.rect0);
        drawRootView(canvas_, region.rect1);

        // 处理设备丢失
        if (canvas_->endDraw() == GRet::Retry) {
            processDeviceLost();
        }
    }

    void Window::drawWithDebug(const DirtyRegion& region) {
        auto bounds = getContentBounds();
        // 创建离屏缓冲，将窗口内容画在这里
        if (!off_canvas_) {
            off_canvas_ = std::make_unique<Canvas>(
                canvas_->getWidth(), canvas_->getHeight(),
                canvas_->getBuffer()->getImageOptions());
        }

        uint64_t micro = 0;
        bool enable_ui_debug = debug_drawer_->getMode() == StatisticDrawer::Mode::RENDER;
        if (enable_ui_debug) {
            micro = utl::TimeUtils::upTimeMicros();
        }

        // 先在离屏画布上画
        off_canvas_->beginDraw();

        drawRootView(off_canvas_.get(), region.rect0);
        drawRootView(off_canvas_.get(), region.rect1);

        off_canvas_->endDraw();

        auto img = off_canvas_->extractImage();

        // 把离屏画布上的内容画到窗口上
        canvas_->beginDraw();
        canvas_->clear();
        canvas_->drawImage(img.get());

        if (enable_ui_debug) {
            auto duration = utl::TimeUtils::upTimeMicros() - micro;
            debug_drawer_->addDuration(duration);
        }

        if (debug_drawer_) {
            // 标出更新区域
            Color color = Color::Pink300;
            color.a = 0.4f;
            canvas_->fillRect(RectF(region.rect0), color);
            canvas_->fillRect(RectF(region.rect1), color);

            debug_drawer_->draw(
                bounds.x(), bounds.y(),
                bounds.width(), bounds.height(), canvas_);
        }

        // 处理设备丢失
        if (canvas_->endDraw() == GRet::Retry) {
            processDeviceLost();
        }
    }

    void Window::drawRootView(Canvas* canvas, const Rect& rect) {
        if (rect.empty()) {
            return;
        }

        auto bounds = getContentBounds();

        // 先裁剪，rect 为窗口坐标
        canvas->pushClip(RectF(rect));
        canvas->clear(background_color_);

        // 平移画布，避开超出窗口可见区的部分
        canvas->save();
        canvas->translate(float(bounds.x()), float(bounds.y()));

        onPreDrawCanvas(canvas);

        auto rl_bounds = root_layout_->getBounds();
        rl_bounds.extend(root_layout_->getBoundsExtension());
        root_layout_->transformBounds(&rl_bounds);

        if (root_layout_->isLayouted() &&
            root_layout_->getVisibility() == View::SHOW &&
            !rl_bounds.empty() && rl_bounds.intersect(rect))
        {
            Rect dirty(rect);
            dirty.same(rl_bounds);
            dirty.offset(-root_layout_->getX(), -root_layout_->getY());
            root_layout_->draw(canvas, dirty);
        }

        onPostDrawCanvas(canvas);

        canvas->restore();
        canvas->popClip();
    }

    void Window::processDeviceLost() {
        onUpdateContext(Context::DEV_LOST);
        Application::getGraphicDeviceManager()->recreate();
        onUpdateContext(Context::DEV_RESTORE);
    }

    void Window::onMove(int x, int y) {
    }

    void Window::onResize(int type, int width, int height) {
        if (!impl_->isCreated()) {
            return;
        }

        if (rt_) {
            GRet ret = rt_->onResize(0, 0);
            if (ret == GRet::Failed) {
                LOG(Log::ERR) << "Resize canvas failed.";
                return;
            }

            // 处理设备丢失
            if (ret == GRet::Retry) {
                processDeviceLost();
            }
        }

        off_canvas_.reset();

        switch (type) {
        case WINDOW_RESIZE_RESTORED:
        case WINDOW_RESIZE_MINIMIZED:
        case WINDOW_RESIZE_MAXIMIZED:
            for (auto listener : status_changed_listeners_) {
                listener->onWindowStatusChanged();
            }
            break;
        default:
            break;
        }

        onLayout();
    }

    void Window::onMoving() {
    }

    bool Window::onResizing(Size* new_size) {
        new_size->width((std::max)(new_size->width(), min_width_));
        new_size->height((std::max)(new_size->height(), min_height_));
        return true;
    }

    bool Window::onClose() {
        if (isStartupWindow()) {
            size_t count = WindowManager::getInstance()->getWindowCount();
            for (size_t i = 0; i < count;) {
                auto window = WindowManager::getInstance()->getWindow(i);
                if (!window->isStartupWindow()) {
                    window->close();
                } else {
                    ++i;
                }
                count = WindowManager::getInstance()->getWindowCount();
            }
        }
        return true;
    }

    void Window::onDestroy() {
        context_menu_.reset();
        text_action_menu_.reset();

        root_layout_->dispatchDetachFromWindow();
        delete root_layout_;
        root_layout_ = nullptr;

        delete canvas_;
        canvas_ = nullptr;
        rt_ = nullptr;
        buffer_ = nullptr;

        delete labour_cycler_;
        labour_cycler_ = nullptr;

        WindowManager::getInstance()->removeListener(this);
        WindowManager::getInstance()->removeWindow(this);

        if (isStartupWindow()) {
            utl::MessagePump::quit();
        }
    }

    void Window::onDestroyed() {
        if (is_own_by_myself_) {
            delete this;
        }
    }

    bool Window::processPointerHolder(View* holder, InputEvent* e) {
        if (e->getEvent() == InputEvent::EVM_LEAVE_WIN &&
            e->isTouchEvent())
        {
            return false;
        }

        // 若有之前捕获过 Poiner 的 View 存在，则直接将所有 Poiner 事件
        // 直接发送至该 View。
        if (holder &&
            holder->getVisibility() == View::SHOW &&
            holder->isEnabled())
        {
            // 进行坐标变换，将目标 View 左上角映射为(0, 0)。
            int total_left = 0;
            int total_top = 0;
            auto parent = holder->getParent();
            while (parent) {
                total_left += (parent->getX() - parent->getScrollX());
                total_top += (parent->getY() - parent->getScrollY());

                parent = parent->getParent();
            }

            e->setX(e->getX() - total_left);
            e->setY(e->getY() - total_top);
            e->setIsNoDispatch(true);

            if (e->getEvent() == InputEvent::EVM_LEAVE_WIN) {
                e->setEvent(InputEvent::EV_LEAVE_VIEW);
                return holder->dispatchInputEvent(e);
            }

            /**
             * 对于已经获取了 Poiner 焦点的 View，如果当前事件处理后释放了 Poiner 焦点，
             * 并且此时 Poiner 已不在该 View 内，那么应向该 View 发送离开事件。
             * （Poiner 离开事件在正常情况下是由 LayoutView 产生的，但对于获取了
             * Poiner 焦点的 View，事件走不到 LayoutView 中，无法利用这种机制）
             */
            auto canary = holder->getCanary();
            auto prev_holder = holder;
            bool ret = holder->dispatchInputEvent(e);
            if (!canary.expired() &&
                holder != prev_holder &&
                !prev_holder->isLocalPointerInThisVisible(e))
            {
                e->setEvent(InputEvent::EV_LEAVE_VIEW);
                prev_holder->dispatchInputEvent(e);
            }
            return ret;
        }

        if (e->getEvent() == InputEvent::EVM_LEAVE_WIN) {
            if (last_input_view_) {
                e->setEvent(InputEvent::EV_LEAVE_VIEW);
                e->setIsNoDispatch(true);
                last_input_view_->dispatchInputEvent(e);
                return false;
            }
        }

        return root_layout_->dispatchInputEvent(e);
    }

    bool Window::onInputEvent(InputEvent* e) {
        if (e->isMouseEvent()) {
            return processPointerHolder(mouse_holder_, e);
        }

        if (e->isTouchEvent()) {
            return processPointerHolder(touch_holder_, e);
        }

        if (e->isKeyboardEvent()) {
            // debug view
            if (e->getEvent() == InputEvent::EVK_DOWN && e->getKeyboardKey() == Keyboard::KEY_Q) {
                bool is_shift_key_pressed = Keyboard::isKeyPressed(Keyboard::KEY_SHIFT);
                bool is_ctrl_key_pressed = Keyboard::isKeyPressed(Keyboard::KEY_CONTROL);
                if (is_ctrl_key_pressed && is_shift_key_pressed) {
                    if (debug_drawer_) {
                        debug_drawer_.reset();
                        off_canvas_.reset();
                    } else {
                        debug_drawer_ = std::make_unique<StatisticDrawer>(getContext());
                    }
                    requestDraw();
                } else if (is_ctrl_key_pressed && !is_shift_key_pressed) {
                    if (debug_drawer_) {
                        debug_drawer_->toggleMode();
                        requestDraw();
                    }
                }
            }

            // TODO：某些 View 需要提前处理按键。
            // TODO：快捷键支持
            if (FocusManager::focusOn(this)->onInputEvent(e)) {
                return true;
            }

            if (focus_holder_) {
                return focus_holder_->dispatchInputEvent(e);
            }
        }

        return false;
    }

    HitPoint Window::onNCHitTest(int x, int y) {
        x -= root_layout_->getX();
        y -= root_layout_->getY();
        return root_layout_->onNCHitTest(x, y);
    }

    void Window::onWindowButtonChanged(WindowButton button) {
        for (auto listener : status_changed_listeners_) {
            listener->onWindowButtonChanged(button);
        }
    }

    bool Window::onGetWindowIconName(std::u16string* icon_name, std::u16string* small_icon_name) const {
        return false;
    }

    void Window::onPostLayout() {
        labour_cycler_->removeMessages(SCHEDULE_LAYOUT);
        labour_cycler_->post([this]()
        {
            impl_->doLayout();
        }, SCHEDULE_LAYOUT);
    }

    void Window::onPostRender() {
        labour_cycler_->removeMessages(SCHEDULE_RENDER);
        labour_cycler_->post([this]()
        {
            if (!impl_->isCreated()) {
                return;
            }

            // 脏区域使用窗口坐标
            auto dirty_region = cur_dirty_region_;
            cur_dirty_region_.clear();

            auto dirty_rect(dirty_region);
            scaleToNative(impl_.get(), &dirty_rect.rect0);
            scaleToNative(impl_.get(), &dirty_rect.rect1);

            impl_->doDraw(dirty_rect);
        }, SCHEDULE_RENDER);
    }

    Context Window::onGetContext() const {
        return context_;
    }

    void Window::onUpdateContext(Context::Type type) {
        if (type == Context::NONE) {
            return;
        }

        if (type == Context::DPI_CHANGED &&
            Application::getOptions().is_auto_dpi_scale)
        {
            float dpi = context_.getDefaultDpi() * context_.getAutoScale();
            if (buffer_) {
                buffer_->onDPIChange(dpi, dpi);
            }
        }

        onContextChanged(type, context_);
        root_layout_->dispatchContextChanged(type, context_);

        requestLayout();
        requestDraw();
    }

    void Window::onWindowAdded(Window* w) {
        ubassert(w != this);
    }

    void Window::onWindowRemoved(Window* w) {
        ubassert(w != this);

        if (w == parent_) {
            parent_ = nullptr;
        }
    }

}
