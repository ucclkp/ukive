// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_view.h"

#include <algorithm>

#include "ukive/elements/image_element.h"
#include "ukive/graphics/canvas.h"


namespace ukive {

    ImageView::ImageView(Context c)
        : ImageView(c, {}) {}

    ImageView::ImageView(Context c, AttrsRef attrs)
        : View(c, attrs),
          scale_type_(FIT_WHEN_LARGE) {
    }

    ImageView::~ImageView() {}

    Size ImageView::onDetermineSize(const SizeInfo& info) {
        int final_width = 0;
        int final_height = 0;

        switch (info.width().mode) {
        case SizeInfo::CONTENT:
            if (img_element_) {
                final_width = img_element_->getContentWidth();
            }

            final_width = final_width + getPadding().hori();
            final_width = (std::min)(info.width().val, final_width);
            break;

        case SizeInfo::FREEDOM:
            if (img_element_) {
                final_width = img_element_->getContentWidth();
            }

            final_width = final_width + getPadding().hori();
            break;

        case SizeInfo::DEFINED:
            final_width = info.width().val;
            break;

        default:
            break;
        }

        switch (info.height().mode) {
        case SizeInfo::CONTENT:
            if (img_element_) {
                final_height = img_element_->getContentHeight();
            }

            final_height = final_height + getPadding().vert();
            final_height = (std::min)(info.height().val, final_height);
            break;

        case SizeInfo::FREEDOM:
            if (img_element_) {
                final_height = img_element_->getContentHeight();
            }

            final_height = final_height + getPadding().vert();
            break;

        case SizeInfo::DEFINED:
            final_height = info.height().val;
            break;

        default:
            break;
        }

        return Size(final_width, final_height);
    }

    void ImageView::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        if (scale_type_ == MATRIX) {
            canvas->save();
            canvas->concat(matrix_);

            if (img_element_) {
                img_element_->draw(canvas);
            }

            canvas->restore();
        } else {
            if (img_element_) {
                img_element_->draw(canvas);
            }
        }
    }

    bool ImageView::onInputEvent(InputEvent* e) {
        return View::onInputEvent(e);
    }

    void ImageView::onBoundsChanged(const Rect& new_bounds, const Rect& old_bounds) {
        View::onBoundsChanged(new_bounds, old_bounds);

        auto b = getContentBounds();
        setImageBounds(b.width(), b.height());
    }

    void ImageView::setMatrix(const Matrix2x3F& m) {
        matrix_ = m;
        requestDraw();
    }

    void ImageView::setScaleType(ScaleType type) {
        if (scale_type_ == type) {
            return;
        }

        scale_type_ = type;
        auto b = getContentBounds();
        setImageBounds(b.width(), b.height());

        requestDraw();
    }

    void ImageView::setImage(const GPtr<ImageFrame>& img) {
        if (img) {
            img_element_.reset(new ImageElement(img));
            auto b = getContentBounds();
            setImageBounds(b.width(), b.height());
        } else {
            img_element_.reset();
        }

        requestLayout();
        requestDraw();
    }

    void ImageView::setImageBounds(int width, int height) {
        if (!img_element_ || width <= 0 || height <= 0) {
            return;
        }

        switch (scale_type_) {
        case FULL:
            img_element_->setBounds(0, 0, width, height);
            break;

        case FIT_ALWAYS:
            fitImageBounds(width, height, true);
            break;

        case FIT_WHEN_LARGE:
            fitImageBounds(width, height, false);
            break;

        case MATRIX:
        default:
            img_element_->setBounds(
                0, 0,
                img_element_->getContentWidth(),
                img_element_->getContentHeight());
            break;
        }
    }

    void ImageView::fitImageBounds(int width, int height, bool always) {
        int img_width = img_element_->getContentWidth();
        int img_height = img_element_->getContentHeight();
        if (img_width <= 0 || img_height <= 0) {
            return;
        }

        if (always || (img_width > width || img_height > height)) {
            float scale = (std::min)(
                float(width) / img_width,
                float(height) / img_height);

            img_width = int(img_width * scale);
            img_height = int(img_height * scale);
        }

        int img_x = (width - img_width) / 2;
        int img_y = (height - img_height) / 2;

        img_element_->setBounds(
            img_x, img_y,
            img_width, img_height);
    }

    Matrix2x3F ImageView::getMatrix() const {
        return matrix_;
    }

    ImageView::ScaleType ImageView::getScaleType() const {
        return scale_type_;
    }

    GPtr<ImageFrame> ImageView::getImage() const {
        if (!img_element_) {
            return {};
        }
        return img_element_->getImage();
    }

    void ImageView::onContextChanged(Context::Type type, const Context& context) {
        View::onContextChanged(type, context);
        if (img_element_) {
            img_element_->notifyContextChanged(type, context);
        }
    }

}