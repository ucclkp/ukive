// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_RESOURCES_DIM_H_
#define UKIVE_RESOURCES_DIM_H_

#include "ukive/window/context.h"


namespace ukive {

    class dim {
    public:
        enum Type {
            PIXEL,
            DIP,
        };

        static dim of_px(int px) {
            dim d;
            d.set_px(px);
            return d;
        }

        static dim of_dp(float dp) {
            dim d;
            d.set_dp(dp);
            return d;
        }

        dim()
            : px(0), type(PIXEL) {}

        void set_px(int px) {
            this->px = px;
            type = PIXEL;
        }
        void set_dp(float dp) {
            this->dp = dp;
            type = DIP;
        }

        int to_px(const Context& c) const {
            if (type == DIP) {
                return int(c.dp2px(dp));
            }
            return px;
        }

        union {
            int px;
            float dp;
        };
        Type type;
    };

    using dimcref = const dim&;

    namespace dim_literal {

        inline dim operator""_px(unsigned long long px) {
            return dim::of_px(int(px));
        }
        inline dim operator""_dp(unsigned long long dp) {
            return dim::of_dp(float(dp));
        }
        inline dim operator""_dp(long double dp) {
            return dim::of_dp(float(dp));
        }

    }

}

#endif  // UKIVE_RESOURCES_DIM_H_