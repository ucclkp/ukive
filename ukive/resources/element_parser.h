// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_RESOURCES_ELEMENT_PARSER_H_
#define UKIVE_RESOURCES_ELEMENT_PARSER_H_

#include <string>


namespace ukive {

    class Element;

    class ElementParser {
    public:
        static bool parse(
            const std::string_view& s, Element** out);
    };

}

#endif  // UKIVE_RESOURCES_ELEMENT_PARSER_H_