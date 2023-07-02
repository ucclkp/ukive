// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "color.h"

#include <cmath>

#include "utils/log.h"
#include "utils/strings/int_conv.hpp"
#include "utils/strings/string_utils.hpp"


namespace ukive {

    Color::Color()
        : r(0.f), g(0.f), b(0.f), a(0.f) {}

    Color::Color(float r, float g, float b, float a)
        : r(r), g(g), b(b), a(a) {}

    Color::Color(const Color& c, float a)
        : r(c.r), g(c.g), b(c.b), a(a) {}

    // static
    bool Color::parseRGB(
        const std::string_view& color,
        int a, Color* c)
    {
        if (!parseARGB(color, c)) {
            return false;
        }
        c->a = a;
        return true;
    }

    // static
    bool Color::parseARGB(const std::string_view& color, Color* c) {
        if (color.empty() || color.at(0) != '#') {
            return false;
        }

        if (color.length() == 4) {
            int r = utl::ctoi(color[1]);
            int g = utl::ctoi(color[2]);
            int b = utl::ctoi(color[3]);
            if (r >= 16 || g >= 16 || b >= 16) {
                return false;
            }
            r = r * 16 + r;
            g = g * 16 + g;
            b = b * 16 + b;

            *c = ofInt(r, g, b);
            return true;
        }

        if (color.length() == 5) {
            int a = utl::ctoi(color[1]);
            int r = utl::ctoi(color[2]);
            int g = utl::ctoi(color[3]);
            int b = utl::ctoi(color[4]);
            if (r >= 16 || g >= 16 || b >= 16) {
                return false;
            }
            a = a * 16 + a;
            r = r * 16 + r;
            g = g * 16 + g;
            b = b * 16 + b;

            *c = ofInt(r, g, b, a);
            return true;
        }

        if (color.length() == 7) {
            int r, g, b;
            if (!utl::stoi(color.substr(1, 2), &r, 16) ||
                !utl::stoi(color.substr(3, 2), &g, 16) ||
                !utl::stoi(color.substr(5, 2), &b, 16))
            {
                return false;
            }

            *c = ofInt(r, g, b);
            return true;
        }

        if (color.length() == 9) {
            int a, r, g, b;
            if (!utl::stoi(color.substr(1, 2), &a, 16) ||
                !utl::stoi(color.substr(3, 2), &r, 16) ||
                !utl::stoi(color.substr(5, 2), &g, 16) ||
                !utl::stoi(color.substr(7, 2), &b, 16))
            {
                return false;
            }

            *c = ofInt(r, g, b, a);
            return true;
        }

        return false;
    }

    // static
    Color Color::parseARGB(
        const std::string_view& color,
        const Color& def_color)
    {
        Color c;
        if (!parseARGB(color, &c)) {
            return def_color;
        }
        return c;
    }

