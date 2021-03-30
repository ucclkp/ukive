// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_RESOURCES_ATTRIBUTES_H_
#define UKIVE_RESOURCES_ATTRIBUTES_H_

#include <map>
#include <string>


namespace ukive {

    using Attributes = std::map<std::string, std::string>;
    using AttrsRef = const Attributes&;

}

#endif  // UKIVE_RESOURCES_ATTRIBUTES_H_