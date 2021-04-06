// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/tree/tree_node_button.h"

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/path.h"
#include "ukive/window/window.h"


namespace ukive {

    TreeNodeButton::TreeNodeButton(Context c)
        : TreeNodeButton(c, {}) {}

    TreeNodeButton::TreeNodeButton(Context c, AttrsRef attrs)
        : View(c, attrs)
    {
        setClickable(true);

        size_ = c.dp2pxi(9);
        if (size_ & 1) { ++size_; }
        tri_length_ = size_;
        tri_height_ = size_ / 2;

        tri_path_.reset(Path::create());
        if (tri_path_->open()) {
            tri_path_->begin(PointF(0, 0), true);
            tri_path_->addLine(PointF(Point(tri_height_, tri_height_)));
            tri_path_->addLine(PointF(0, float(tri_length_)));
            tri_path_->addLine(PointF(0, 0));
            tri_path_->end(true);
            tri_path_->close();
        }
    }

    void TreeNodeButton::setStatus(Status s) {
        if (status_ != s) {
            status_ = s;
            requestDraw();
        }
    }

    Size TreeNodeButton::onDetermineSize(const SizeInfo& info) {
        return Size(size_ + getPadding().hori(), size_ + getPadding().vert());
    }

    void TreeNodeButton::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        if (status_ == NONE) {
            return;
        }

        auto bounds = getContentBounds();

        int x = bounds.width() / 2;
        int y = (bounds.height() - tri_length_) / 2;

        canvas->save();
        canvas->translate(float(x), float(y));
        if (status_ == EXPANDED) {
            canvas->rotate(90.f, 0, float(tri_height_));
            canvas->fillPath(tri_path_.get(), Color::Grey600);
        } else {
            canvas->fillPath(tri_path_.get(), Color::Grey400);
        }

        canvas->restore();
    }

}