    // static
    bool Color::parseName(const std::string_view& color, Color* c) {
        if (utl::startWith(color, "red")) {
            auto level = color.substr(3);
            if (level == "50") {
                *c = Red50;
            } else if (level == "100") {
                *c = Red100;
            } else if (level == "200") {
                *c = Red200;
            } else if (level == "300") {
                *c = Red300;
            } else if (level == "400") {
                *c = Red400;
            } else if (level == "500") {
                *c = Red500;
            } else if (level == "600") {
                *c = Red600;
            } else if (level == "700") {
                *c = Red700;
            } else if (level == "800") {
                *c = Red800;
            } else if (level == "900") {
                *c = Red900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "orange")) {
            auto level = color.substr(6);
            if (level == "50") {
                *c = Orange50;
            } else if (level == "100") {
                *c = Orange100;
            } else if (level == "200") {
                *c = Orange200;
            } else if (level == "300") {
                *c = Orange300;
            } else if (level == "400") {
                *c = Orange400;
            } else if (level == "500") {
                *c = Orange500;
            } else if (level == "600") {
                *c = Orange600;
            } else if (level == "700") {
                *c = Orange700;
            } else if (level == "800") {
                *c = Orange800;
            } else if (level == "900") {
                *c = Orange900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "yellow")) {
            auto level = color.substr(6);
            if (level == "50") {
                *c = Yellow50;
            } else if (level == "100") {
                *c = Yellow100;
            } else if (level == "200") {
                *c = Yellow200;
            } else if (level == "300") {
                *c = Yellow300;
            } else if (level == "400") {
                *c = Yellow400;
            } else if (level == "500") {
                *c = Yellow500;
            } else if (level == "600") {
                *c = Yellow600;
            } else if (level == "700") {
                *c = Yellow700;
            } else if (level == "800") {
                *c = Yellow800;
            } else if (level == "900") {
                *c = Yellow900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "pink")) {
            auto level = color.substr(4);
            if (level == "50") {
                *c = Pink50;
            } else if (level == "100") {
                *c = Pink100;
            } else if (level == "200") {
                *c = Pink200;
            } else if (level == "300") {
                *c = Pink300;
            } else if (level == "400") {
                *c = Pink400;
            } else if (level == "500") {
                *c = Pink500;
            } else if (level == "600") {
                *c = Pink600;
            } else if (level == "700") {
                *c = Pink700;
            } else if (level == "800") {
                *c = Pink800;
            } else if (level == "900") {
                *c = Pink900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "green")) {
            auto level = color.substr(5);
            if (level == "50") {
                *c = Green50;
            } else if (level == "100") {
                *c = Green100;
            } else if (level == "200") {
                *c = Green200;
            } else if (level == "300") {
                *c = Green300;
            } else if (level == "400") {
                *c = Green400;
            } else if (level == "500") {
                *c = Green500;
            } else if (level == "600") {
                *c = Green600;
            } else if (level == "700") {
                *c = Green700;
            } else if (level == "800") {
                *c = Green800;
            } else if (level == "900") {
                *c = Green900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "blue")) {
            auto level = color.substr(4);
            if (level == "50") {
                *c = Blue50;
            } else if (level == "100") {
                *c = Blue100;
            } else if (level == "200") {
                *c = Blue200;
            } else if (level == "300") {
                *c = Blue300;
            } else if (level == "400") {
                *c = Blue400;
            } else if (level == "500") {
                *c = Blue500;
            } else if (level == "600") {
                *c = Blue600;
            } else if (level == "700") {
                *c = Blue700;
            } else if (level == "800") {
                *c = Blue800;
            } else if (level == "900") {
                *c = Blue900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "grey")) {
            auto level = color.substr(4);
            if (level == "50") {
                *c = Grey50;
            } else if (level == "100") {
                *c = Grey100;
            } else if (level == "200") {
                *c = Grey200;
            } else if (level == "300") {
                *c = Grey300;
            } else if (level == "400") {
                *c = Grey400;
            } else if (level == "500") {
                *c = Grey500;
            } else if (level == "600") {
                *c = Grey600;
            } else if (level == "700") {
                *c = Grey700;
            } else if (level == "800") {
                *c = Grey800;
            } else if (level == "900") {
                *c = Grey900;
            } else {
                return false;
            }
            return true;
        }

        return false;
    }

