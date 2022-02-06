// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_ICC_TYPES_H_
#define UKIVE_GRAPHICS_COLORS_ICC_ICC_TYPES_H_


namespace ukive {
namespace icc {

    enum ICCParserRCode {
        ICCRC_OK = 0,
        ICCRC_PARSING_FAILED,
        ICCRC_NO_FILE,
        ICCRC_NO_HEADER,
        ICCRC_NOT_FOUND,
        ICCRC_NOT_INVERTIBLE,
        ICCRC_NOT_IMPL,
        ICCRC_WRONG_TYPE,
        ICCRC_NO_DATA,
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_ICC_TYPES_H_