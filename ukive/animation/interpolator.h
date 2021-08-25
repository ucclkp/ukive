// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_INTERPOLATOR_H_
#define UKIVE_ANIMATION_INTERPOLATOR_H_


namespace ukive {

    class Interpolator {
    public:
        virtual ~Interpolator() = default;

        virtual double interpolate(
            double init_val, double final_val, double progress) = 0;
    };


    class LinearInterpolator : public Interpolator {
    public:
        LinearInterpolator();

        double interpolate(
            double init_val, double final_val, double progress) override;
    };

}

#endif  // UKIVE_ANIMATION_INTERPOLATOR_H_