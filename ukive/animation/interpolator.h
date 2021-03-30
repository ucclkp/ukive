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

        virtual void setInitVal(double init_val) = 0;
        virtual double interpolate(double progress) = 0;
    };

    class LinearInterpolator : public Interpolator {
    public:
        explicit LinearInterpolator(double final_val);

        void setInitVal(double init_val) override;
        double interpolate(double progress) override;

    private:
        double init_val_;
        double final_val_;
    };

}

#endif  // UKIVE_ANIMATION_INTERPOLATOR_H_