// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_RGB_SYSTEM_H_
#define UKIVE_GRAPHICS_COLORS_RGB_SYSTEM_H_


namespace ukive {

    struct RGBSystem {

        enum transfer {
            trc_custom = 0,
            trc_srgb,
            trc_bt709,
            trc_bt2020,
        };

        struct pos {
            float x, y;
        };

        pos red;
        pos green;
        pos blue;
        pos ref_white;
        transfer trc;
        float red_gamma;
        float green_gamma;
        float blue_gamma;

        static pos whitepoint_D63() {
            return { 0.3140f, 0.3510f };
        }
        static pos whitepoint_D65() {
            return { 0.3127f, 0.3290f };
        }

        /**
         * BT.470  B/G
         * BT.601  625
         * BT.1358 625
         * BT.1700 625
         */
        static RGBSystem bt601_pal() {
            RGBSystem s;
            s.red   = { 0.640f, 0.330f };
            s.green = { 0.290f, 0.600f };
            s.blue  = { 0.150f, 0.060f };
            s.ref_white = whitepoint_D65();
            s.trc = trc_bt709;
            s.red_gamma   = 2.22f;
            s.green_gamma = 2.22f;
            s.blue_gamma  = 2.22f;
            return s;
        }

        /**
         * BT.601  525
         * BT.1358 525
         * BT.1700 525
         * SMPTE ST 170 / SMPTE 170M
         * SMPTE ST 240
         */
        static RGBSystem bt601_ntsc() {
            RGBSystem s;
            s.red   = { 0.630f, 0.340f };
            s.green = { 0.310f, 0.595f };
            s.blue  = { 0.155f, 0.070f };
            s.ref_white = whitepoint_D65();
            s.trc = trc_bt709;
            s.red_gamma   = 2.22f;
            s.green_gamma = 2.22f;
            s.blue_gamma  = 2.22f;
            return s;
        }

        /**
         * BT.709
         * BT.1361
         * IEC 61966-2-1 / sRGB
         * IEC 61966-2-4
         * SMPTE RP 177 Annex B
         */
        static RGBSystem bt709() {
            RGBSystem s;
            s.red   = { 0.640f, 0.330f };
            s.green = { 0.300f, 0.600f };
            s.blue  = { 0.150f, 0.060f };
            s.ref_white = whitepoint_D65();
            s.trc = trc_bt709;
            s.red_gamma   = 2.22f;
            s.green_gamma = 2.22f;
            s.blue_gamma  = 2.22f;
            return s;
        }

        /**
         * BT.2020
         * BT.2100
         */
        static RGBSystem bt2020() {
            RGBSystem s;
            s.red   = { 0.708f, 0.292f };
            s.green = { 0.170f, 0.797f };
            s.blue  = { 0.131f, 0.046f };
            s.ref_white = whitepoint_D65();
            s.trc = trc_bt2020;
            s.red_gamma   = 2.22f;
            s.green_gamma = 2.22f;
            s.blue_gamma  = 2.22f;
            return s;
        }

        /**
         * DisplayP3
         * SMPTE EG 432-1
         */
        static RGBSystem display_p3() {
            RGBSystem s;
            s.red   = { 0.680f, 0.320f };
            s.green = { 0.265f, 0.690f };
            s.blue  = { 0.150f, 0.060f };
            s.ref_white = whitepoint_D65();
            s.trc = trc_srgb;
            s.red_gamma   = 2.22f;
            s.green_gamma = 2.22f;
            s.blue_gamma  = 2.22f;
            return s;
        }

        /**
         * DCI-P3
         * SMPTE RP 431-2
         */
        static RGBSystem dci_p3() {
            RGBSystem s;
            s.red   = { 0.680f, 0.320f };
            s.green = { 0.265f, 0.690f };
            s.blue  = { 0.150f, 0.060f };
            s.ref_white = whitepoint_D63();
            s.trc = trc_srgb;
            s.red_gamma   = 2.6f;
            s.green_gamma = 2.6f;
            s.blue_gamma  = 2.6f;
            return s;
        }

    };

}

#endif  // UKIVE_GRAPHICS_COLORS_RGB_SYSTEM_H_