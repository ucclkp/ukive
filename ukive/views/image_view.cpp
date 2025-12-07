// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_view.h"

#include <algorithm>

#include "ukive/app/application.h"
#include "ukive/elements/image_element.h"
#include "ukive/graphics/canvas.h"
#include "ukive/resources/resource_manager.h"


namespace ukive {

    ImageView::ImageView(Context c)
        : ImageView(c, {}) {}

    ImageView::ImageView(Context c, AttrsRef attrs)
        : View(c, attrs),
          scale_type_(ST_FIT_ALWAYS) {}

    ImageView::~ImageView() {}

    Size ImageView::onDetermineSize(const SizeInfo& info) {
        int bound_width = 0;
        int bound_height = 0;

        switch (info.width().mode) {
        case SizeInfo::CONTENT:
            if (img_element_) {
                bound_width = img_element_->getContentWidth();
            }
            break;

        case SizeInfo::FREEDOM:
            if (img_element_) {
                bound_width = img_element_->getContentWidth();
            }
            break;

        case SizeInfo::DEFINED:
            bound_width = info.width().val;
            break;

        default:
            break;
        }

        switch (info.height().mode) {
        case SizeInfo::CONTENT:
            if (img_element_) {
                bound_height = img_element_->getContentHeight();
            }
            break;

        case SizeInfo::FREEDOM:
            if (img_element_) {
                bound_height = img_element_->getContentHeight();
            }
            break;

        case SizeInfo::DEFINED:
            bound_height = info.height().val;
            break;

        default:
            break;
        }

        auto bounds = calImageBoundsByScaleType(bound_width, bound_height);

        int final_width_fit_img  = (int)bounds.width()  + getPadding().hori();
        int final_height_fit_img = (int)bounds.height() + getPadding().vert();

        if (info.width().mode == SizeInfo::DEFINED) {
            final_width_fit_img = bound_width;
        } else if (info.width().mode == SizeInfo::CONTENT) {
            final_width_fit_img = (std::min)(info.width().val, final_width_fit_img);
        }

        if (info.height().mode == SizeInfo::DEFINED) {
            final_height_fit_img = bound_height;
        } else if (info.height().mode == SizeInfo::CONTENT) {
            final_height_fit_img = (std::min)(info.height().val, final_height_fit_img);
        }

        return Size(final_width_fit_img, final_height_fit_img);
    }

    void ImageView::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        if (scale_type_ == ST_MATRIX) {
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
            img_element_->setOpacity(opacity_);
            img_element_->setFilter(need_filter_);
            auto b = getContentBounds();
            setImageBounds(b.width(), b.height());
        } else {
            img_element_.reset();
        }

        requestLayout();
        requestDraw();
    }

    void ImageView::setImageName(const std::u16string_view& name) {
        auto rm = Application::getResourceManager();
        rm->getImagePath(name);
        // TODO
    }

    void ImageView::setImageOpacity(float opacity) {
        opacity_ = opacity;
        if (img_element_) {
            img_element_->setOpacity(opacity_);
        }
    }

    void ImageView::setImageFilter(bool filter) {
        need_filter_ = filter;
        if (img_element_) {
            img_element_->setFilter(need_filter_);
        }
    }

    void ImageView::setImageBounds(int width, int height) {
        if (!img_element_ || width <= 0 || height <= 0) {
            return;
        }

        img_element_->setBounds(
            (Rect)calImageBoundsByScaleType(width, height));
    }

    RectF ImageView::calImageBoundsByScaleType(int width, int height) {
        if (!img_element_) {
            return {};
        }

        switch (scale_type_) {
        case ST_FULL:
            return RectF(0, 0, width, height);

        case ST_FIT_ALWAYS:
            return fitImageBounds(width, height, true);

        case ST_FIT_WHEN_LARGE:
            return fitImageBounds(width, height, false);

        case ST_NONE:
        case ST_MATRIX:
        default:
            return RectF(
                0, 0,
                img_element_->getContentWidth(),
                img_element_->getContentHeight());
        }
    }

    RectF ImageView::fitImageBounds(int width, int height, bool always) const {
        if (!img_element_) {
            return {};
        }

        float img_width = img_element_->getContentWidth();
        float img_height = img_element_->getContentHeight();
        if (img_width <= 0 || img_height <= 0) {
            return {};
        }

        if (always || (img_width > width || img_height > height)) {
            float scale = (std::min)(
                float(width) / img_width,
                float(height) / img_height);

            img_width = img_width * scale;
            img_height = img_height * scale;
        }

        auto img_x = (width - img_width) / 2.f;
        auto img_y = (height - img_height) / 2.f;

        return RectF(
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

    Rect ImageView::getImageBounds() const {
        if (!img_element_) {
            return {};
        }
        return img_element_->getBounds();
    }

    float ImageView::GetImageOpacity() const {
        return opacity_;
    }

    RectF ImageView::calculateImageBounds() const {
        RectF r;
        int width = getContentBounds().width();
        int height = getContentBounds().height();

        switch (scale_type_) {
        case ST_FULL:
            r.xywh(0, 0, width, height);
            break;

        case ST_FIT_ALWAYS:
            r = fitImageBounds(width, height, true);
            break;

        case ST_FIT_WHEN_LARGE:
            r = fitImageBounds(width, height, false);
            break;

        case ST_NONE:
        case ST_MATRIX:
        default:
            if (img_element_) {
                r.xywh(
                    0, 0,
                    img_element_->getContentWidth(),
                    img_element_->getContentHeight());
            } else {
                r.xywh(0, 0, 0, 0);
            }
            break;
        }

        return r;
    }

    void ImageView::onContextChanged(Context::Type type, const Context& context) {
        View::onContextChanged(type, context);
        if (img_element_) {
            img_element_->notifyContextChanged(type, context);
        }
    }

}