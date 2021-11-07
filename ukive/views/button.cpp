// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "button.h"

#include "ukive/window/window.h"
#include "ukive/elements/ripple_element.h"
#include "ukive/graphics/canvas.h"
#include "ukive/event/input_event.h"
#include "ukive/graphics/path.h"

#include "necro/layout_constants.h"


namespace ukive {

    // Button
    Button::Button(Context c)
        : Button(c, {})
    {}

    Button::Button(Context c, AttrsRef attrs)
        : TextView(c, attrs)
    {
        if (attrs.find(necro::kAttrTextViewText) == attrs.end()) {
            setText(u"Button");
        }
        setTextSize(c.dp2pxi(13));
        setIsEditable(false);
        setIsSelectable(false);
        setClickable(true);
        setFocusable(true);

        initButton();
    }

    void Button::initButton() {
        shape_element_ = new ShapeElement(ShapeElement::ROUND_RECT);
        shape_element_->setRadius(getContext().dp2px(2.f));
        shape_element_->setSolidEnable(true);
        shape_element_->setSolidColor(Color::White);

        ripple_background_ = new RippleElement();
        ripple_background_->add(shape_element_);

        setPadding(
            getContext().dp2pxi(24),
            getContext().dp2pxi(6),
            getContext().dp2pxi(24),
            getContext().dp2pxi(6));
        setBackground(ripple_background_);
        setShadowRadius(getContext().dp2pxi(2.0f));
    }

    void Button::setButtonColor(Color color) {
        shape_element_->setSolidColor(color);
        requestDraw();
    }

    void Button::setButtonShape(ShapeElement::Shape shape) {
        switch (shape) {
        case ShapeElement::OVAL:
            setOutline(OUTLINE_OVAL);
            break;

        case ShapeElement::RECT:
        case ShapeElement::ROUND_RECT:
        default:
            setOutline(OUTLINE_RECT);
            break;
        }
        shape_element_->setShape(shape);
        requestDraw();
    }


    // DropdownButton
    DropdownButton::DropdownButton(Context c)
        : View(c)
    {
        setClickable(true);

        initButton();
    }

    void DropdownButton::initButton() {
        auto shape_element = new ShapeElement(ShapeElement::OVAL);
        shape_element->setSolidEnable(true);
        shape_element->setSolidColor(Color::White);

        auto ripple_background = new RippleElement();
        ripple_background->setDrawMaskEnabled(false);
        ripple_background->add(shape_element);
        setBackground(ripple_background);

        tri_length_ = getContext().dp2pxi(10);
        tri_height_ = getContext().dp2pxi(6);
        button_size_ = getContext().dp2pxi(28);

        // 设为偶数
        if (tri_length_ & 1) { ++tri_length_; }
        if (tri_height_ & 1) { ++tri_height_; }
        if (button_size_ & 1) { ++button_size_; }

        tri_path_.reset(Path::create());
        if (tri_path_->open()) {
            tri_path_->begin(PointF(0, 0), true);
            tri_path_->addLine(PointF(float(tri_length_), 0));
            tri_path_->addLine(PointF(Point(tri_length_ / 2, tri_height_)));
            tri_path_->addLine(PointF(0, 0));
            tri_path_->end(true);
            tri_path_->close();
        }
    }

    Size DropdownButton::onDetermineSize(const SizeInfo& info) {
        return getPreferredSize(info, button_size_, button_size_);
    }

    void DropdownButton::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        int x = (getWidth() - tri_length_) / 2;
        int y = (getHeight() - tri_height_) / 2;

        canvas->save();
        canvas->translate(float(x), float(y + 1));

        canvas->fillPath(tri_path_.get(), Color::Grey500);

        canvas->restore();
    }

}
