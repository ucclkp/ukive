// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_RGB_SYSTEM_H_
#define UKIVE_GRAPHICS_COLORS_RGB_SYSTEM_H_


namespace ukive {

    struct RGBSystem {

        struct Pos {
            float x, y;
        };

        Pos red;
        Pos green;
        Pos blue;
        Pos ref_white;
        float red_gamma;
        float green_gamma;
        float blue_gamma;

        static RGBSystem bt601_pal() {
            RGBSystem bt601;
            bt601.red   = { 0.640f, 0.330f };
            bt601.green = { 0.290f, 0.600f };
            bt601.blue  = { 0.150f, 0.060f };
            bt601.ref_white = { 0.3127f, 0.3290f };
            bt601.red_gamma   = 2.22f;
            bt601.green_gamma = 2.22f;
            bt601.blue_gamma  = 2.22f;
            return bt601;
        }

        static RGBSystem bt601_ntsc() {
            RGBSystem bt601;
            bt601.red   = { 0.630f, 0.340f };
            bt601.green = { 0.310f, 0.595f };
            bt601.blue  = { 0.155f, 0.070f };
            bt601.ref_white = { 0.3127f, 0.3290f };
            bt601.red_gamma   = 2.22f;
            bt601.green_gamma = 2.22f;
            bt601.blue_gamma  = 2.22f;
            return bt601;
        }

        static RGBSystem bt709() {
            RGBSystem bt709;
            bt709.red   = { 0.640f, 0.330f };
            bt709.green = { 0.300f, 0.600f };
            bt709.blue  = { 0.150f, 0.060f };
            bt709.ref_white = { 0.3127f, 0.3290f };
            bt709.red_gamma   = 2.22f;
            bt709.green_gamma = 2.22f;
            bt709.blue_gamma  = 2.22f;
            return bt709;
        }

        static RGBSystem bt2020() {
            RGBSystem bt2020;
            bt2020.red   = { 0.708f, 0.292f };
            bt2020.green = { 0.170f, 0.797f };
            bt2020.blue  = { 0.131f, 0.046f };
            bt2020.ref_white = { 0.3127f, 0.3290f };
            bt2020.red_gamma   = 2.22f;
            bt2020.green_gamma = 2.22f;
            bt2020.blue_gamma  = 2.22f;
            return bt2020;
        }
    };

}

#endif  // UKIVE_GRAPHICS_COLORS_RGB_SYSTEM_H_