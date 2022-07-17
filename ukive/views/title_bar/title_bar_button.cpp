// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/title_bar/title_bar_button.h"

#include <algorithm>

#include "ukive/elements/ripple_element.h"
#include "ukive/window/window.h"
#include "ukive/graphics/canvas.h"


namespace ukive {

    TitleBarButton::TitleBarButton(Context c)
        : TitleBarButton(c, {}) {}

    TitleBarButton::TitleBarButton(Context c, AttrsRef attrs)
        : super(c, attrs)
    {
        setClickable(true);

        bg_ = new RippleElement(Color::White);
        bg_->setDrawMaskEnabled(false);

        setBackground(bg_);
        graph_size_ = c.dp2pxri(9);
        graph_stroke_ = c.dp2pxi(1);
        restore_bias_ = c.dp2pxi(2);
    }

    void TitleBarButton::setGraphSize(int size) {
        if (size != graph_size_) {
            graph_size_ = size;
            requestDraw();
        }
    }

    void TitleBarButton::setType(WindowButton btn) {
        if (btn != btn_) {
            btn_ = btn;

            switch (btn_) {
            case WindowButton::Close:
                bg_->setInteractColor(Color::Red500);
                bg_->setDownAlpha(1);
                bg_->setHoverAlpha(0.7);
                break;

            default:
                bg_->setInteractColor(Color::White);
                //bg_->setDownAlpha(1);
                //bg_->setHoverAlpha(0.7);
                break;
            }

            requestDraw();
        }
    }

    Size TitleBarButton::onDetermineSize(const SizeInfo& info) {
        int final_w, final_h;
        switch (info.width().mode) {
        case SizeInfo::CONTENT:
            final_w = (std::min)(info.width().val, getPadding().hori());
            break;

        case SizeInfo::DEFINED:
            final_w = info.width().val;
            break;

        case SizeInfo::FREEDOM:
        default:
            final_w = getPadding().hori();
            break;
        }

        switch (info.height().mode) {
        case SizeInfo::CONTENT:
            final_h = (std::min)(info.height().val, getPadding().vert());
            break;

        case SizeInfo::DEFINED:
            final_h = info.height().val;
            break;

        case SizeInfo::FREEDOM:
        default:
            final_h = getPadding().vert();
            break;
        }

        return Size(final_w, final_h);
    }

    void TitleBarButton::onDraw(Canvas* c) {
        super::onDraw(c);

        auto bounds = getContentBounds();
        auto cp = bounds.pos_center();
        Rect graph_bounds{
            cp.x() - graph_size_ / 2,
            cp.y() - graph_size_ / 2,
            graph_size_,
            graph_size_ };
        auto gbf = RectF(graph_bounds);

        switch (btn_) {
        case WindowButton::Close:
            c->drawLine(gbf.pos(), gbf.pos_rb(), graph_stroke_, Color::White);
            c->drawLine(gbf.pos_rt(), gbf.pos_lb(), graph_stroke_, Color::White);
            break;

        case WindowButton::Max:
            gbf.offset(0.5f, 0.5f);
            c->drawRect(gbf, graph_stroke_, Color::White);
            break;

        case WindowButton::Restore:
        {
            gbf.offset(0.5f, 0.5f);

            auto b1 = gbf;
            b1.y(b1.y() + restore_bias_);
            b1.size(b1.width() - restore_bias_, b1.height() - restore_bias_);
            c->drawRect(b1, graph_stroke_, Color::White);

            auto b2 = gbf;
            b2.x(b2.x() + restore_bias_);
            b2.size(b2.width() - restore_bias_, b2.height() - restore_bias_);
            c->drawLine(b2.pos(), b2.pos_rt(), graph_stroke_, Color::White);
            c->drawLine(b2.pos_rb(), b2.pos_rt(), graph_stroke_, Color::White);
            break;
        }

        case WindowButton::Min:
            c->drawLine(
                { gbf.x(), float(cp.y() + 0.5f) },
                { gbf.right(), float(cp.y() + 0.5f) },
                graph_stroke_, Color::White);
            break;
        }
    }

}
