// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_BASICS_LEVITATOR_H_
#define UKIVE_BASICS_LEVITATOR_H_

#include "ukive/views/layout/simple_layout.h"
#include "ukive/views/layout_info/gravity.h"


namespace ukive {

    class Canvas;
    class Window;
    class InputEvent;
    class Element;
    class Levitator;

    class OnLeviatorEventListener {
    public:
        virtual ~OnLeviatorEventListener() = default;

        virtual void onBeforeInnerWindowLayout(
            Levitator* lev, Rect* new_bounds, const Rect& old_bounds) {}
        virtual void onRequestDismissByTouchOutside(Levitator* lev) {}
    };

    /**
     * 模拟窗口。
     */
    class Levitator {
        class LevFrameView : public SimpleLayout {
        public:
            explicit LevFrameView(Levitator* lev, Context c);
            ~LevFrameView();

            Size onDetermineSize(const SizeInfo& info) override;
            void onBeforeLayout(Rect* new_bounds, const Rect& old_bounds) override;
            void onLayout(const Rect& new_bounds, const Rect& old_bounds) override;

            bool dispatchInputEvent(InputEvent* e) override;

            bool onHookInputEvent(InputEvent* e) override;
            bool onInputEvent(InputEvent* e) override;

        private:
            using super = SimpleLayout;

            Levitator* levitator_;
        };

    public:
        struct PosInfo {
            int corner;
            bool is_evaded;
            bool is_max_visible;
            Padding pp;
            Rect limit_range;
            View* limit_view;
            View* rel_view;

            PosInfo()
                : corner(GV_NONE),
                  is_evaded(false),
                  is_max_visible(true),
                  limit_view(nullptr),
                  rel_view(nullptr) {}
        };

        struct SnapInfo {
            bool is_max_visible;
            bool is_discretized;

            SnapInfo()
                : is_max_visible(true),
                  is_discretized(true) {}
        };

        Levitator();
        virtual ~Levitator();

        void setLayoutWidth(int width);
        void setLayoutHeight(int height);
        void setLayoutSize(int width, int height);
        void setLayoutMargin(const Margin& margin);
        void setShadowRadius(int radius);
        void setBackground(Element* element);
        void setOutsideTouchable(bool touchable);
        void setInputEnabled(bool enabled);

        /**
         * 在点击外围区域时关闭本窗口。
         * 此选项只有在 setOutsideTouchable() 设置为 false 时有效。
         * 当 {enable} 设置为 true 时，点击外围区域将触发 onRequestDismissByTouchOutside()
         * 回调，而且也仅仅是触发该回调，而不做其他事。
         */
        void setDismissByTouchOutside(bool enable);

        void setContentView(View* v);
        void setEventListener(OnLeviatorEventListener* l);

        int getLayoutWidth() const;
        int getLayoutHeight() const;
        int getShadowRadius() const;
        Element* getBackground() const;
        bool isOutsideTouchable() const;
        bool isDismissByTouchOutside() const;
        bool isInputEnabled() const;
        View* getContentView() const;
        View* getFrameView() const;

        bool isShowing() const;

        void show(Window* w, int x, int y, const PosInfo& info = {});
        void show(View* anchor, int gravity, const SnapInfo& info = {});
        void update(int x, int y);
        void update(int x, int y, const PosInfo& info);
        void update(View* anchor, int gravity);
        void update(View* anchor, int gravity, const SnapInfo& info);
        void dismissing();
        void dismiss();

    private:
        void createFrameView(Context c);

        void leaveFromInside();
        void leaveFromOutside();

        int width_;
        int height_;
        int shadow_radius_;
        bool outside_touchable_;
        bool dismiss_by_touch_outside_;
        bool input_enabled_ = true;
        Margin margin_;
        Element* background_;

        Window* window_ = nullptr;
        View* content_view_;
        LayoutView* frame_view_;
        OnLeviatorEventListener* listener_ = nullptr;
        bool is_showing_;
        bool is_dismissing_ = false;
    };

}

#endif  // UKIVE_BASICS_LEVITATOR_H_
