// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "element_parser.h"

#include "attr_utils.h"

#include "utils/strings/string_utils.hpp"

#include "ukive/elements/element.h"


namespace ukive {

    // static
    bool ElementParser::parse(
        const std::string_view& s, Element** out)
    {
        if (utl::startWith(s, "@color/")) {
            size_t token_len = 7;
            auto end = s.find(',', token_len);
            if (end == std::string_view::npos) {
                end = s.length();
            }

            auto color_str = s.substr(token_len, end - token_len);
            if (color_str.empty()) {
                return false;
            }

            Color c;
            if (color_str[0] == '#') {
                if (!Color::parseARGB(color_str, &c)) {
                    return false;
                }
            } else {
                if (!Color::parseName(color_str, &c)) {
                    return false;
                }
            }

            *out = new Element(c);
            return true;
        }

        if (utl::startWith(s, "@element/")) {
            size_t token_len = 9;
            auto end = s.find(',', token_len);
            if (end == std::string_view::npos) {
                end = s.length();
            }

            auto color_str = s.substr(token_len, end - token_len);
            if (color_str.empty()) {
                return false;
            }

            return false;
        }

        return false;
    }

}