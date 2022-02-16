// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_DIAGNOSTIC_GRID_VIEW_H_
#define UKIVE_DIAGNOSTIC_GRID_VIEW_H_

#include "utils/weak_ref_nest.hpp"

#include "ukive/animation/animator.h"
#include "ukive/diagnostic/grid_navigator.h"
#include "ukive/graphics/size.hpp"
#include "ukive/graphics/vsyncable.h"
#include "ukive/views/view.h"


namespace ukive {

    class Path;
    class TextLayout;

    class GridView : public View, public VSyncable {
    public:
        explicit GridView(Context c);
        GridView(Context c, AttrsRef attrs);

        void setFlippedY(bool flipped_y);
        void addPixel(int col, int row, const Color& color);
        bool getPixel(int col, int row, Color* color) const;
        void clearPixels();

        void addPoint(
            double col, double row, int radius, const Color& color);
        void addPoint(
            double col, double row, int radius, const Color& color,
            const std::u16string_view& text, const Color& text_color);
        void clearPoints();

        void addLine(
            const PointD& p1, const PointD& p2, const Color& color);
        void addQBezierLine(
            const PointD& p1, const PointD& p2, const PointD& p3, const Color& color);
        void clearLines();

        // View
        Size onDetermineSize(const SizeInfo& info) override;
        void onAfterLayout(const Rect& new_bounds, const Rect& old_bounds) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;
        void onVisibilityChanged(int visibility) override;

        // VSyncable
        void onVSync(
            uint64_t start_time, uint32_t display_freq, uint32_t real_interval) override;

    private:
        using super = View;

        struct PixelInfo {
            int col;
            int row;
            Color color;
        };

        struct PointInfo {
            double col;
            double row;
            int radius;
            Color color;

            std::shared_ptr<TextLayout> tl;
            Color text_color;
        };

        struct LineInfo {
            bool is_qb;
            PointD p1;
            PointD p2;
            PointD p3;
            std::shared_ptr<Path> qb;
            Color color;
        };

        void drawGridLines(Canvas* canvas);
        void drawGridPixel(
            Canvas* canvas, int col, int row, const Color& color, bool highlight);
        void drawGridPoint(Canvas* canvas, const PointInfo& info);
        void drawGridLine(Canvas* canvas, LineInfo& info);
        void drawGridText(Canvas* canvas, int col, int row);
        void makeGridTextLayout(int x, int y);
        void makeGridTextLayoutGP(int col, int row);

        void translate(int dx, int dy);
        void scale(int level, int sx, int sy);

        void getGridRect(int col, int row, Rect* rect);
        void getGridPoint(int col, int row, Point* p);
        void getGridPoint(int col, int row, int off_col, int off_row, Point* p);
        void getGridPoint(double col, double row, Point* p);
        void getGridPosition(int x, int y, Point* pos, Size* off);
        bool getGridColor(int col, int row, Color* color);
        float getAdjustedLineWidth(int dip) const;

        void showNewNav();
        void showOrgNav();

        int start_col_ = 0;
        int start_row_ = 0;
        int start_col_offset_ = 0;
        int start_row_offset_ = 0;
        int scale_level_ = 0;
        bool flipped_y_ = false;

        int length_ = 0;
        int init_length_ = 0;
        float line_width_ = 0;

        int start_x_ = 0;
        int start_y_ = 0;
        int prev_x_ = 0;
        int prev_y_ = 0;
        bool is_moving_ = false;

        bool has_text_ = false;
        int text_col_ = 0;
        int text_row_ = 0;

        Animator animator_;
        std::vector<PixelInfo> pixels_;
        std::vector<PointInfo> points_;
        std::vector<LineInfo> lines_;
        GridNavigator new_nav_;
        GridNavigator org_nav_;
        std::unique_ptr<TextLayout> coord_tl_;
        std::unique_ptr<TextLayout> color_tl_;
        utl::WeakRefNest<GridView> weak_ref_nest_;
    };

}

#endif  // UKIVE_DIAGNOSTIC_GRID_VIEW_H_