// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLOR_H_
#define UKIVE_GRAPHICS_COLOR_H_

#include <string>


namespace ukive {

    class Color {
    public:
        Color();
        Color(float r, float g, float b, float a = 1.f);

        float a;
        float r;
        float g;
        float b;

    public:
        // 解析颜色字符串。
        // 支持的格式：#RRGGBB 或 #AARRGGBB
        static Color parse(const std::string_view& color);

        static Color ofInt(int r, int g, int b, int a = 255);
        static Color ofRGB(uint32_t rgb, float a = 1.f);
        static Color ofARGB(uint32_t argb);

        static int getA(uint32_t argb);
        static int getR(uint32_t argb);
        static int getG(uint32_t argb);
        static int getB(uint32_t argb);

        // Color Black.
        const static Color Black;
        // Color White.
        const static Color White;
        // Color Transparent
        const static Color Transparent;

        // Material Color Red.
        static Color Red50, Red100, Red200, Red300, Red400,
            Red500, Red600, Red700, Red800, Red900;

        // Material Color Orange.
        static Color Orange50, Orange100, Orange200, Orange300, Orange400,
            Orange500, Orange600, Orange700, Orange800, Orange900;

        // Material Color Yellow.
        static Color Yellow50, Yellow100, Yellow200, Yellow300, Yellow400,
            Yellow500, Yellow600, Yellow700, Yellow800, Yellow900;

        // Material Color Pink.
        static Color Pink50, Pink100, Pink200, Pink300, Pink400,
            Pink500, Pink600, Pink700, Pink800, Pink900;

        // Material Color Green.
        static Color Green50, Green100, Green200, Green300, Green400,
            Green500, Green600, Green700, Green800, Green900;

        // Material Color Blue.
        static Color Blue50, Blue100, Blue200, Blue300, Blue400,
            Blue500, Blue600, Blue700, Blue800, Blue900;

        // Material Color Grey.
        static Color Grey50, Grey100, Grey200, Grey300, Grey400,
            Grey500, Grey600, Grey700, Grey800, Grey900;

    private:
        static const uint32_t alpha_shift = 24;
        static const uint32_t red_shift = 16;
        static const uint32_t green_shift = 8;
        static const uint32_t blue_shift = 0;

        static const uint32_t alpha_mask = 0xff << alpha_shift;
        static const uint32_t red_mask = 0xff << red_shift;
        static const uint32_t green_mask = 0xff << green_shift;
        static const uint32_t blue_mask = 0xff << blue_shift;
    };

}

#endif  // UKIVE_GRAPHICS_COLOR_H_