// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_SELECTION_HPP_
#define UKIVE_TEXT_SELECTION_HPP_

#include <algorithm>
#include <cstddef>


namespace ukive {

    template<typename Ty>
    class SelectionT {
    public:
        SelectionT()
            : start(0), end(0) {}
        SelectionT(Ty start, Ty end)
            : start(start), end(end) {}

        void same(const SelectionT& rhs) {
            SelectionT ln(*this);
            ln.nor();

            SelectionT rn(rhs);
            rn.nor();

            auto _start = (std::max)(ln.start, rn.start);
            auto _end = (std::min)(ln.end, rn.end);
            if (_end < _start) {
                start = 0;
                end = 0;
            } else {
                start = _start;
                end = _end;
            }
        }

        void set(Ty pos) {
            this->start = pos;
            this->end = pos;
        }
        void set(Ty start, Ty end) {
            this->start = start;
            this->end = end;
        }
        void nor() {
            if (start > end) {
                swap();
            }
        }
        void swap() {
            auto tmp = start;
            start = end;
            end = tmp;
        }
        void reduce_to_start() {
            end = start;
        }
        void reduce_to_end() {
            start = end;
        }
        void zero() {
            start = 0;
            end = 0;
        }
        void offset(Ty off) {
            start += off;
            end += off;
        }

        bool equal(Ty start, Ty end) const {
            return this->start == start && this->end == end;
        }

        Ty length() const {
            return start <= end ? (end - start) : (start - end);
        }

        bool intersect(const SelectionT& rhs) const {
            SelectionT ln(*this);
            ln.nor();

            SelectionT rn(rhs);
            rn.nor();

            auto _start = (std::max)(ln.start, rn.start);
            auto _end = (std::min)(ln.end, rn.end);
            return _end > _start;
        }

        bool is_nor() const {
            return start <= end;
        }

        bool is_zero() const {
            return start == 0 && end == 0;
        }

        bool empty() const {
            return start == end;
        }

        Ty start;
        Ty end;
    };

    using Selection = SelectionT<size_t>;

}

#endif  // UKIVE_TEXT_SELECTION_HPP_