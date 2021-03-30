// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_PAINT_H_
#define UKIVE_GRAPHICS_PAINT_H_

#include "ukive/graphics/color.h"


namespace ukive {

    class ImageFrame;

    class Paint {
    public:
        enum class Style {
            STROKE,
            FILL,
            IMAGE,
        };

        enum class CapStyle {
            FLAT,
            SQUARE,
            ROUND,
            TRIANGLE,
        };

        enum class LineJoinStyle {
            MITER,
            BEVEL,
            ROUND,
            MITER_OR_BEVEL
        };

        enum class DashStyle {
            SOLID,
            DASH,
            DOT,
            DASH_DOT,
            DASH_DOT_DOT,
            CUSTOM
        };

        enum class ImageExtendMode {
            CLAMP,
            WRAP,
            MIRROR,
        };

        struct StrokeStyle {
            CapStyle start_cap;
            CapStyle end_cap;
            CapStyle dash_cap;
            LineJoinStyle line_join;
            float miter_limit;
            DashStyle dash_style;
            float dash_offset;

            float* dashes;
            int dashes_count;

            StrokeStyle():
                start_cap(CapStyle::FLAT),
                end_cap(CapStyle::FLAT),
                dash_cap(CapStyle::FLAT),
                line_join(LineJoinStyle::MITER),
                miter_limit(0),
                dash_style(DashStyle::SOLID),
                dash_offset(0),
                dashes(nullptr),
                dashes_count(0) {}
        };

        Paint();
        ~Paint() = default;

        void setStyle(Style s);
        void setStrokeStyle(const StrokeStyle& s);
        void setImage(
            const ImageFrame* b,
            ImageExtendMode x_em = ImageExtendMode::CLAMP,
            ImageExtendMode y_em = ImageExtendMode::CLAMP);
        void setColor(const Color& color);
        void setAntialias(bool enabled);
        void setTextAntialias(bool enabled);
        void setStrokeWidth(float width);

        Style getStyle() const;
        const StrokeStyle& getStrokeStyle() const;
        const ImageFrame* getImage() const;
        ImageExtendMode getImageExtendModeX() const;
        ImageExtendMode getImageExtendModeY() const;
        Color getColor() const;
        bool hasStrokeStyle() const;
        bool isAntialiased() const;
        bool isTextAntialiased() const;
        float getStrokeWidth() const;

    private:
        Color color_;
        Style style_;
        StrokeStyle stroke_style_;
        float stroke_width_;
        bool has_stroke_style_;
        bool is_antialiased_;
        bool is_text_antialiased_;

        const ImageFrame* frame_ = nullptr;
        ImageExtendMode x_em_;
        ImageExtendMode y_em_;
    };

}

#endif  // UKIVE_GRAPHICS_PAINT_H_