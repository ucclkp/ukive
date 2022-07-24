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
        setEditable(false);
        setSelectable(false);
        setClickable(true);
        setFocusable(true);

        initButton();
    }

    void Button::initButton() {
        bg_ = new RippleElement(Element::SHAPE_RRECT, Color::White);
        bg_->setRadius(getContext().dp2px(2.f));

        setPadding(
            getContext().dp2pxi(24),
            getContext().dp2pxi(6),
            getContext().dp2pxi(24),
            getContext().dp2pxi(6));
        setBackground(bg_);
        setShadowRadius(getContext().dp2pxi(2.0f));
    }

    void Button::setButtonColor(Color color) {
        bg_->setSolidColor(color);
        requestDraw();
    }

    void Button::setButtonShape(Element::Shape shape) {
        switch (shape) {
        case Element::SHAPE_OVAL:
            setOutline(OUTLINE_OVAL);
            break;

        case Element::SHAPE_RECT:
        case Element::SHAPE_RRECT:
        default:
            setOutline(OUTLINE_RECT);
            break;
        }
        bg_->setShape(shape);
        requestDraw();
    }


    // DropdownButton
    DropdownButton::DropdownButton(Context c)
        : View(c)
    {
        initButton();
    }

    void DropdownButton::initButton() {
        setClickable(true);

        auto bg = new RippleElement(Element::SHAPE_OVAL, Color::White);
        bg->setDrawMaskEnabled(false);
        setBackground(bg);

        tri_length_ = getContext().dp2pxi(10);
        tri_height_ = getContext().dp2pxi(6);
        button_size_ = getContext().dp2pxi(28);

        // 设为偶数
        if (tri_length_ & 1) { ++tri_length_; }
        if (tri_height_ & 1) { ++tri_height_; }
        if (button_size_ & 1) { ++button_size_; }

        tri_path_.reset(Path::create());
        if (tri_path_->open()) {
            tri_path_->begin({ 0, 0 }, true);
            tri_path_->addLine({ float(tri_length_), 0.f });
            tri_path_->addLine(PointF(Point{ tri_length_ / 2, tri_height_ }));
            tri_path_->addLine({ 0, 0 });
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
