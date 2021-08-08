// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MENU_INNER_WINDOW_H_
#define UKIVE_MENU_INNER_WINDOW_H_

#include "ukive/views/layout/simple_layout.h"
#include "ukive/views/layout_info/gravity.h"


namespace ukive {

    class Canvas;
    class Window;
    class InputEvent;
    class Element;
    class InnerWindow;

    class OnInnerWindowEventListener {
    public:
        virtual ~OnInnerWindowEventListener() = default;

        virtual void onBeforeInnerWindowLayout(
            InnerWindow* iw, Rect* new_bounds, const Rect& old_bounds) {}
        virtual void onRequestDismissByTouchOutside(InnerWindow* iw) {}
    };

    /**
     * 模拟窗口。
     */
    class InnerWindow {
    public:
        class InnerDecorView : public SimpleLayout {
        public:
            explicit InnerDecorView(InnerWindow* inner, Context c);
            ~InnerDecorView();

            Size onDetermineSize(const SizeInfo& info) override;
            void onBeforeLayout(Rect* new_bounds, const Rect& old_bounds) override;

            bool dispatchInputEvent(InputEvent* e) override;

            bool onHookInputEvent(InputEvent* e) override;
            bool onInputEvent(InputEvent* e) override;

        private:
            using super = SimpleLayout;

            InnerWindow* inner_window_;
        };

        InnerWindow();
        virtual ~InnerWindow();

        void setWidth(int width);
        void setHeight(int height);
        void setSize(int width, int height);
        void setShadowRadius(int radius);
        void setBackground(Element* element);
        void setOutsideTouchable(bool touchable);

        /**
         * 在点击外围区域时关闭本窗口。
         * 此选项只有在 setOutsideTouchable() 设置为 false 时有效。
         * 当 {enable} 设置为 true 时，点击外围区域将触发 onRequestDismissByTouchOutside()
         * 回调，而且也仅仅是触发该回调，而不做其他事。
         */
        void setDismissByTouchOutside(bool enable);

        void setContentView(View* v);
        void setEventListener(OnInnerWindowEventListener* l);

        int getWidth() const;
        int getHeight() const;
        int getShadowRadius() const;
        Element* getBackground() const;
        bool isOutsideTouchable() const;
        bool isDismissByTouchOutside() const;
        View* getContentView() const;
        View* getDecorView() const;

        bool isShowing() const;

        void show(Window* w, int x, int y);
        void show(View* anchor, int gravity);
        void update(int x, int y);
        void update(View* anchor, int gravity);
        void markDismissing();
        void dismiss();

    private:
        void createDecorView(Context c);

        int width_;
        int height_;
        int shadow_radius_;
        bool outside_touchable_;
        bool dismiss_by_touch_outside_;
        Element* background_;

        Window* window_ = nullptr;
        View* content_view_;
        InnerDecorView* decor_view_;
        OnInnerWindowEventListener* listener_ = nullptr;
        bool is_showing_;
        bool is_marked_as_dismissing_ = false;
    };

}

#endif  // UKIVE_MENU_INNER_WINDOW_H_