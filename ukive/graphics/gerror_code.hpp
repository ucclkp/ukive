// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GERROR_CODE_HPP_
#define UKIVE_GRAPHICS_GERROR_CODE_HPP_


namespace ukive {

    class gerc {
    public:
        gerc()
            : raw_code_(0) {}
        gerc(long raw_code)
            : raw_code_(raw_code) {}

        bool operator==(const gerc& rhs) const {
            return raw_code_ == rhs.raw_code_;
        }

        bool operator!=(const gerc& rhs) const {
            return raw_code_ != rhs.raw_code_;
        }

        explicit operator bool() const {
            return raw_code_ == 0;
        }

        long raw_code() const {
            return raw_code_;
        }

    private:
        long raw_code_;
    };

}

#endif  // UKIVE_GRAPHICS_GERROR_CODE_HPP_