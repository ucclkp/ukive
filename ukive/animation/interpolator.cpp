// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/interpolator.h"


namespace ukive {

    // LinearInterpolator
    LinearInterpolator::LinearInterpolator()
    {}

    double LinearInterpolator::interpolate(
        double init_val, double final_val, double progress)
    {
        if (progress == 1) {
            return final_val;
        }

        auto distance = final_val - init_val;
        return init_val + distance * progress;
    }

}