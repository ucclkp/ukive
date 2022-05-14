// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "grid_view.h"

#include <cmath>

#include "utils/strings/int_conv.hpp"
#include "utils/weak_bind.hpp"

#include "ukive/event/input_event.h"
#include "ukive/event/keyboard.h"
#include "ukive/graphics/canvas.h"
#include "ukive/text/text_layout.h"
#include "ukive/window/window.h"
#include "ukive/graphics/path.h"


namespace {
    const int kLineWidth = 1;
    const int kInitSqLength = 16;
    const int kMaxScaleLevel = 11;
}

namespace ukive {

    GridView::GridView(Context c)
        : GridView(c, {}) {}

    GridView::GridView(Context c, AttrsRef attrs)
        : View(c, attrs),
          new_nav_(this),
          org_nav_(this),
          weak_ref_nest_(this)
    {
        length_ = getContext().dp2pxi(kInitSqLength);
        init_length_ = length_;

        line_width_ = getAdjustedLineWidth(kLineWidth);

        coord_tl_.reset(TextLayout::create());
        color_tl_.reset(TextLayout::create());

        auto text_color = Color::White;
        text_color.a *= 0.75f;
        auto bg_color = Color::Blue400;
        bg_color.a *= 0.75f;

        new_nav_.create(u"N", text_color, bg_color);

        text_color = Color::Black;
        text_color.a *= 0.75f;
        bg_color = Color::White;
        bg_color.a *= 0.75f;

        org_nav_.create(u"O", text_color, bg_color);

        setFocusable(true);
    }

    void GridView::setFlippedY(bool flipped_y) {
        if (flipped_y != flipped_y_) {
            flipped_y_ = flipped_y;
            requestDraw();
        }
    }

    void GridView::addPixel(int col, int row, const Color& color) {
        PixelInfo info{ col, row, color };

        bool hit = false;
        for (auto& p : pixels_) {
            if (p.col == col && p.row == row) {
                p = info;
                hit = true;
                break;
            }
        }

        if (!hit) {
            pixels_.push_back(info);
        }

        startVSync();
        animator_.start();
        showNewNav();
        requestDraw();
    }

    bool GridView::getPixel(int col, int row, Color* color) const {
        for (auto& p : pixels_) {
            if (p.col == col && p.row == row) {
                *color = p.color;
                return true;
            }
        }
        return false;
    }

    void GridView::clearPixels() {
        pixels_.clear();
        animator_.finish();
        requestDraw();
    }

    void GridView::addPoint(
        double col, double row, int radius, const Color& color)
    {
        PointInfo info{ col, row, radius, color };
        points_.push_back(info);
        requestDraw();
    }

    void GridView::addPoint(
        double col, double row, int radius, const Color& color,
        const std::u16string_view& text, const Color& text_color)
    {
        PointInfo info{ col, row, radius, color, nullptr, text_color };

        info.tl.reset(TextLayout::create());
        info.tl->make(
            text, u"Consolas", float(getContext().dp2pxi(12)),
            TextLayout::FontStyle::NORMAL, TextLayout::FontWeight::NORMAL, u"en-US");
        info.tl->setTextWrapping(TextLayout::TextWrapping::NONE);

        TextLayout::TextMetrics m;
        if (info.tl->getTextMetrics(&m)) {
            info.tl->setMaxWidth(m.rect.width());
        }

        points_.push_back(info);
        requestDraw();
    }

    void GridView::clearPoints() {
        points_.clear();
        requestDraw();
    }

    void GridView::addLine(
        const PointD& p1, const PointD& p2, const Color& color)
    {
        LineInfo info{ false, p1, {}, p2, nullptr, color };
        lines_.push_back(info);
        requestDraw();
    }

    void GridView::addQBezierLine(
        const PointD& p1, const PointD& p2, const PointD& p3, const Color& color)
    {
        LineInfo info{ true, p1, p2, p3, nullptr, color };
        lines_.push_back(info);
        requestDraw();
    }

    void GridView::clearLines() {
        lines_.clear();
        requestDraw();
    }

    void GridView::showOverlay(bool show) {
        if (show) {
            showNewNav();
            showOrgNav();
        } else {
            new_nav_.closeNav();
            org_nav_.closeNav();
        }
    }

    Size GridView::onDetermineSize(const SizeInfo& info) {
        return getPreferredSize(info, 0, 0);
    }

    void GridView::onAfterLayout(const Rect& new_bounds, const Rect& old_bounds) {
        super::onAfterLayout(new_bounds, old_bounds);

        if (new_bounds == old_bounds) {
            return;
        }

        showNewNav();
        showOrgNav();

        for (auto& info : lines_) {
            info.qb.reset();
        }
    }

