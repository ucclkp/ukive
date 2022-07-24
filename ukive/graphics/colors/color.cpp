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
        : a(0.f), r(0.f), g(0.f), b(0.f) {}

    Color::Color(float r, float g, float b, float a)
        : a(a), r(r), g(g), b(b) {}

    Color::Color(const Color& c, float a)
        : a(a), r(c.r), g(c.g), b(c.b) {}

    // static
    Color Color::parse(const std::string_view& color) {
        Color c;
        if (!parse(color, &c)) {
            LOG(Log::ERR) << "Unknown color: " << color;
            return Red500;
        }
        return c;
    }

    // static
    bool Color::parse(const std::string_view& color, Color* c) {
        if (color.empty() || color.at(0) != '#') {
            return false;
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
    bool Color::parseLiteral(const std::string_view& color, Color* c) {
        if (utl::startWith(color, "red")) {
            if (color.substr(3) == "50") {
                *c = Red50;
            } else if (color.substr(3) == "100") {
                *c = Red100;
            } else if (color.substr(3) == "200") {
                *c = Red200;
            } else if (color.substr(3) == "300") {
                *c = Red300;
            } else if (color.substr(3) == "400") {
                *c = Red400;
            } else if (color.substr(3) == "500") {
                *c = Red500;
            } else if (color.substr(3) == "600") {
                *c = Red600;
            } else if (color.substr(3) == "700") {
                *c = Red700;
            } else if (color.substr(3) == "800") {
                *c = Red800;
            } else if (color.substr(3) == "900") {
                *c = Red900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "orange")) {
            if (color.substr(3) == "50") {
                *c = Orange50;
            } else if (color.substr(3) == "100") {
                *c = Orange100;
            } else if (color.substr(3) == "200") {
                *c = Orange200;
            } else if (color.substr(3) == "300") {
                *c = Orange300;
            } else if (color.substr(3) == "400") {
                *c = Orange400;
            } else if (color.substr(3) == "500") {
                *c = Orange500;
            } else if (color.substr(3) == "600") {
                *c = Orange600;
            } else if (color.substr(3) == "700") {
                *c = Orange700;
            } else if (color.substr(3) == "800") {
                *c = Orange800;
            } else if (color.substr(3) == "900") {
                *c = Orange900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "yellow")) {
            if (color.substr(3) == "50") {
                *c = Yellow50;
            } else if (color.substr(3) == "100") {
                *c = Yellow100;
            } else if (color.substr(3) == "200") {
                *c = Yellow200;
            } else if (color.substr(3) == "300") {
                *c = Yellow300;
            } else if (color.substr(3) == "400") {
                *c = Yellow400;
            } else if (color.substr(3) == "500") {
                *c = Yellow500;
            } else if (color.substr(3) == "600") {
                *c = Yellow600;
            } else if (color.substr(3) == "700") {
                *c = Yellow700;
            } else if (color.substr(3) == "800") {
                *c = Yellow800;
            } else if (color.substr(3) == "900") {
                *c = Yellow900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "pink")) {
            if (color.substr(3) == "50") {
                *c = Pink50;
            } else if (color.substr(3) == "100") {
                *c = Pink100;
            } else if (color.substr(3) == "200") {
                *c = Pink200;
            } else if (color.substr(3) == "300") {
                *c = Pink300;
            } else if (color.substr(3) == "400") {
                *c = Pink400;
            } else if (color.substr(3) == "500") {
                *c = Pink500;
            } else if (color.substr(3) == "600") {
                *c = Pink600;
            } else if (color.substr(3) == "700") {
                *c = Pink700;
            } else if (color.substr(3) == "800") {
                *c = Pink800;
            } else if (color.substr(3) == "900") {
                *c = Pink900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "green")) {
            if (color.substr(3) == "50") {
                *c = Green50;
            } else if (color.substr(3) == "100") {
                *c = Green100;
            } else if (color.substr(3) == "200") {
                *c = Green200;
            } else if (color.substr(3) == "300") {
                *c = Green300;
            } else if (color.substr(3) == "400") {
                *c = Green400;
            } else if (color.substr(3) == "500") {
                *c = Green500;
            } else if (color.substr(3) == "600") {
                *c = Green600;
            } else if (color.substr(3) == "700") {
                *c = Green700;
            } else if (color.substr(3) == "800") {
                *c = Green800;
            } else if (color.substr(3) == "900") {
                *c = Green900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "blue")) {
            if (color.substr(3) == "50") {
                *c = Blue50;
            } else if (color.substr(3) == "100") {
                *c = Blue100;
            } else if (color.substr(3) == "200") {
                *c = Blue200;
            } else if (color.substr(3) == "300") {
                *c = Blue300;
            } else if (color.substr(3) == "400") {
                *c = Blue400;
            } else if (color.substr(3) == "500") {
                *c = Blue500;
            } else if (color.substr(3) == "600") {
                *c = Blue600;
            } else if (color.substr(3) == "700") {
                *c = Blue700;
            } else if (color.substr(3) == "800") {
                *c = Blue800;
            } else if (color.substr(3) == "900") {
                *c = Blue900;
            } else {
                return false;
            }
            return true;
        }

        if (utl::startWith(color, "grey")) {
            if (color.substr(3) == "50") {
                *c = Grey50;
            } else if (color.substr(3) == "100") {
                *c = Grey100;
            } else if (color.substr(3) == "200") {
                *c = Grey200;
            } else if (color.substr(3) == "300") {
                *c = Grey300;
            } else if (color.substr(3) == "400") {
                *c = Grey400;
            } else if (color.substr(3) == "500") {
                *c = Grey500;
            } else if (color.substr(3) == "600") {
                *c = Grey600;
            } else if (color.substr(3) == "700") {
                *c = Grey700;
            } else if (color.substr(3) == "800") {
                *c = Grey800;
            } else if (color.substr(3) == "900") {
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
    const Color Color::White = Color::parse("#FFFFFF");
    // Color Black.
    const Color Color::Black = Color::parse("#000000");
    // Color Transparent.
    const Color Color::Transparent = Color::parse("#00000000");

    // Material Color Red.
    Color Color::Red50 = Color::parse("#FFEBEE");
    Color Color::Red100 = Color::parse("#FFCDD2");
    Color Color::Red200 = Color::parse("#EF9A9A");
    Color Color::Red300 = Color::parse("#E57373");
    Color Color::Red400 = Color::parse("#EF5350");
    Color Color::Red500 = Color::parse("#F44336");
    Color Color::Red600 = Color::parse("#E53935");
    Color Color::Red700 = Color::parse("#D32F2F");
    Color Color::Red800 = Color::parse("#C62828");
    Color Color::Red900 = Color::parse("#B71C1C");

    // Material Color Orange.
    Color Color::Orange50 = Color::parse("#FFF3E0");
    Color Color::Orange100 = Color::parse("#FFE0B2");
    Color Color::Orange200 = Color::parse("#FFCC80");
    Color Color::Orange300 = Color::parse("#FFB74D");
    Color Color::Orange400 = Color::parse("#FFA726");
    Color Color::Orange500 = Color::parse("#FF9800");
    Color Color::Orange600 = Color::parse("#FB8C00");
    Color Color::Orange700 = Color::parse("#F57C00");
    Color Color::Orange800 = Color::parse("#EF6C00");
    Color Color::Orange900 = Color::parse("#E65100");

    // Material Color Yellow.
    Color Color::Yellow50 = Color::parse("#FFFDE7");
    Color Color::Yellow100 = Color::parse("#FFF9C4");
    Color Color::Yellow200 = Color::parse("#FFF59D");
    Color Color::Yellow300 = Color::parse("#FFF176");
    Color Color::Yellow400 = Color::parse("#FFEE58");
    Color Color::Yellow500 = Color::parse("#FFEB3B");
    Color Color::Yellow600 = Color::parse("#FDD835");
    Color Color::Yellow700 = Color::parse("#FBC02D");
    Color Color::Yellow800 = Color::parse("#F9A825");
    Color Color::Yellow900 = Color::parse("#F57F17");

    // Material Color Pink.
    Color Color::Pink50 = Color::parse("#FCE4EC");
    Color Color::Pink100 = Color::parse("#F8BBD0");
    Color Color::Pink200 = Color::parse("#F48FB1");
    Color Color::Pink300 = Color::parse("#F06292");
    Color Color::Pink400 = Color::parse("#EC407A");
    Color Color::Pink500 = Color::parse("#E91E63");
    Color Color::Pink600 = Color::parse("#D81B60");
    Color Color::Pink700 = Color::parse("#C2185B");
    Color Color::Pink800 = Color::parse("#AD1457");
    Color Color::Pink900 = Color::parse("#880E4F");

    // Material Color Green.
    Color Color::Green50 = Color::parse("#E8F5E9");
    Color Color::Green100 = Color::parse("#C8E6C9");
    Color Color::Green200 = Color::parse("#A5D6A7");
    Color Color::Green300 = Color::parse("#81C784");
    Color Color::Green400 = Color::parse("#66BB6A");
    Color Color::Green500 = Color::parse("#4CAF50");
    Color Color::Green600 = Color::parse("#43A047");
    Color Color::Green700 = Color::parse("#388E3C");
    Color Color::Green800 = Color::parse("#2E7D32");
    Color Color::Green900 = Color::parse("#1B5E20");

    // Material Color Blue.
    Color Color::Blue50 = Color::parse("#E3F2FD");
    Color Color::Blue100 = Color::parse("#BBDEFB");
    Color Color::Blue200 = Color::parse("#90CAF9");
    Color Color::Blue300 = Color::parse("#64B5F6");
    Color Color::Blue400 = Color::parse("#42A5F5");
    Color Color::Blue500 = Color::parse("#2196F3");
    Color Color::Blue600 = Color::parse("#1E88E5");
    Color Color::Blue700 = Color::parse("#1976D2");
    Color Color::Blue800 = Color::parse("#1565C0");
    Color Color::Blue900 = Color::parse("#0D47A1");

    // Material Color Grey.
    Color Color::Grey50 = Color::parse("#FAFAFA");
    Color Color::Grey100 = Color::parse("#F5F5F5");
    Color Color::Grey200 = Color::parse("#EEEEEE");
    Color Color::Grey300 = Color::parse("#E0E0E0");
    Color Color::Grey400 = Color::parse("#BDBDBD");
    Color Color::Grey500 = Color::parse("#9E9E9E");
    Color Color::Grey600 = Color::parse("#757575");
    Color Color::Grey700 = Color::parse("#616161");
    Color Color::Grey800 = Color::parse("#424242");
    Color Color::Grey900 = Color::parse("#212121");

}
