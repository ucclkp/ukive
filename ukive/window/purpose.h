// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_PURPOSE_H_
#define UKIVE_WINDOW_PURPOSE_H_

#include <map>


namespace ukive {

    class Purpose {
    public:
        Purpose() = default;

        std::map<std::string, std::u16string> params;
    };

}

#endif  // UKIVE_WINDOW_PURPOSE_H_