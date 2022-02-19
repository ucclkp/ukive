// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_SIZE_HPP_
#define UKIVE_GRAPHICS_SIZE_HPP_

#include <cstddef>


namespace ukive {

    template <typename Ty>
    class SizeT {
    public:
        using type = Ty;

        SizeT()
            : width(0), height(0) {}

        SizeT(Ty w, Ty h)
            : width(w), height(h) {}

        bool operator==(const SizeT& rhs) const {
            return width == rhs.width && height == rhs.height;
        }
        bool operator!=(const SizeT& rhs) const {
            return width != rhs.width || height != rhs.height;
        }

        template<typename C>
        explicit operator SizeT<C>() const {
            return SizeT<C>(static_cast<C>(width), static_cast<C>(height));
        }

        bool empty() {
            return width <= 0 || height <= 0;
        }

        void set(Ty w, Ty h) {
            width = w;
            height = h;
        }

        void setToMax(const SizeT& rhs) {
            if (rhs.width > width) {
                width = rhs.width;
            }
            if (rhs.height > height) {
                height = rhs.height;
            }
        }

        void setToMin(const SizeT& rhs) {
            if (rhs.width < width) {
                width = rhs.width;
            }
            if (rhs.height < height) {
                height = rhs.height;
            }
        }

        void insets(const SizeT& rhs) {
            insets(rhs.width, rhs.height);
        }

        void insets(Ty w, Ty h) {
            if (w < width) {
                width = width - w;
            } else {
                width = 0;
            }

            if (h < height) {
                height = height - h;
            } else {
                height = 0;
            }
        }

        Ty width, height;
    };

    using Size = SizeT<int>;
    using SizeU = SizeT<unsigned int>;
    using SizeS = SizeT<size_t>;
    using SizeF = SizeT<float>;

}

#endif  // UKIVE_GRAPHICS_SIZE_HPP_
