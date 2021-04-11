// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_RECT_H_
#define UKIVE_GRAPHICS_RECT_H_

#include <algorithm>

#include "ukive/graphics/padding.hpp"
#include "ukive/graphics/point.hpp"
#include "ukive/graphics/size.hpp"


namespace ukive {

    template <typename Ty>
    class RectT {
    public:
        using type = Ty;

        RectT()
            : left(0), top(0), right(0), bottom(0) {}
        RectT(Ty x, Ty y, Ty width, Ty height)
            : left(x), top(y), right(x + width), bottom(y + height) {}

        template <typename C>
        explicit operator RectT<C>() const {
            return RectT<C>(
                static_cast<C>(left), static_cast<C>(top),
                static_cast<C>(right - left), static_cast<C>(bottom - top));
        }

        RectT& operator&(const RectT& rhs) {
            same(rhs);
            return *this;
        }
        RectT& operator|(const RectT& rhs) {
            join(rhs);
            return *this;
        }
        bool operator==(const RectT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const RectT& rhs) const {
            return !equal(rhs);
        }

        PointT<Ty> pos() const {
            return PointT<Ty>(left, top);
        }
        SizeT<Ty> size() const {
            return SizeT<Ty>(width(), height());
        }
        Ty width() const {
            if constexpr (std::is_unsigned<Ty>::value) {
                return right >= left ? right - left : left - right;
            } else {
                return right - left;
            }
        }
        Ty height() const {
            if constexpr (std::is_unsigned<Ty>::value) {
                return bottom >= top ? bottom - top : top - bottom;
            } else {
                return bottom - top;
            }
        }

        bool empty() const {
            return (right - left <= 0) || (bottom - top <= 0);
        }
        bool equal(const RectT& rhs) const {
            return (left == rhs.left
                && top == rhs.top
                && right == rhs.right
                && bottom == rhs.bottom);
        }
        bool hit(Ty x, Ty y) const {
            return (x >= left && x < right
                && y >= top && y < bottom);
        }
        bool hit(const PointT<Ty>& p) const {
            return (p.x >= left && p.x < right
                && p.y >= top && p.y < bottom);
        }
        bool contains(const RectT& rect) const {
            return (rect.left >= left && rect.right <= right)
                && (rect.top >= top && rect.bottom <= bottom);
        }
        bool intersect(const RectT& rect) const {
            return (rect.right > left && rect.left < right)
                && (rect.bottom > top && rect.top < bottom);
        }
        void join(const RectT& rhs) {
            if (empty()) {
                left = rhs.left;
                top = rhs.top;
                right = rhs.right;
                bottom = rhs.bottom;
            } else {
                left = std::min(left, rhs.left);
                top = std::min(top, rhs.top);
                right = std::max(right, rhs.right);
                bottom = std::max(bottom, rhs.bottom);
            }
        }
        void same(const RectT& rhs) {
            left = std::max(left, rhs.left);
            top = std::max(top, rhs.top);
            right = std::min(right, rhs.right);
            bottom = std::min(bottom, rhs.bottom);

            if (empty()) {
                left = top = right = bottom = 0;
            }
        }

        void setPos(const PointT<Ty>& pos) {
            setPos(pos.x, pos.y);
        }
        void setPos(Ty x, Ty y) {
            auto w = width();
            auto h = height();

            left = x;
            top = y;
            right = x + w;
            bottom = y + h;
        }
        void setSize(const SizeT<Ty>& size) {
            setSize(size.width, size.height);
        }
        void setSize(Ty width, Ty height) {
            right = left + width;
            bottom = top + height;
        }
        void set(const PointT<Ty>& pos, const SizeT<Ty>& size) {
            this->left = pos.x;
            this->top = pos.y;
            this->right = left + size.width;
            this->bottom = top + size.height;
        }
        void set(Ty left, Ty top, Ty width, Ty height) {
            this->left = left;
            this->top = top;
            this->right = left + width;
            this->bottom = top + height;
        }
        void insets(const PaddingT<Ty>& insets) {
            left += insets.start;
            top += insets.top;
            right -= insets.end;
            bottom -= insets.bottom;
        }
        void insets(Ty left, Ty top, Ty right, Ty bottom) {
            this->left += left;
            this->top += top;
            this->right -= right;
            this->bottom -= bottom;
        }
        void extend(const PaddingT<Ty>& ext) {
            left -= ext.start;
            top -= ext.top;
            right += ext.end;
            bottom += ext.bottom;
        }
        void extend(Ty left, Ty top, Ty right, Ty bottom) {
            this->left -= left;
            this->top -= top;
            this->right += right;
            this->bottom += bottom;
        }
        void offset(Ty dx, Ty dy) {
            left += dx;
            right += dx;
            top += dy;
            bottom += dy;
        }

        Ty left;
        Ty top;
        Ty right;
        Ty bottom;
    };

    using Rect = RectT<int>;
    using RectF = RectT<float>;
    using RectD = RectT<double>;

}

#endif  // UKIVE_GRAPHICS_RECT_H_
