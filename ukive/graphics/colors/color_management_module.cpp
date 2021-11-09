// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "color_management_module.h"

#include <filesystem>
#include <fstream>

#include "ukive/graphics/colors/icc/icc_parser.h"


namespace ukive {

    ColorManagementModule::ColorManagementModule() {}

    ColorManagementModule::~ColorManagementModule() {}

    bool ColorManagementModule::convertColor(
        const std::u16string& icc_path, const Color& src, Color* dst)
    {
        std::ifstream icc_file(std::filesystem::path(icc_path), std::ios::binary);
        if (!icc_file) {
            return false;
        }

        icc::ICCParser parser;
        if (!parser.parse(icc_file)) {
            return false;
        }
    }

}
