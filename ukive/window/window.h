// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WINDOW_H_
#define UKIVE_WINDOW_WINDOW_H_

#include <memory>
#include <vector>

#include "utils/message/cycler.h"
#include "utils/multi_callbacks.hpp"

#include "ukive/graphics/dirty_region.h"
#include "ukive/graphics/size.hpp"
#include "ukive/system/theme_info.h"
#include "ukive/views/layout_info/gravity.h"
#include "ukive/views/view.h"
#include "ukive/window/context.h"
#include "ukive/window/purpose.h"
#include "ukive/window/window_manager.h"
#include "ukive/window/window_native_delegate.h"
#include "ukive/window/window_types.h"


namespace ukive {

    class Canvas;
    class InputEvent;
    class WindowNative;
    class StatisticDrawer;
    class WindowBuffer;
    class CyroRenderer;
    class TitleBar;
    class RootLayout;
    class ContextMenu;
    class ContextMenuCallback;
    class TextActionMenu;
    class TextActionMenuCallback;
    class OnWindowStatusChangedListener;
    struct ThemeConfig;

    class Window :
        public WindowManageListener,
        public WindowNativeDelegate
    {
    public:
        struct InitParams {
            Window* parent = nullptr;
            WindowFrameType frame_type = WINDOW_FRAME_CUSTOM;
        };

        Window();
        virtual ~Window();

        bool init(const InitParams& params);
        void show();
        void hide();
        void minimize();
        void maximize();
        void restore();
        void focus();
        void close();
        void center();

        void setTitle(const std::u16string_view& title);
        void setX(int x);
        void setY(int y);
        void setPosition(int x, int y);
        void setWidth(int width);
        void setHeight(int height);
        void setBounds(int x, int y, int width, int height);
        void setMinWidth(int min_width);
        void setMinHeight(int min_height);
        void setCurrentCursor(Cursor cursor);
        void setContentView(int layout_id);
        void setContentView(View* content);
        void setBackgroundColor(const Color& color);
        void setFullscreen(bool enabled);
        void setResizable(bool enabled);
        void setMinimizable(bool enabled);
        void setMaximizable(bool enabled);
        void setClosable(bool enabled);
        void setCloseMethods(uint32_t methods);
        void setKeepOnTop(bool enabled);
        void setShowInTaskBar(bool enabled);
        void setIgnoreMouseEvents(bool ignore);
        void setTranslucentType(TranslucentType type);
        void setStartupWindow(bool enable);
        void setLastInputView(View* v);
        void setOwnership(bool myself);
        void setPurpose(const Purpose& p);

        Rect getBounds() const;
        Rect getContentBounds() const;
        int getMinWidth() const;
        int getMinHeight() const;
        std::u16string getTitle() const;
        RootLayout* getRootLayout() const;
        Color getBackgroundColor() const;
        utl::Cycler* getCycler() const;
        Canvas* getCanvas() const;
        WindowFrameType getFrameType() const;
        View* getLastInputView() const;
        View* getContentView() const;
        TitleBar* getTitleBar() const;
        WindowNative* getImpl() const;
        bool getOwnership() const;
        Context getContext() const;
        const Purpose& getPurpose() const;
        TranslucentType getTranslucentType() const;
        uint32_t getCloseMethods() const;
        Window* getParent() const;

        bool isCreated() const;
        bool isShowing() const;
        bool isStartupWindow() const;
        bool isMinimized() const;
        bool isMaximized() const;
        bool isTitleBarShowing() const;
        bool isHDREnabled() const;
        bool isFullscreen() const;
        bool isResizable() const;
        bool isMinimizable() const;
        bool isMaximizable() const;
        bool isClosable() const;
        bool isKeepOnTop() const;
        bool isShowInTaskBar() const;
        bool isIgnoreMouseEvents() const;
        bool hasSizeBorder() const;

        void showTitleBar();
        void hideTitleBar();
        void removeTitleBar();

        void addStatusChangedListener(OnWindowStatusChangedListener* l);
        void removeStatusChangedListener(OnWindowStatusChangedListener* l);
        const std::vector<OnWindowStatusChangedListener*>& getStatusChangedListeners() const;

        void convScreenToClient(Point* p) const;
        void convClientToScreen(Point* p) const;

        void captureMouse(View* v);
        void releaseMouse(bool all = false);

        void captureTouch(View* v);
        void releaseTouch(bool all = false);

        // 当一个 View 获取到焦点时，应调用此方法。
        void captureKeyboard(View* v);
        // 当一个 View 放弃焦点时，应调用此方法。
        void releaseKeyboard();

        View* getMouseHolder() const;
        int getMouseHolderRef() const;
        View* getTouchHolder() const;
        int getTouchHolderRef() const;
        View* getKeyboardHolder() const;

        void requestDraw();
        void requestDraw(const Rect& rect);
        void requestLayout();

        View* findView(int id) const;

        template <typename T>
        T* findView(int id) const {
            return static_cast<T*>(findView(id));
        }

        ContextMenu* startContextMenu(
            ContextMenuCallback* callback, View* anchor, int gravity);
        TextActionMenu* startTextActionMenu(TextActionMenuCallback* callback);

    protected:
        // WindowNativeDelegate
        void onCreate() override;
        void onCreated() override;
        void onShow(bool show) override;
        void onActivate(bool activate) override;
        void onSetFocus() override;
        void onKillFocus() override;
        void onSetText(const std::u16string& text) override;
        void onSetIcon() override;
        void onLayout() override;
        void onDraw(const DirtyRegion& region) override;
        void onMove(int x, int y) override;
        void onResize(int type, int width, int height) override;
        void onMoving() override;
        bool onResizing(Size* new_size) override;
        bool onClose() override;
        void onDestroy() override;
        void onDestroyed() override;
        bool onInputEvent(InputEvent* e) override;
        HitPoint onNCHitTest(int x, int y) override;
        void onWindowButtonChanged(WindowButton button) override;
        bool onGetWindowIconName(std::u16string* icon_name, std::u16string* small_icon_name) const override;
        void onPostLayout() override;
        void onPostRender() override;
        Context onGetContext() const override;
        void onUpdateContext() override;

        virtual void onContextChanged(const Context& context) {}
        virtual void onPreDrawCanvas(Canvas* canvas) {}
        virtual void onPostDrawCanvas(Canvas* canvas) {}

        // WindowManageListener
        void onWindowAdded(Window* w) override;
        void onWindowRemoved(Window* w) override;

    private:
        enum {
            SCHEDULE_RENDER = 0,
            SCHEDULE_LAYOUT = 1,
        };

        void draw(const DirtyRegion& region);
        void drawWithDebug(const DirtyRegion& region);
        void drawRootView(Canvas* canvas, const Rect& rect);
        void processDeviceLost();
        bool processPointerHolder(View* holder, InputEvent* e);

        Context context_;
        std::unique_ptr<WindowNative> impl_;

        utl::Cycler* labour_cycler_;
        RootLayout* root_layout_;

        Canvas* canvas_;
        WindowBuffer* buffer_ = nullptr;
        std::shared_ptr<CyroRenderer> renderer_;

        View* mouse_holder_;
        View* touch_holder_;
        View* focus_holder_;
        View* focus_holder_backup_;
        View* last_input_view_;
        int mouse_holder_ref_;
        int touch_holder_ref_;
        Window* parent_ = nullptr;

        std::unique_ptr<ContextMenu> context_menu_;
        std::unique_ptr<TextActionMenu> text_action_menu_;
        std::vector<OnWindowStatusChangedListener*> status_changed_listeners_;

        Color background_color_;
        bool is_startup_window_;
        bool is_hdr_enabled_ = false;
        bool is_own_by_myself_ = false;
        int min_width_, min_height_;
        Purpose purpose_;

        DirtyRegion cur_dirty_region_;
        std::unique_ptr<Canvas> off_canvas_;
        std::unique_ptr<StatisticDrawer> debug_drawer_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_H_