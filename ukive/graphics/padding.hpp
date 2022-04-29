// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_PADDING_H_
#define UKIVE_GRAPHICS_PADDING_H_

#include "utils/numbers.hpp"


namespace ukive {

    template <typename Ty>
    class PaddingT {
    public:
        PaddingT()
            : start_(0), top_(0), end_(0), bottom_(0) {}
        PaddingT(Ty start, Ty top, Ty end, Ty bottom)
            : start_(start), top_(top), end_(end), bottom_(bottom) {}

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

        PaddingT& start(Ty start) {
            this->start_ = start;
            return *this;
        }
        PaddingT& top(Ty top) {
            this->top_ = top;
            return *this;
        }
        PaddingT& end(Ty end) {
            this->end_ = end;
            return *this;
        }
        PaddingT& bottom(Ty bottom) {
            this->bottom_ = bottom;
            return *this;
        }

        PaddingT& set(Ty start, Ty top, Ty end, Ty bottom) {
            this->start_ = start;
            this->top_ = top;
            this->end_ = end;
            this->bottom_ = bottom;
            return *this;
        }

        PaddingT& add(Ty start, Ty top, Ty end, Ty bottom) {
            this->start_ += start;
            this->top_ += top;
            this->end_ += end;
            this->bottom_ += bottom;
            return *this;
        }

        PaddingT& add(const PaddingT& rhs) {
            this->start_ += rhs.start_;
            this->top_ += rhs.top_;
            this->end_ += rhs.end_;
            this->bottom_ += rhs.bottom_;
            return *this;
        }

        Ty vert() const {
            return top_ + bottom_;
        }

        Ty hori() const {
            return start_ + end_;
        }

        bool equal(const PaddingT& rhs) const {
            return start_ == rhs.start_ &&
                top_ == rhs.top_ &&
                end_ == rhs.end_ &&
                bottom_ == rhs.bottom_;
        }

        bool empty() const {
            return utl::is_num_zero(start_) &&
                utl::is_num_zero(top_) &&
                utl::is_num_zero(end_) &&
                utl::is_num_zero(bottom_);
        }

        Ty start() const { return start_; }
        Ty top() const { return top_; }
        Ty end() const { return end_; }
        Ty bottom() const { return bottom_; }

    private:
        Ty start_;
        Ty top_;
        Ty end_;
        Ty bottom_;
    };

    using Margin = PaddingT<int>;
    using Padding = PaddingT<int>;

}

#endif  // UKIVE_GRAPHICS_PADDING_H_