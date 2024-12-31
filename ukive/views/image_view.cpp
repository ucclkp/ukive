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

        switch (scale_type_) {
        case ST_NONE:
            img_element_->setBounds(
                0, 0,
                img_element_->getContentWidth(),
                img_element_->getContentHeight());
            break;

        case ST_FULL:
            img_element_->setBounds(0, 0, width, height);
            break;

        case ST_FIT_ALWAYS:
        {
            auto bounds = fitImageBounds(width, height, true);
            img_element_->setBounds((Rect)bounds);
            break;
        }

        case ST_FIT_WHEN_LARGE:
        {
            auto bounds = fitImageBounds(width, height, false);
            img_element_->setBounds((Rect)bounds);
            break;
        }

        case ST_MATRIX:
        default:
            img_element_->setBounds(
                0, 0,
                img_element_->getContentWidth(),
                img_element_->getContentHeight());
            break;
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