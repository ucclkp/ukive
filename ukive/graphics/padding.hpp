// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_PADDING_H_
#define UKIVE_GRAPHICS_PADDING_H_


namespace ukive {

    template <typename Ty>
    class PaddingT {
    public:
        PaddingT()
            : start(0), top(0), end(0), bottom(0) {}
        PaddingT(Ty start, Ty top, Ty end, Ty bottom)
            : start(start), top(top), end(end), bottom(bottom) {}

        bool operator==(const PaddingT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const PaddingT& rhs) const {
            return !equal(rhs);
        }

        PaddingT operator+(const PaddingT& rhs) const {
            PaddingT out(*this);
            out.add(rhs);
            return out;
        }

        void set(Ty start, Ty top, Ty end, Ty bottom) {
            this->start = start;
            this->top = top;
            this->end = end;
            this->bottom = bottom;
        }

        void add(Ty start, Ty top, Ty end, Ty bottom) {
            this->start += start;
            this->top += top;
            this->end += end;
            this->bottom += bottom;
        }

        void add(const PaddingT& rhs) {
            this->start += rhs.start;
            this->top += rhs.top;
            this->end += rhs.end;
            this->bottom += rhs.bottom;
        }

        Ty vert() const {
            return top + bottom;
        }

        Ty hori() const {
            return start + end;
        }

        bool equal(const PaddingT& rhs) const {
            return start == rhs.start &&
                top == rhs.top &&
                end == rhs.end &&
                bottom == rhs.bottom;
        }

        bool empty() const {
            return start == 0 && top == 0 && end == 0 && bottom == 0;
        }

        Ty start;
        Ty top;
        Ty end;
        Ty bottom;
    };

    using Margin = PaddingT<int>;
    using Padding = PaddingT<int>;

}

#endif  // UKIVE_GRAPHICS_PADDING_H_