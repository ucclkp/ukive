// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "small_title_bar.h"

#include "ukive/views/text_view.h"
#include "ukive/window/window.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/views/title_bar/title_bar_button.h"
#include "ukive/elements/element.h"
#include "ukive/system/theme_info.h"


namespace ukive {

    SmallTitleBar::SmallTitleBar(Context c)
        : SmallTitleBar(c, {})
    {
        setLayoutSize(LS_FILL, c.dp2pxi(30));
    }

    SmallTitleBar::SmallTitleBar(Context c, AttrsRef attrs)
        : super(c, attrs),
          title_tv_(nullptr)
    {
        Color color;
        determineColor(&color);

        setShadowRadius(c.dp2pxi(4));
        setBackground(new Element(color));
        initViews();
    }

    SmallTitleBar::~SmallTitleBar() {
    }

    void SmallTitleBar::setColor(const Color& c) {
        getBackground()->setSolidColor(c);
        requestDraw();
    }

    void SmallTitleBar::setThemeColorEnabled(bool enabled) {
        if (is_theme_color_enabled_ != enabled) {
            is_theme_color_enabled_ = enabled;

            Color color;
            determineColor(&color);
            setColor(color);
        }
    }

    bool SmallTitleBar::isThemeColorEnabled() const {
        return is_theme_color_enabled_;
    }

    void SmallTitleBar::initViews() {
        using Rlp = RestraintLayoutInfo;
        auto c = getContext();

        auto root_layout = new RestraintLayout(c);
        root_layout->setLayoutSize(LS_FILL, LS_FILL);
        addView(root_layout);

        title_tv_ = new TextView(c);
        title_tv_->setTextSize(c.dp2pxi(12));
        title_tv_->setTextColor(Color::White);
        title_tv_->setLayoutMargin(c.dp2pxi(12), 0, 0, 0);
        auto title_tv_lp = Rlp::Builder()
            .start(root_layout->getId())
            .top(root_layout->getId())
            .bottom(root_layout->getId()).build();
        title_tv_->setExtraLayoutInfo(title_tv_lp);
        root_layout->addView(title_tv_);

        // Buttons
        close_btn_ = new TitleBarButton(c);
        close_btn_->setOnClickListener(this);
        close_btn_->setType(WindowButton::Close);
        close_btn_->setLayoutSize(c.dp2pxi(46), View::LS_FILL);
        auto close_btn_lp = Rlp::Builder()
            .end(root_layout->getId())
            .top(root_layout->getId())
            .bottom(root_layout->getId()).build();
        close_btn_->setExtraLayoutInfo(close_btn_lp);
        root_layout->addView(close_btn_);

        max_btn_ = new TitleBarButton(c);
        max_btn_->setOnClickListener(this);
        max_btn_->setType(WindowButton::Max);
        max_btn_->setLayoutSize(c.dp2pxi(46), View::LS_FILL);
        auto max_btn_lp = Rlp::Builder()
            .end(close_btn_->getId(), Rlp::START)
            .top(root_layout->getId())
            .bottom(root_layout->getId()).build();
        max_btn_->setExtraLayoutInfo(max_btn_lp);
        root_layout->addView(max_btn_);

        min_btn_ = new TitleBarButton(c);
        min_btn_->setOnClickListener(this);
        min_btn_->setType(WindowButton::Min);
        min_btn_->setLayoutSize(c.dp2pxi(46), View::LS_FILL);
        auto min_btn_lp = Rlp::Builder()
            .end(max_btn_->getId(), Rlp::START)
            .top(root_layout->getId())
            .bottom(root_layout->getId()).build();
        min_btn_->setExtraLayoutInfo(min_btn_lp);
        root_layout->addView(min_btn_);
    }

    void SmallTitleBar::onClick(View* v) {
        auto w = getWindow();
        if (!w) {
            return;
        }

        if (v == min_btn_) {
            w->minimize();
        } else if (v == max_btn_) {
            if (w->isMaximized()) {
                w->restore();
            } else {
                w->maximize();
            }
        } else if (v == close_btn_) {
            w->close();
        }
    }

    void SmallTitleBar::onWindowTextChanged(const std::u16string& text) {
        title_tv_->setText(text);
    }

    void SmallTitleBar::onWindowIconChanged() {
    }

    void SmallTitleBar::onWindowStatusChanged() {
        if (getWindow()->isMaximized()) {
            max_btn_->setType(WindowButton::Restore);
        } else {
            max_btn_->setType(WindowButton::Max);
        }
    }

    void SmallTitleBar::onWindowButtonChanged(WindowButton button) {
        switch (button) {
        case WindowButton::Min:
            min_btn_->setVisibility(
                getWindow()->isMinimizable() ? SHOW : VANISHED);
            break;
        case WindowButton::Max:
            max_btn_->setVisibility(
                getWindow()->isMaximizable() ? SHOW : VANISHED);
            break;
        case WindowButton::Close:
            close_btn_->setVisibility(
                getWindow()->isClosable() ? SHOW : VANISHED);
            break;
        default:
            break;
        }
    }

    void SmallTitleBar::onAttachedToWindow(Window* w) {
        super::onAttachedToWindow(w);
        title_tv_->setText(getWindow()->getTitle());
    }

    void SmallTitleBar::onContextChanged(Context::Type type, const Context& context) {
        super::onContextChanged(type, context);

        if (!is_theme_color_enabled_ ||
            type != Context::THEME_CHANGED)
        {
            return;
        }

        auto& config = context.getCurrentThemeConfig();
        switch (config.type) {
        case ThemeConfig::COLOR_CHANGED:
        {
            auto color = config.primary_color;
            if (color.a < 0.3f) color.a = 0.3f;
            setColor(color);
            break;
        }

        case ThemeConfig::COLOR_EXISTANCE:
        {
            Color color;
            determineColor(&color);
            setColor(color);
            break;
        }

        default: break;
        }
    }

    HitPoint SmallTitleBar::onNCHitTest(int x, int y) {
        if (min_btn_->getBounds().hit(x, y)) {
            return HitPoint::CLIENT;
        }
        if (max_btn_->getBounds().hit(x, y)) {
            return HitPoint::CLIENT;
        }
        if (close_btn_->getBounds().hit(x, y)) {
            return HitPoint::CLIENT;
        }
        return HitPoint::CAPTION;
    }

    void SmallTitleBar::determineColor(Color* c) {
        if (is_theme_color_enabled_) {
            auto& config = getContext().getCurrentThemeConfig();
            if (config.has_color) {
                *c = config.primary_color;
                if (c->a < 0.3f) c->a = 0.3f;
            } else {
                *c = Color::Blue800;
            }
        } else {
            *c = Color::Blue800;
            c->a = 0.7f;
        }

        //*c = Color(1, 1, 1, 0.5f);
    }

}
