// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_

#include "ukive/views/layout/non_client_layout.h"
#include "ukive/views/layout_info/shade_layout_info.h"


namespace ukive {

    class Color;
    class Window;
    class LayoutInfo;
    class SimpleLayout;
    class SequenceLayout;
    class ShadeLayout;
    class TitleBar;

    class RootLayout : public NonClientLayout {
    public:
        explicit RootLayout(Context c);
        RootLayout(Context c, AttrsRef attrs);
        ~RootLayout();

        TitleBar* showTitleBar();
        void hideTitleBar();
        void removeTitleBar();

        void addShade(View* shade, const ShadeParams& params);
        bool updateShade(View* shade, const ShadeParams& params);
        bool getShadeParams(View* shade, ShadeParams* params);
        void removeShade(View* shade);
        void setShadeBackground(Element* element, bool owned = true);

        bool isTitleBarShowing() const;
        TitleBar* getTitleBar() const;
        View* getContentView() const;

        void setContent(int id);
        void setContent(View* content);

        void requestDrawRelParent(const Rect& rect) override;
        void requestLayout() override;
        View* findView(int id) override;
        HitPoint onNCHitTest(int x, int y) override;

    private:
        typedef NonClientLayout super;
        TitleBar* title_bar_;
        ShadeLayout* shade_layout_;
        SimpleLayout* content_layout_;
        View* content_view_;

        bool shade_added_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_