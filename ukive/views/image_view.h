// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_IMAGE_VIEW_H_
#define UKIVE_VIEWS_IMAGE_VIEW_H_

#include <memory>

#include "ukive/views/view.h"
#include "ukive/graphics/matrix_2x3_f.h"
#include "ukive/graphics/gptr.hpp"


namespace ukive {

    class ImageFrame;
    class ImageElement;

    class ImageView : public View {
    public:
        enum ScaleType {
            FULL,
            FIT_ALWAYS,
            FIT_WHEN_LARGE,
            MATRIX,
        };

        explicit ImageView(Context c);
        ImageView(Context c, AttrsRef attrs);
        ~ImageView();

        Size onDetermineSize(const SizeInfo& info) override;
        void onBoundsChanged(const Rect& new_bounds, const Rect& old_bounds) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

        void setMatrix(const Matrix2x3F& m);
        void setScaleType(ScaleType type);
        void setImage(const GPtr<ImageFrame>& img);

        Matrix2x3F getMatrix() const;
        ScaleType getScaleType() const;
        GPtr<ImageFrame> getImage() const;

    protected:
        void onContextChanged(const Context& context) override;

    private:
        void setImageBounds(int width, int height);
        void fitImageBounds(int width, int height, bool always);

        Matrix2x3F matrix_;
        ScaleType scale_type_;
        std::unique_ptr<ImageElement> img_element_;
    };

}

#endif  // UKIVE_VIEWS_IMAGE_VIEW_H_