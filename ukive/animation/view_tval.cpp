// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "view_tval.h"

#include "ukive/views/view.h"
#include "ukive/views/layout_info/shade_layout_info.h"


namespace ukive {

    // ViewTVal
    ViewTVal::ViewTVal() {}
    ViewTVal::ViewTVal(tvalcr s, tvalcr e)
        : start_val_(s),
          end_val_(e) {}

    void ViewTVal::set(tvalcr tv) {
        start_val_ = tv;
        end_val_ = tv;
        is_evaluated_ = false;
    }

    void ViewTVal::set(tvalcr s, tvalcr e) {
        start_val_ = s;
        end_val_ = e;
        is_evaluated_ = false;
    }

    void ViewTVal::reset() {
        start_val_ = tval();
        end_val_ = tval();
        is_evaluated_ = false;
        real_val_ = 0;
    }

    // static
    double ViewTVal::resolve(View* v, tvalcr tv) {
        double real_start;
        switch (tv.type()) {
        case tval::vt::absolute:
            real_start = tv.absolute();
            break;
        case tval::vt::width:
            real_start = v->getWidth() * tv.width();
            break;
        case tval::vt::height:
            real_start = v->getHeight() * tv.height();
            break;
        case tval::vt::diagonal:
        {
            auto w = v->getWidth();
            auto h = v->getHeight();
            real_start = std::sqrt(w*w + h*h) * tv.diagonal();
            break;
        }
        default:
            real_start = 0;
            break;
        }
        return real_start;
    }

    double ViewTVal::getStart(View* v) const {
        return resolve(v, start_val_);
    }

    double ViewTVal::getEnd(View* v) const {
        return resolve(v, end_val_);
    }

    double ViewTVal::get(View* v, double interpolation) const {
        double s = getStart(v);
        double e = getEnd(v);
        return s + (e - s) * interpolation;
    }

    void ViewTVal::calculate(View* v, double interpolation) {
        real_val_ = get(v, interpolation);
        is_evaluated_ = true;
    }

    double ViewTVal::getRV(View* v) const {
        if (is_evaluated_) {
            return real_val_;
        }
        return getStart(v);
    }


    // ViewRevealTVals
    ViewRevealTVals::ViewRevealTVals()
        : type_(Type::None) {}

    void ViewRevealTVals::setCircle(
        tvalcr center_x, tvalcr center_y,
        tvalcr start_radius, tvalcr end_radius)
    {
        type_ = Type::Circle;
        x_.set(center_x);
        y_.set(center_y);
        width_.set(start_radius, end_radius);
        is_evaluated_ = false;
    }

    void ViewRevealTVals::setRect(
        tvalcr x, tvalcr y,
        tvalcr start_width, tvalcr end_width,
        tvalcr start_height, tvalcr end_height)
    {
        type_ = Type::Rect;
        x_.set(x);
        y_.set(y);
        width_.set(start_width, end_width);
        height_.set(start_height, end_height);
        is_evaluated_ = false;
    }

    void ViewRevealTVals::reset() {
        type_ = Type::None;
        is_evaluated_ = false;
    }

    RectD ViewRevealTVals::getStart(View* v) const {
        RectD out;
        if (type_ == Type::Circle) {
            out.pos(
                resolveX(v, x_.getStartTVal(), 0),
                resolveY(v, y_.getStartTVal(), 0));
            out.size(resolveRadius(v, width_.getStartTVal()), 0);
        } else {
            out.size(
                resolveWidth(v, width_.getStartTVal()),
                resolveHeight(v, height_.getStartTVal()));
            out.pos(
                resolveX(v, x_.getStartTVal(), out.width()),
                resolveY(v, y_.getStartTVal(), out.height()));
        }
        return out;
    }

    RectD ViewRevealTVals::getEnd(View* v) const {
        RectD out;
        if (type_ == Type::Circle) {
            out.pos(
                resolveX(v, x_.getEndTVal(), 0),
                resolveY(v, y_.getEndTVal(), 0));
            out.size(resolveRadius(v, width_.getEndTVal()), 0);
        } else {
            out.size(
                resolveWidth(v, width_.getEndTVal()),
                resolveHeight(v, height_.getEndTVal()));
            out.pos(
                resolveX(v, x_.getEndTVal(), out.width()),
                resolveY(v, y_.getEndTVal(), out.height()));
        }
        return out;
    }

    RectD ViewRevealTVals::get(View* v, double interpolation) const {
        auto s = getStart(v);
        auto e = getEnd(v);

        RectD out;
        out.x(s.x() + (e.x() - s.x()) * interpolation);
        out.y(s.y() + (e.y() - s.y()) * interpolation);
        out.width(s.width()  + (e.width()  - s.width())  * interpolation);
        out.height(s.height() + (e.height() - s.height()) * interpolation);
        return out;
    }

    void ViewRevealTVals::calculate(View* v, double interpolation) {
        real_val_ = get(v, interpolation);
        is_evaluated_ = true;
    }

