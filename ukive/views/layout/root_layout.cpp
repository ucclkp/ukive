// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "root_layout.h"

#include <typeinfo>

#include "ukive/views/layout/simple_layout.h"
#include "ukive/views/layout/shade_layout.h"
#include "ukive/window/window.h"
#include "ukive/views/title_bar/default_title_bar.h"
#include "ukive/resources/layout_instantiator.h"


namespace {

    class ContentFrameView : public ukive::SimpleLayout {
    public:
        using super = SimpleLayout;
        using SimpleLayout::SimpleLayout;

        ukive::Size onDetermineSize(const ukive::SizeInfo& info) override {
            View* title_bar;
            View* content_view;
            if (getChildCount() > 1) {
                title_bar = getChildAt(1);
                content_view = getChildAt(0);
            } else if (getChildCount() == 1) {
                title_bar = nullptr;
                content_view = getChildAt(0);
            } else {
                title_bar = nullptr;
                content_view = nullptr;
            }

            if (!content_view ||
                !title_bar ||
                title_bar->getVisibility() == VANISHED)
            {
                if (content_view) {
                    content_view->setLayoutMargin(0, 0, 0, 0);
                }
                return super::onDetermineSize(info);
            }

            determineChildSize(title_bar, info);
            content_view->setLayoutMargin(0, title_bar->getDeterminedSize().height(), 0, 0);

            determineChildSize(content_view, info);
            return getWrappedSize(info);
        }
    };

}

namespace ukive {

    RootLayout::RootLayout(Context c)
        : RootLayout(c, {}) {}

    RootLayout::RootLayout(Context c, AttrsRef attrs)
        : super(c, attrs),
          title_bar_(nullptr),
          shade_layout_(nullptr),
          content_layout_(nullptr),
          content_view_(nullptr),
          shade_added_(false)
    {
        content_layout_ = new ContentFrameView(c);
        content_layout_->setLayoutSize(LS_FILL, LS_FILL);
        addView(content_layout_);

        shade_layout_ = new ShadeLayout(c);
        shade_layout_->setLayoutSize(LS_FILL, LS_FILL);
    }

    RootLayout::~RootLayout() {
        if (!shade_added_) {
            delete shade_layout_;
        }
    }

    TitleBar* RootLayout::showTitleBar() {
        if (!title_bar_) {
            title_bar_ = new DefaultTitleBar(getContext());
            title_bar_->setLayoutSize(LS_FILL, LS_AUTO);
            content_layout_->addView(title_bar_);
        } else {
            title_bar_->setVisibility(SHOW);
        }
        return title_bar_;
    }

    void RootLayout::hideTitleBar() {
        if (title_bar_) {
            title_bar_->setVisibility(VANISHED);
        }
    }

    void RootLayout::removeTitleBar() {
        if (title_bar_) {
            content_layout_->removeView(title_bar_);
            title_bar_ = nullptr;
        }
    }

    void RootLayout::addShade(View* shade, const ShadeParams& params) {
        auto li = new ShadeLayoutInfo();
        li->params = params;
        shade->setExtraLayoutInfo(li);

        shade_layout_->addView(shade);
        if (shade_layout_->getChildCount() == 1) {
            addView(shade_layout_);
            shade_added_ = true;
        }
    }

    bool RootLayout::updateShade(View* shade, const ShadeParams& params) {
        for (auto v : *shade_layout_) {
            if (v == shade) {
                auto li = static_cast<ShadeLayoutInfo*>(v->getExtraLayoutInfo());
                if (li->params != params) {
                    li->params = params;
                    shade_layout_->requestLayout();
                    requestDraw();
                }
                return true;
            }
        }
        return false;
    }

    bool RootLayout::getShadeParams(View* shade, ShadeParams* params) {
        for (auto v : *shade_layout_) {
            if (v == shade) {
                auto li = static_cast<ShadeLayoutInfo*>(v->getExtraLayoutInfo());
                *params = li->params;
                return true;
            }
        }
        return false;
    }

    void RootLayout::removeShade(View* shade) {
        shade_layout_->removeView(shade, false);
        if (shade_layout_->getChildCount() == 0) {
            shade_layout_->setBackground(nullptr);
            removeView(shade_layout_, false);
            shade_added_ = false;
        }
    }

    void RootLayout::setShadeBackground(Element* element, bool owned) {
        shade_layout_->setBackground(element, owned);
    }

    bool RootLayout::isTitleBarShowing() const {
        return title_bar_ && title_bar_->getVisibility() == SHOW;
    }

    TitleBar* RootLayout::getTitleBar() const {
        return title_bar_;
    }

    View* RootLayout::getContentView() const {
        return content_view_;
    }

    void RootLayout::setContent(int id) {
        auto content_view = LayoutInstantiator::from(getContext(), content_layout_, id);
        setContent(content_view);
    }

    void RootLayout::setContent(View* content) {
        if (content == content_view_ || !content) {
            return;
        }

        if (content_view_) {
            content_layout_->removeView(content_view_);
        }

        content_view_ = content;
        content->setLayoutSize(LS_FILL, LS_FILL);
        content_layout_->addView(0, content);
    }

    void RootLayout::requestDrawRelParent(const Rect& rect) {
        super::requestDrawRelParent(rect);
        auto w = getWindow();
        if (w) {
            w->requestDraw(rect);
        }
    }

    void RootLayout::requestLayout() {
        super::requestLayout();
        auto w = getWindow();
        if (w) {
            w->requestLayout();
        }
    }

    View* RootLayout::findView(int id) {
        return content_layout_->findView(id);
    }

    HitPoint RootLayout::onNCHitTest(int x, int y) {
        auto hit_point = super::onNCHitTest(x, y);
        if (hit_point != HitPoint::CLIENT) {
            return hit_point;
        }

        if (shade_layout_ &&
            shade_layout_->getVisibility() == SHOW)
        {
            auto bounds = shade_layout_->getBounds();
            if (bounds.hit(x, y)) {
                if (shade_layout_->hitChildren(x - bounds.x(), y - bounds.y())) {
                    return HitPoint::CLIENT;
                }
            }
        }

        if (title_bar_ &&
            title_bar_->getVisibility() == SHOW)
        {
            x -= content_layout_->getX();
            y -= content_layout_->getY();
            auto bounds = title_bar_->getBounds();
            if (bounds.hit(x, y)) {
                hit_point = title_bar_->onNCHitTest(x - bounds.x(), y - bounds.y());
            }
        }
        return hit_point;
    }

}