    // static
    Color Color::ofInt(int r, int g, int b, int a) {
        return Color(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }

    // static
    Color Color::ofRGB(uint32_t rgb, float a) {
        return Color(
            ((rgb & red_mask) >> red_shift) / 255.f,
            ((rgb & green_mask) >> green_shift) / 255.f,
            ((rgb & blue_mask) >> blue_shift) / 255.f,
            a);
    }

    // static
    Color Color::ofARGB(uint32_t argb) {
        return Color(
            ((argb & red_mask) >> red_shift) / 255.f,
            ((argb & green_mask) >> green_shift) / 255.f,
            ((argb & blue_mask) >> blue_shift) / 255.f,
            ((argb & alpha_mask) >> alpha_shift) / 255.f);
    }

    // static
    int Color::getA(uint32_t argb) {
        return (argb & alpha_mask) >> alpha_shift;
    }

    // static
    int Color::getR(uint32_t argb) {
        return (argb & red_mask) >> red_shift;
    }

    // static
    int Color::getG(uint32_t argb) {
        return (argb & green_mask) >> green_shift;
    }

    // static
    int Color::getB(uint32_t argb) {
        return (argb & blue_mask) >> blue_shift;
    }

    // static
    float Color::linearToSRGB(float L) {
        if (L < 0) { return 0; }
        if (L > 1) { return 1; }

        if (L <= 0.0031308f) {
            return L * 12.92f;
        }

        return float(1.055 * std::pow(L, 1 / 2.4) - 0.055);
    }

    // static
    void Color::linearToSRGB(Color* c) {
        c->r = linearToSRGB(c->r);
        c->g = linearToSRGB(c->g);
        c->b = linearToSRGB(c->b);
    }

    // static
    float Color::sRGBToLinear(float S) {
        if (S < 0) { return 0; }
        if (S > 1) { return 1; }

        if (S <= 0.04045f) {
            return S / 12.92f;
        }

        return float(std::pow((S + 0.055) / 1.055, 2.4));
    }

    // static
    void Color::sRGBToLinear(Color* c) {
        c->r = sRGBToLinear(c->r);
        c->g = sRGBToLinear(c->g);
        c->b = sRGBToLinear(c->b);
    }


    // Color White.
    const Color Color::White = Color::parseARGB("#FFFFFF");
    // Color Black.
    const Color Color::Black = Color::parseARGB("#000000");
    // Color Transparent.
    const Color Color::Transparent = Color::parseARGB("#00000000");

    // Material Color Red.
    Color Color::Red50 = Color::parseARGB("#FFEBEE");
    Color Color::Red100 = Color::parseARGB("#FFCDD2");
    Color Color::Red200 = Color::parseARGB("#EF9A9A");
    Color Color::Red300 = Color::parseARGB("#E57373");
    Color Color::Red400 = Color::parseARGB("#EF5350");
    Color Color::Red500 = Color::parseARGB("#F44336");
    Color Color::Red600 = Color::parseARGB("#E53935");
    Color Color::Red700 = Color::parseARGB("#D32F2F");
    Color Color::Red800 = Color::parseARGB("#C62828");
    Color Color::Red900 = Color::parseARGB("#B71C1C");

    // Material Color Orange.
    Color Color::Orange50 = Color::parseARGB("#FFF3E0");
    Color Color::Orange100 = Color::parseARGB("#FFE0B2");
    Color Color::Orange200 = Color::parseARGB("#FFCC80");
    Color Color::Orange300 = Color::parseARGB("#FFB74D");
    Color Color::Orange400 = Color::parseARGB("#FFA726");
    Color Color::Orange500 = Color::parseARGB("#FF9800");
    Color Color::Orange600 = Color::parseARGB("#FB8C00");
    Color Color::Orange700 = Color::parseARGB("#F57C00");
    Color Color::Orange800 = Color::parseARGB("#EF6C00");
    Color Color::Orange900 = Color::parseARGB("#E65100");

    // Material Color Yellow.
    Color Color::Yellow50 = Color::parseARGB("#FFFDE7");
    Color Color::Yellow100 = Color::parseARGB("#FFF9C4");
    Color Color::Yellow200 = Color::parseARGB("#FFF59D");
    Color Color::Yellow300 = Color::parseARGB("#FFF176");
    Color Color::Yellow400 = Color::parseARGB("#FFEE58");
    Color Color::Yellow500 = Color::parseARGB("#FFEB3B");
    Color Color::Yellow600 = Color::parseARGB("#FDD835");
    Color Color::Yellow700 = Color::parseARGB("#FBC02D");
    Color Color::Yellow800 = Color::parseARGB("#F9A825");
    Color Color::Yellow900 = Color::parseARGB("#F57F17");

    // Material Color Pink.
    Color Color::Pink50 = Color::parseARGB("#FCE4EC");
    Color Color::Pink100 = Color::parseARGB("#F8BBD0");
    Color Color::Pink200 = Color::parseARGB("#F48FB1");
    Color Color::Pink300 = Color::parseARGB("#F06292");
    Color Color::Pink400 = Color::parseARGB("#EC407A");
    Color Color::Pink500 = Color::parseARGB("#E91E63");
    Color Color::Pink600 = Color::parseARGB("#D81B60");
    Color Color::Pink700 = Color::parseARGB("#C2185B");
    Color Color::Pink800 = Color::parseARGB("#AD1457");
    Color Color::Pink900 = Color::parseARGB("#880E4F");

    // Material Color Green.
    Color Color::Green50 = Color::parseARGB("#E8F5E9");
    Color Color::Green100 = Color::parseARGB("#C8E6C9");
    Color Color::Green200 = Color::parseARGB("#A5D6A7");
    Color Color::Green300 = Color::parseARGB("#81C784");
    Color Color::Green400 = Color::parseARGB("#66BB6A");
    Color Color::Green500 = Color::parseARGB("#4CAF50");
    Color Color::Green600 = Color::parseARGB("#43A047");
    Color Color::Green700 = Color::parseARGB("#388E3C");
    Color Color::Green800 = Color::parseARGB("#2E7D32");
    Color Color::Green900 = Color::parseARGB("#1B5E20");

    // Material Color Blue.
    Color Color::Blue50 = Color::parseARGB("#E3F2FD");
    Color Color::Blue100 = Color::parseARGB("#BBDEFB");
    Color Color::Blue200 = Color::parseARGB("#90CAF9");
    Color Color::Blue300 = Color::parseARGB("#64B5F6");
    Color Color::Blue400 = Color::parseARGB("#42A5F5");
    Color Color::Blue500 = Color::parseARGB("#2196F3");
    Color Color::Blue600 = Color::parseARGB("#1E88E5");
    Color Color::Blue700 = Color::parseARGB("#1976D2");
    Color Color::Blue800 = Color::parseARGB("#1565C0");
    Color Color::Blue900 = Color::parseARGB("#0D47A1");

    // Material Color Grey.
    Color Color::Grey50 = Color::parseARGB("#FAFAFA");
    Color Color::Grey100 = Color::parseARGB("#F5F5F5");
    Color Color::Grey200 = Color::parseARGB("#EEEEEE");
    Color Color::Grey300 = Color::parseARGB("#E0E0E0");
    Color Color::Grey400 = Color::parseARGB("#BDBDBD");
    Color Color::Grey500 = Color::parseARGB("#9E9E9E");
    Color Color::Grey600 = Color::parseARGB("#757575");
    Color Color::Grey700 = Color::parseARGB("#616161");
    Color Color::Grey800 = Color::parseARGB("#424242");
    Color Color::Grey900 = Color::parseARGB("#212121");

}
