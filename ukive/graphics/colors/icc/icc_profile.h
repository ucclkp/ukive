// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_ICC_PROFILE_H_
#define UKIVE_GRAPHICS_COLORS_ICC_ICC_PROFILE_H_

#include <filesystem>
#include <fstream>

#include "ukive/graphics/colors/icc/icc_parser.h"


namespace ukive {
namespace icc {

    class ICCProfile {
    public:
        ICCProfile();

        bool load(const std::filesystem::path& icc_file_path);
        bool open();
        void close();

        int getHeader(ICCParser::ProfileHeader* header) const;
        int obtainTagData(uint32_t tag_sign, const ICCType** type);

    private:
        ICCParser parser_;
        std::ifstream icc_fs_;
        std::filesystem::path icc_file_path_;
        bool header_parsed_ = false;
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_ICC_PROFILE_H_