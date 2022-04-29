// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_RECT_HPP_
#define UKIVE_GRAPHICS_RECT_HPP_

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
            : pos_{ 0, 0 } {}
        RectT(Ty x, Ty y, Ty width, Ty height)
            : pos_{ x, y }, size_(width, height) {}
        RectT(const PointT<Ty>& pos, const SizeT<Ty>& size)
            : pos_(pos), size_(size) {}

        template <typename C>
        explicit operator RectT<C>() const {
            return RectT<C>(PointT<C>(pos_), SizeT<C>(size_));
        }

        RectT operator&(const RectT& rhs) const {
            RectT out(*this);
            out.same(rhs);
            return out;
        }
        RectT operator|(const RectT& rhs) const {
            RectT out(*this);
            out.join(rhs);
            return out;
        }
        RectT& operator&=(const RectT& rhs) {
            same(rhs);
            return *this;
        }
        RectT& operator|=(const RectT& rhs) {
            join(rhs);
            return *this;
        }
        bool operator==(const RectT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const RectT& rhs) const {
            return !equal(rhs);
        }

        const PointT<Ty>& pos() const { return pos_; }
        const SizeT<Ty>& size() const { return size_; }
        Ty x() const { return pos_.x(); }
        Ty y() const { return pos_.y(); }
        Ty width() const { return size_.width(); }
        Ty height() const { return size_.height(); }
        Ty right() const { return pos_.x() + size_.width(); }
        Ty bottom() const { return pos_.y() + size_.height(); }

        PointT<Ty> pos_rt() const { return { right(), y() }; }
        PointT<Ty> pos_rb() const { return { right(), bottom() }; }
        PointT<Ty> pos_lb() const { return { x(), bottom() }; }

        bool empty() const {
            return size_.empty();
        }
        bool equal(const RectT& rhs) const {
            return pos_ == rhs.pos_ && size_ == rhs.size_;
        }
        bool hit(Ty x, Ty y) const {
            return (x >= this->x() && x < this->right()
                && y >= this->y() && y < this->bottom());
        }
        bool hit(const PointT<Ty>& p) const {
            return (p.x() >= x() && p.x() < right()
                && p.y() >= y() && p.y() < bottom());
        }
        bool contains(const RectT& rect) const {
            return (rect.x() >= x() && rect.right() <= right())
                && (rect.y() >= y() && rect.bottom() <= bottom());
        }
        bool intersect(const RectT& rect) const {
            return (rect.right() > x() && rect.x() < right())
                && (rect.bottom() > y() && rect.y() < bottom());
        }

        RectT& join(const RectT& rhs) {
            if (empty()) {
                pos_ = rhs.pos_;
                size_ = rhs.size_;
            } else {
                auto l = (std::min)(x(), rhs.x());
                auto t = (std::min)(y(), rhs.y());
                auto r = (std::max)(right(), rhs.right());
                auto b = (std::max)(bottom(), rhs.bottom());

                if (r < l) r = l;
                if (b < t) b = t;

                xywh(l, t, r - l, b - t);
            }
            return *this;
        }
        RectT& same(const RectT& rhs) {
            auto l = (std::max)(x(), rhs.x());
            auto t = (std::max)(y(), rhs.y());
            auto r = (std::min)(right(), rhs.right());
            auto b = (std::min)(bottom(), rhs.bottom());

            if (r < l) r = l;
            if (b < t) b = t;

            return xywh(l, t, r - l, b - t);
        }

        RectT& x(Ty x) {
            pos_.x(x);
            return *this;
        }
        RectT& y(Ty y) {
            pos_.y(y);
            return *this;
        }
        RectT& pos(const PointT<Ty>& pos) {
            pos_ = pos;
            return *this;
        }
        RectT& pos(Ty x, Ty y) {
            pos_ = { x, y };
            return *this;
        }
        RectT& width(Ty width) {
            size_.width(width);
            return *this;
        }
        RectT& height(Ty height) {
            size_.height(height);
            return *this;
        }
        RectT& size(const SizeT<Ty>& size) {
            size_ = size;
            return *this;
        }
        RectT& size(Ty width, Ty height) {
            size_.set(width, height);
            return *this;
        }
        RectT& set(const PointT<Ty>& pos, const SizeT<Ty>& size) {
            pos_ = pos;
            size_ = size;
            return *this;
        }
        RectT& xywh(Ty x, Ty y, Ty width, Ty height) {
            pos_ = { x, y };
            size_.set(width, height);
            return *this;
        }
        RectT& right(Ty r) {
            if (r < pos_.x()) {
                size_.width(0);
            } else {
                size_.width(r - pos_.x());
            }
            return *this;
        }
        RectT& bottom(Ty b) {
            if (b < pos_.y()) {
                size_.height(0);
            } else {
                size_.height(b - pos_.y());
            }
            return *this;
        }
        RectT& xyrb(Ty x, Ty y, Ty r, Ty b) {
            pos_ = { x, y };
            right(r);
            bottom(b);
            return *this;
        }

        RectT& insets(const PaddingT<Ty>& insets) {
            pos_.x() += insets.start();
            pos_.y() += insets.top();

            if (insets.hori() > size_.width()) {
                size_.width(0);
            } else {
                size_.width(size_.width() - insets.hori());
            }

            if (insets.vert() > size_.height()) {
                size_.height(0);
            } else {
                size_.height(size_.height() - insets.vert());
            }
            return *this;
        }
        RectT& insets(Ty x, Ty y, Ty r, Ty b) {
            pos_.x() += x;
            pos_.y() += y;

            if (x + r > size_.width()) {
                size_.width(0);
            } else {
                size_.width(size_.width() - x + r);
            }

            if (y + b > size_.height()) {
                size_.height(0);
            } else {
                size_.height(size_.height() - y + b);
            }
            return *this;
        }
        RectT& extend(const PaddingT<Ty>& ext) {
            pos_.x() -= ext.start();
            pos_.y() -= ext.top();
            size_.width(size_.width() + ext.hori());
            size_.height(size_.height() + ext.vert());

            if (size_.width() < 0) {
                size_.width(0);
            }
            if (size_.height() < 0) {
                size_.height(0);
            }
            return *this;
        }
        RectT& extend(Ty x, Ty y, Ty r, Ty b) {
            pos_.x() -= x;
            pos_.y() -= y;
            size_.width(size_.width() + x + r);
            size_.height(size_.height() + y + b);

            if (size_.width() < 0) {
                size_.width(0);
            }
            if (size_.height() < 0) {
                size_.height(0);
            }
            return *this;
        }
        RectT& offset(Ty dx, Ty dy) {
            pos_.x() += dx;
            pos_.y() += dy;
            return *this;
        }

    private:
        PointT<Ty> pos_;
        SizeT<Ty> size_;
    };

    using Rect = RectT<int>;
    using RectF = RectT<float>;
    using RectD = RectT<double>;

}

#endif  // UKIVE_GRAPHICS_RECT_HPP_
