// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_profile.h"

#include <cassert>
#include <fstream>

#include "ukive/graphics/colors/icc/icc_types.h"


namespace ukive {
namespace icc {

    ICCProfile::ICCProfile() {}

    bool ICCProfile::load(const std::filesystem::path& icc_file_path) {
        icc_file_path_ = icc_file_path;
        header_parsed_ = false;

        icc_fs_.open(icc_file_path, std::ios::binary);
        if (!icc_fs_) {
            return false;
        }

        if (!parser_.parseHeader(icc_fs_)) {
            return false;
        }

        header_parsed_ = true;
        return true;
    }

    bool ICCProfile::open() {
        if (icc_fs_.is_open()) {
            return true;
        }

        icc_fs_.open(icc_file_path_, std::ios::binary);
        if (!icc_fs_) {
            return false;
        }

        return true;
    }

    void ICCProfile::close() {
        icc_fs_.close();
        assert(!!icc_fs_);
    }

    int ICCProfile::getHeader(ICCParser::ProfileHeader* header) const {
        if (!header_parsed_) {
            return ICCRC_NOT_FOUND;
        }
        *header = parser_.getHeader();
        return ICCRC_OK;
    }

    int ICCProfile::obtainTagData(uint32_t tag_sign, const ICCType** type) {
        if (!header_parsed_) {
            return ICCRC_NO_HEADER;
        }
        if (!open()) {
            return ICCRC_NO_FILE;
        }
        return parser_.obtainTagData(icc_fs_, tag_sign, type);
    }

}
}
