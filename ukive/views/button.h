// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_BUTTON_H_
#define UKIVE_VIEWS_BUTTON_H_

#include "ukive/views/text_view.h"
#include "ukive/elements/shape_element.h"


namespace ukive {

    class Path;
    class ShapeElement;
    class RippleElement;

    class Button : public TextView {
    public:
        explicit Button(Context c);
        Button(Context c, AttrsRef attrs);

        void setButtonColor(Color color);
        void setButtonShape(ShapeElement::Shape shape);

    private:
        void initButton();

        ShapeElement* shape_element_;
        RippleElement* ripple_background_;
    };


    class DropdownButton : public View {
    public:
        explicit DropdownButton(Context c);

    protected:
        // View
        Size onDetermineSize(const SizeInfo& info) override;
        void onDraw(Canvas* canvas) override;

    private:
        void initButton();

        int tri_length_ = 0;
        int tri_height_ = 0;
        int button_size_ = 0;
        std::unique_ptr<Path> tri_path_;
    };

}

#endif  // UKIVE_VIEWS_BUTTON_H_