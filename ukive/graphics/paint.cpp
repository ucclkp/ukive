// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "paint.h"


namespace ukive {

    Paint::Paint()
        : color_(Color::Black),
          style_(Style::STROKE),
          stroke_width_(1.f),
          has_stroke_style_(false),
          is_antialiased_(true),
          is_text_antialiased_(true),
          x_em_(ImageExtendMode::CLAMP),
          y_em_(ImageExtendMode::CLAMP)
    {}

    void Paint::setStyle(Style s) {
        style_ = s;
    }

    void Paint::setStrokeStyle(const StrokeStyle& s) {
        stroke_style_ = s;
        has_stroke_style_ = true;
    }

    void Paint::setImage(
        const ImageFrame* b, ImageExtendMode x_em, ImageExtendMode y_em)
    {
        frame_ = b;
    }

    void Paint::setColor(const Color& color) {
        color_ = color;
    }

    void Paint::setAntialias(bool enabled) {
        is_antialiased_ = enabled;
    }

    void Paint::setTextAntialias(bool enabled) {
        is_text_antialiased_ = enabled;
    }

    void Paint::setStrokeWidth(float width) {
        stroke_width_ = width;
    }

    Paint::Style Paint::getStyle() const {
        return style_;
    }

    const Paint::StrokeStyle& Paint::getStrokeStyle() const {
        return stroke_style_;
    }

    const ImageFrame* Paint::getImage() const {
        return frame_;
    }

    Paint::ImageExtendMode Paint::getImageExtendModeX() const {
        return x_em_;
    }

    Paint::ImageExtendMode Paint::getImageExtendModeY() const {
        return y_em_;
    }

    Color Paint::getColor() const {
        return color_;
    }

    bool Paint::hasStrokeStyle() const {
        return has_stroke_style_;
    }

    bool Paint::isAntialiased() const {
        return is_antialiased_;
    }

    bool Paint::isTextAntialiased() const {
        return is_text_antialiased_;
    }

    float Paint::getStrokeWidth() const {
        return stroke_width_;
    }

}