    void GridView::onDraw(Canvas* canvas) {
        super::onDraw(canvas);

        drawGridLines(canvas);

        auto bounds = getContentBounds();

        Point p;
        getGridPoint(0, 0, &p);
        PointF pf(p);

        canvas->drawLine(
            PointF{ 0, pf.y() },
            PointF{ float(bounds.width()), pf.y() },
            line_width_, Color::Black);
        canvas->drawLine(
            PointF{ pf.x(), 0 },
            PointF{ pf.x(), float(bounds.height()) },
            line_width_, Color::Black);
        canvas->fillCircle(
            pf, float(getContext().dp2pxi(4)), Color(0, 0, 0, 1));

        size_t index = 0;
        for (const auto& info : pixels_) {
            drawGridPixel(
                canvas,
                info.col, info.row, info.color,
                index + 1 == pixels_.size());
            ++index;
        }

        for (auto& info : lines_) {
            drawGridLine(canvas, info);
        }

        for (const auto& info : points_) {
            drawGridPoint(canvas, info);
        }

        if (has_text_ && scale_level_ >= 0) {
            drawGridText(canvas, text_col_, text_row_);
        }
    }

    bool GridView::onInputEvent(InputEvent* e) {
        bool result = super::onInputEvent(e);

        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
            result = true;
            is_moving_ = false;
            start_x_ = prev_x_ = e->getX();
            start_y_ = prev_y_ = e->getY();
            break;

        case InputEvent::EVM_MOVE:
            result = true;
            if (isPressed()) {
                int move_th = getContext().dp2pxi(4);
                if (std::abs(e->getX() - start_x_) > move_th ||
                    std::abs(e->getY() - start_y_) > move_th)
                {
                    is_moving_ = true;
                }

                if (is_moving_) {
                    translate(e->getX() - prev_x_, e->getY() - prev_y_);
                    requestDraw();
                }
                prev_x_ = e->getX();
                prev_y_ = e->getY();
            }
            break;

        case InputEvent::EVM_UP:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                result = true;
                if (!is_moving_) {
                    makeGridTextLayout(
                        e->getX() - getPadding().start(),
                        e->getY() - getPadding().top());
                }
                is_moving_ = false;
            }
            break;
        }

        case InputEvent::EVM_WHEEL:
            if (e->getWheelValue() > 0) {
                if (scale_level_ < kMaxScaleLevel) {
                    ++scale_level_;
                    scale(scale_level_, e->getX(), e->getY());
                    requestDraw();
                }
            } else if (e->getWheelValue() < 0) {
                if (scale_level_ > 0) {
                    --scale_level_;
                    scale(scale_level_, e->getX(), e->getY());
                    requestDraw();
                }
            }
            break;

        case InputEvent::EV_LEAVE:
            is_moving_ = false;
            break;

        case InputEvent::EVK_DOWN:
        {
            if (!has_text_) {
                break;
            }

            int key = e->getKeyboardKey();
            int col = text_col_;
            int row = text_row_;
            bool need_refresh = false;
            if (key == Keyboard::KEY_LEFT) {
                --col;
                need_refresh = true;
            } else if (key == Keyboard::KEY_RIGHT) {
                ++col;
                need_refresh = true;
            } else if (key == Keyboard::KEY_UP) {
                if (flipped_y_) {
                    ++row;
                } else {
                    --row;
                }
                need_refresh = true;
            } else if (key == Keyboard::KEY_DOWN) {
                if (flipped_y_) {
                    --row;
                } else {
                    ++row;
                }
                need_refresh = true;
            }

            if (need_refresh) {
                makeGridTextLayoutGP(col, row);
                requestDraw();
            }
            break;
        }

        default:
            break;
        }

        return result;
    }

    void GridView::onVisibilityChanged(int visibility) {
        super::onVisibilityChanged(visibility);
        showOverlay(visibility == SHOW);
    }

    void GridView::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        if (!animator_.update(start_time, display_freq)) {
            stopVSync();
        }
        requestDraw();
    }

    void GridView::drawGridLines(Canvas* canvas) {
        int row = -start_row_;
        int cur_x = start_col_offset_;
        int cur_y = start_row_offset_;
        auto bounds = getContentBounds();

        if (flipped_y_) {
            cur_y = bounds.height() - cur_y;
        }

        // 先画横线
        for (;;) {
            if (row != 0) {
                canvas->drawLine(
                    { 0.f, float(cur_y) },
                    PointF(Point{ bounds.width(), cur_y }),
                    line_width_, Color::Grey200);
            }

            if (flipped_y_) {
                cur_y -= length_;
                if (cur_y < 0) {
                    break;
                }
            } else {
                cur_y += length_;
                if (cur_y > bounds.height()) {
                    break;
                }
            }
            ++row;
        }

        int col = -start_col_;
        // 再画竖线
        for (;;) {
            if (col != 0) {
                canvas->drawLine(
                    PointF(Point{ cur_x, 0 }),
                    PointF(Point{ cur_x, bounds.height() }),
                    line_width_, Color::Grey200);
            }
            cur_x += length_;
            if (cur_x > bounds.width()) {
                break;
            }
            ++col;
        }
    }

    void GridView::drawGridPixel(
        Canvas* canvas, int col, int row, const Color& color, bool highlight)
    {
        int inset = int(line_width_ / 2);

        Rect rect;
        getGridRect(col, row, &rect);
        rect.insets(inset, inset, inset, inset);
        canvas->fillRect(RectF(rect), color);

        if (highlight) {
            inset = getContext().dp2pxi(8) + int(getContext().dp2pxi(64) * (1 - animator_.getCurValue()));
            rect.insets(-inset, -inset, -inset, -inset);
            canvas->drawRect(
                RectF(rect), float(getContext().dp2pxi(4)), Color::Blue500);
        }
    }

    void GridView::drawGridPoint(Canvas* canvas, const PointInfo& info) {
        Point p;
        getGridPoint(info.col, info.row, &p);

        canvas->fillCircle(PointF(p), float(info.radius), info.color);

        if (info.tl) {
            canvas->drawTextLayout(
                float(p.x() + info.radius),
                float(p.y() + info.radius), info.tl.get(), info.text_color);
        }
    }

    void GridView::drawGridLine(Canvas* canvas, LineInfo& info) {
        Point p1, p2, p3;
        getGridPoint(info.p1.x(), info.p1.y(), &p1);
        if (info.is_qb) {
            getGridPoint(info.p2.x(), info.p2.y(), &p2);
        }
        getGridPoint(info.p3.x(), info.p3.y(), &p3);

        if (info.is_qb) {
            if (!info.qb) {
                auto path = Path::create();
                if (path->open()) {
                    path->begin(PointF(p1), false);
                    path->addQBezier(PointF(p2), PointF(p3));
                    path->end(false);
                    path->close();
                }
                info.qb.reset(path);
            }
            canvas->drawPath(info.qb.get(), getAdjustedLineWidth(1), info.color);
        } else {
            canvas->drawLine(PointF(p1), PointF(p3), getAdjustedLineWidth(1), info.color);
        }
    }

    void GridView::drawGridText(Canvas* canvas, int col, int row) {
        int inset = int(line_width_ / 2);

        Rect rect;
        getGridRect(col, row, &rect);
        rect.insets(inset, inset, inset, inset);

        canvas->drawTextLayout(
            float(rect.x()), float(rect.y()),
            coord_tl_.get(), Color::Yellow700);

        canvas->drawTextLayout(
            float(rect.x()), rect.y() + coord_tl_->getMaxHeight(),
            color_tl_.get(), Color::Yellow700);
    }

    void GridView::makeGridTextLayout(int x, int y) {
        Size off;
        Point grid_pos;
        getGridPosition(x, y, &grid_pos, &off);
        makeGridTextLayoutGP(grid_pos.x(), grid_pos.y());
    }

    void GridView::makeGridTextLayoutGP(int col, int row) {
        if (has_text_ && text_col_ == col && text_row_ == row) {
            has_text_ = false;
            return;
        }

        {
            std::u16string coord;
            coord.append(u"(")
                .append(utl::itos16(col))
                .append(u", ")
                .append(utl::itos16(row))
                .append(u")");
            if (!coord_tl_->make(
                coord, u"Consolas", getContext().dp2px(12),
                TextLayout::FontStyle::NORMAL, TextLayout::FontWeight::NORMAL, u""))
            {
                return;
            }

            coord_tl_->setTextWrapping(TextLayout::TextWrapping::NONE);

            TextLayout::TextMetrics m;
            if (!coord_tl_->getTextMetrics(&m)) {
                return;
            }

            coord_tl_->setMaxWidth(m.rect.width());
            coord_tl_->setMaxHeight(m.rect.height());
        }
        {
            Color color;
            std::u16string color_text;
            color_text.append(u"ARGB: ");
            if (getGridColor(col, row, &color)) {
                color_text.append(utl::itos16(int(color.a * 255)))
                    .append(u", ")
                    .append(utl::itos16(int(color.r * 255)))
                    .append(u", ")
                    .append(utl::itos16(int(color.g * 255)))
                    .append(u", ")
                    .append(utl::itos16(int(color.b * 255)));
            } else {
                color_text.append(u"N/A");
            }

            if (!color_tl_->make(
                color_text, u"Consolas", getContext().dp2px(12),
                TextLayout::FontStyle::NORMAL, TextLayout::FontWeight::NORMAL, u""))
            {
                return;
            }

            color_tl_->setTextWrapping(TextLayout::TextWrapping::NONE);

            TextLayout::TextMetrics m;
            if (!color_tl_->getTextMetrics(&m)) {
                return;
            }

            color_tl_->setMaxWidth(m.rect.width());
            color_tl_->setMaxHeight(m.rect.height());
        }

        has_text_ = true;
        text_col_ = col;
        text_row_ = row;
    }

    void GridView::translate(int dx, int dy) {
        if (flipped_y_) {
            dy = -dy;
        }

        start_col_offset_ += dx;
        start_row_offset_ += dy;

        start_col_ += start_col_offset_ / length_;
        start_row_ += start_row_offset_ / length_;
        start_col_offset_ %= length_;
        start_row_offset_ %= length_;

        showNewNav();
        showOrgNav();

        for (auto& info : lines_) {
            info.qb.reset();
        }
    }

    void GridView::scale(int level, int sx, int sy) {
        Size off;
        Point grid_pos;
        getGridPosition(sx, sy, &grid_pos, &off);

        int prev_length = length_;

        float scale = std::pow(1.3f, level);
        length_ = int(init_length_ * scale);

        off.width(int(off.width() * float(length_) / prev_length));
        off.height(int(off.height() * float(length_) / prev_length));

        Point pos;
        getGridPoint(grid_pos.x(), grid_pos.y(), off.width(), off.height(), &pos);

        translate(sx - pos.x(), sy - pos.y());
    }

    void GridView::getGridRect(int col, int row, Rect* rect) {
        Point p;
        getGridPoint(col, row, &p);

        if (flipped_y_) {
            rect->xywh(p.x(), p.y() - length_, length_, length_);
        } else {
            rect->xywh(p.x(), p.y(), length_, length_);
        }
    }

    void GridView::getGridPoint(int col, int row, Point* p) {
        getGridPoint(col, row, 0, 0, p);
    }

    void GridView::getGridPoint(int col, int row, int off_col, int off_row, Point* p) {
        p->x(start_col_offset_ + off_col + (col + start_col_) * length_);
        p->y(start_row_offset_ + off_row + (row + start_row_) * length_);

        if (flipped_y_) {
            p->y(getContentBounds().height() - p->y());
        }
    }

    void GridView::getGridPoint(double col, double row, Point* p) {
        int int_col = int(col);
        int int_row = int(row);

        Rect rect;
        getGridRect(int_col, int_row, &rect);

        p->x(int(rect.x() + (col - double(int_col)) * length_));
        if (flipped_y_) {
            p->y(int(rect.bottom() - (row - double(int_row)) * length_));
        } else {
            p->y(int(rect.y() + (row - double(int_row)) * length_));
        }
    }

    void GridView::getGridPosition(int x, int y, Point* pos, Size* off) {
        if (flipped_y_) {
            y = getContentBounds().height() - y;
        }

        off->width((x - start_col_offset_) % length_);
        off->height((y - start_row_offset_) % length_);
        pos->x((x - start_col_offset_) / length_ - start_col_);
        pos->y((y - start_row_offset_) / length_ - start_row_);
    }

    bool GridView::getGridColor(int col, int row, Color* color) {
        for (const auto& info : pixels_) {
            if (info.col == col && info.row == row) {
                *color = info.color;
                return true;
            }
        }
        return false;
    }

    float GridView::getAdjustedLineWidth(int dip) const {
        int result = getContext().dp2pxi(dip);
        if (result % 2) {
            ++result;
        }
        return float(result);
    }

    void GridView::showNewNav() {
        if (getVisibility() != SHOW) {
            return;
        }
        if (pixels_.empty()) {
            new_nav_.closeNav();
            return;
        }

        int col = pixels_.back().col;
        int row = pixels_.back().row;
        int inset = int(line_width_ / 2);

        Rect rect;
        getGridRect(col, row, &rect);
        rect.insets(inset, inset, inset, inset);
        new_nav_.showNav(rect.pos_center());
    }

    void GridView::showOrgNav() {
        if (getVisibility() != SHOW) {
            return;
        }

        Point p;
        getGridPoint(0, 0, &p);

        jour_di("[Grid] Point: [%d, %d]", p.x(), p.y());

        org_nav_.showNav(p);
    }

}
