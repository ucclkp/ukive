// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_PROGRESS_BAR_H_
#define UKIVE_VIEWS_PROGRESS_BAR_H_

#include "ukive/views/view.h"


namespace ukive {

    class ProgressBar : public View {
    public:
        explicit ProgressBar(Context c);
        ProgressBar(Context c, AttrsRef attrs);

        Size onDetermineSize(const SizeInfo& info) override;
        void onDraw(Canvas* canvas) override;

        void setMax(int max_progress);
        void setProgress(int cur_progress);

        int getMax() const;
        int getProgress() const;

    private:
        using super = View;

        int def_width_;
        int def_height_;
        int max_progress_ = 100;
        int cur_progress_ = 0;
    };

}

#endif  // UKIVE_VIEWS_PROGRESS_BAR_H_