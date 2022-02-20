// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_RANGE_HPP_
#define UKIVE_TEXT_RANGE_HPP_

#include <algorithm>
#include <cstddef>


namespace ukive {

    template<typename Ty>
    class RangeT {
    public:
        RangeT()
            : pos(0), length(0) {}
        RangeT(Ty pos, Ty length)
            : pos(pos), length(length) {}

        void same(const RangeT& rhs) {
            auto _pos = (std::max)(pos, rhs.pos);
            auto _end = (std::min)(end(), rhs.end());
            if (_end < _pos) {
                pos = 0;
                length = 0;
            } else {
                pos = _pos;
                length = _end - _pos;
            }
        }

        void set(Ty pos, Ty length) {
            this->pos = pos;
            this->length = length;
        }

        Ty end() const {
            return pos + length;
        }

        bool intersect(const RangeT& rhs) const {
            auto _pos = (std::max)(pos, rhs.pos);
            auto _end = (std::min)(end(), rhs.end());
            return _end > _pos;
        }

        bool empty() const {
            return length == 0;
        }

        Ty pos;
        Ty length;
    };

    using Range = RangeT<size_t>;

}

#endif  // UKIVE_TEXT_RANGE_HPP_