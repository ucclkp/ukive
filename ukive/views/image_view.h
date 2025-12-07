// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_IMAGE_VIEW_H_
#define UKIVE_VIEWS_IMAGE_VIEW_H_

#include <memory>

#include "ukive/views/view.h"
#include "ukive/graphics/gptr.hpp"


namespace ukive {

    class ImageFrame;
    class ImageElement;

    class ImageView : public View {
    public:
        enum ScaleType {
            ST_NONE,
            ST_FULL,
            ST_FIT_ALWAYS,
            ST_FIT_WHEN_LARGE,
            ST_MATRIX,
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
        void setImageName(const std::u16string_view& name);
        void setImageOpacity(float opacity);
        void setImageFilter(bool filter);

        Matrix2x3F getMatrix() const;
        ScaleType getScaleType() const;
        GPtr<ImageFrame> getImage() const;
        Rect getImageBounds() const;
        float GetImageOpacity() const;

        RectF calculateImageBounds() const;

    protected:
        void onContextChanged(Context::Type type, const Context& context) override;

    private:
        void setImageBounds(int width, int height);
        RectF calImageBoundsByScaleType(int width, int height);
        RectF fitImageBounds(int width, int height, bool always) const;

        float opacity_ = 1.f;
        bool need_filter_ = true;
        Matrix2x3F matrix_;
        ScaleType scale_type_;
        std::unique_ptr<ImageElement> img_element_;
    };

}

#endif  // UKIVE_VIEWS_IMAGE_VIEW_H_