    RectD ViewRevealTVals::getRV(View* v) const {
        if (is_evaluated_) {
            return real_val_;
        }
        return getStart(v);
    }

    // static
    double ViewRevealTVals::resolveX(View* v, tvalcr x_tv, double width) {
        if (x_tv.type() != tval::vt::automatic) {
            return ViewTVal::resolve(v, x_tv);
        }

        auto li = v->getExtraLayoutInfo();
        if (!li || typeid(*li) != typeid(ShadeLayoutInfo)) {
            return 0;
        }

        auto sli = static_cast<ShadeLayoutInfo*>(li);
        if (sli->adj_gravity & GV_START) {
            return v->getWidth() - width;
        }
        if (sli->adj_gravity & GV_MID_START) {
            return 0;
        }
        if (sli->adj_gravity & GV_MID_HORI) {
            return (v->getWidth() - width) / 2.0;
        }
        if (sli->adj_gravity & GV_MID_END) {
            return v->getWidth() - width;
        }

        if (sli->adj_corner & GV_START) {
            return 0;
        }
        if (sli->adj_corner & GV_END) {
            return v->getWidth() - width;
        }
        return 0;
    }

    // static
    double ViewRevealTVals::resolveY(View* v, tvalcr y_tv, double height) {
        if (y_tv.type() != tval::vt::automatic) {
            return ViewTVal::resolve(v, y_tv);
        }

        auto li = v->getExtraLayoutInfo();
        if (!li || typeid(*li) != typeid(ShadeLayoutInfo)) {
            return 0;
        }

        auto sli = static_cast<ShadeLayoutInfo*>(li);
        if (sli->adj_gravity & GV_TOP) {
            return v->getHeight() - height;
        }
        if (sli->adj_gravity & GV_MID_TOP) {
            return 0;
        }
        if (sli->adj_gravity & GV_MID_VERT) {
            return (v->getHeight() - height) / 2.0;
        }
        if (sli->adj_gravity & GV_MID_BOTTOM) {
            return v->getHeight() - height;
        }

        if (sli->adj_corner & GV_TOP) {
            return 0;
        }
        if (sli->adj_corner & GV_BOTTOM) {
            return v->getWidth() - height;
        }
        return 0;
    }

    // static
    double ViewRevealTVals::resolveRadius(View* v, tvalcr r_tv) {
        enum class PosType {
            Corner,
            CenterHori,
            CenterVert,
            Center,
        };

        static PosType pos[3][3]{
            { PosType::Corner,     PosType::CenterHori, PosType::Corner     },
            { PosType::CenterVert, PosType::Center,     PosType::CenterVert },
            { PosType::Corner,     PosType::CenterHori, PosType::Corner     },
        };

        if (r_tv.type() != tval::vt::automatic) {
            return ViewTVal::resolve(v, r_tv);
        }

        auto li = v->getExtraLayoutInfo();
        if (!li || typeid(*li) != typeid(ShadeLayoutInfo)) {
            return 0;
        }

        auto sli = static_cast<ShadeLayoutInfo*>(li);
        size_t row, col;
        if (sli->adj_gravity & GV_START) {
            col = 2;
        } else if (sli->adj_gravity & GV_MID_START) {
            col = 0;
        } else if (sli->adj_gravity & GV_MID_HORI) {
            col = 1;
        } else if (sli->adj_gravity & GV_MID_END) {
            col = 2;
        } else {
            col = 0;
        }

        if (sli->adj_gravity & GV_TOP) {
            row = 2;
        } else if (sli->adj_gravity & GV_MID_TOP) {
            row = 0;
        } else if (sli->adj_gravity & GV_MID_VERT) {
            row = 1;
        } else if (sli->adj_gravity & GV_MID_BOTTOM) {
            row = 2;
        } else {
            row = 0;
        }

        switch (pos[row][col]) {
        case PosType::Corner:
        {
            auto w = v->getWidth();
            auto h = v->getHeight();
            return std::sqrt(w*w + h * h);
        }
        case PosType::CenterHori:
        {
            auto w = v->getWidth() / 2.0;
            auto h = v->getHeight();
            return std::sqrt(w*w + h * h);
        }
        case PosType::CenterVert:
        {
            auto w = v->getWidth();
            auto h = v->getHeight() / 2.0;
            return std::sqrt(w*w + h * h);
        }
        case PosType::Center:
        {
            auto w = v->getWidth() / 2.0;
            auto h = v->getHeight() / 2.0;
            return std::sqrt(w*w + h * h);
        }
        default:
            return 0;
        }
    }

    // static
    double ViewRevealTVals::resolveWidth(View* v, tvalcr w_tv) {
        if (w_tv.type() != tval::vt::automatic) {
            return ViewTVal::resolve(v, w_tv);
        }
        return v->getWidth();
    }

    // static
    double ViewRevealTVals::resolveHeight(View* v, tvalcr h_tv) {
        if (h_tv.type() != tval::vt::automatic) {
            return ViewTVal::resolve(v, h_tv);
        }
        return v->getHeight();
    }

}