// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "progress_bar.h"

#include "ukive/graphics/canvas.h"


namespace ukive {

    ProgressBar::ProgressBar(Context c)
        : ProgressBar(c, {}) {}

    ProgressBar::ProgressBar(Context c, AttrsRef attrs)
        : super(c, attrs)
    {
        def_width_ = c.dp2pxi(200);
        def_height_ = c.dp2pxi(6);
    }

    Size ProgressBar::onDetermineSize(const SizeInfo& info) {
        return getPreferredSize(info, def_width_, def_height_);
    }

    void ProgressBar::onDraw(Canvas* canvas) {
        super::onDraw(canvas);

        auto bounds = getContentBounds();
        canvas->fillRect(RectF(bounds), Color::Grey200);

        if (max_progress_ > 0) {
            float percent = (std::min)(1.f, float(cur_progress_) / max_progress_);
            bounds.right(bounds.x() + int(bounds.width() * percent));
            canvas->fillRect(RectF(bounds), Color::Blue400);
        }
    }

    void ProgressBar::setMax(int max_progress) {
        max_progress_ = max_progress;
        requestDraw();
    }

    void ProgressBar::setProgress(int cur_progress) {
        cur_progress_ = cur_progress;
        requestDraw();
    }

    int ProgressBar::getMax() const {
        return max_progress_;
    }

    int ProgressBar::getProgress() const {
        return cur_progress_;
    }

}
