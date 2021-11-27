// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_H_

#include <istream>


namespace ukive {
namespace icc {

    struct DateTimeNumber {
        uint16_t year;
        uint16_t month;
        uint16_t day;
        uint16_t hours;
        uint16_t minutes;
        uint16_t seconds;
    };

    struct XYZNumber {
        uint32_t x;
        uint32_t y;
        uint32_t z;
    };


    class ICCType {
    public:
        explicit ICCType(uint32_t type);
        virtual ~ICCType() = default;

        bool parse(std::istream& s);
        bool parse(std::istream& s, uint32_t size);

        bool isParsed() const;
        uint32_t getType() const;

    protected:
        virtual bool onParse(std::istream& s, uint32_t size);
        virtual bool onCheckSize(uint32_t read_size, uint32_t total_size) const;

    private:
        bool is_parsed_ = false;
        uint32_t type_;
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_H_