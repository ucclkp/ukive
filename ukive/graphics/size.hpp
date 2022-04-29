// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_SIZE_HPP_
#define UKIVE_GRAPHICS_SIZE_HPP_

#include <cstddef>

#include "utils/numbers.hpp"


namespace ukive {

    template <typename Ty>
    class SizeT {
    public:
        using type = Ty;

        SizeT()
            : w_(0), h_(0) {}
        SizeT(Ty w, Ty h)
            : w_(w), h_(h) {}

        SizeT operator&(const SizeT& rhs) const {
            SizeT out(*this);
            out.same(rhs);
            return out;
        }
        SizeT operator|(const SizeT& rhs) const {
            SizeT out(*this);
            out.join(rhs);
            return out;
        }
        SizeT& operator&=(const SizeT& rhs) {
            same(rhs);
            return *this;
        }
        SizeT& operator|=(const SizeT& rhs) {
            join(rhs);
            return *this;
        }
        bool operator==(const SizeT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const SizeT& rhs) const {
            return !equal(rhs);
        }

        template<typename C>
        explicit operator SizeT<C>() const {
            return SizeT<C>(static_cast<C>(w_), static_cast<C>(h_));
        }

        bool empty() const {
            return w_ <= 0 || h_ <= 0;
        }

        bool equal(const SizeT& rhs) const {
            return utl::is_num_equal(w_, rhs.w_) &&
                utl::is_num_equal(h_, rhs.h_);
        }

        SizeT& set(Ty w, Ty h) {
            w_ = w;
            h_ = h;
            return *this;
        }

        SizeT& join(const SizeT& rhs) {
            if (rhs.w_ > w_) {
                w_ = rhs.w_;
            }
            if (rhs.h_ > h_) {
                h_ = rhs.h_;
            }
            return *this;
        }

        SizeT& same(const SizeT& rhs) {
            if (rhs.w_ < w_) {
                w_ = rhs.w_;
            }
            if (rhs.h_ < h_) {
                h_ = rhs.h_;
            }
            return *this;
        }

        SizeT& insets(const SizeT& rhs) {
            insets(rhs.w_, rhs.h_);
            return *this;
        }

        SizeT& insets(Ty w, Ty h) {
            if (w < w_) {
                w_ = w_ - w;
            } else {
                w_ = 0;
            }

            if (h < h_) {
                h_ = h_ - h;
            } else {
                h_ = 0;
            }
            return *this;
        }

        SizeT& width(Ty w) {
            w_ = w;
            return *this;
        }
        SizeT& height(Ty h) {
            h_ = h;
            return *this;
        }

        Ty width() const { return w_; }
        Ty height() const { return h_; }

    private:
        Ty w_, h_;
    };

    using Size = SizeT<int>;
    using SizeU = SizeT<unsigned int>;
    using SizeS = SizeT<size_t>;
    using SizeF = SizeT<float>;

}

#endif  // UKIVE_GRAPHICS_SIZE_HPP_
