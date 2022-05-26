// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_CANVAS_STACK_HPP_
#define UKIVE_GRAPHICS_CANVAS_STACK_HPP_

#include <cassert>
#include <cstddef>
#include <algorithm>


namespace ukive {

    template <typename Ty>
    class CanvasStack {
    public:
        CanvasStack() noexcept
            : dat_(nullptr),
              cur_(0),
              tal_(0) {}

        explicit CanvasStack(size_t pre)
            : cur_(0),
              tal_(pre)
        {
            if (pre) {
                dat_ = new Ty[pre];
            } else {
                dat_ = nullptr;
            }
        }

        CanvasStack(const CanvasStack& rhs) noexcept
            : cur_(rhs.cur_),
              tal_(rhs.tal_)
        {
            if (tal_) {
                dat_ = new Ty[tal_];
            } else {
                dat_ = nullptr;
            }

            if (rhs.dat_) {
                std::copy(rhs.dat_, rhs.dat_ + cur_, dat_);
            }
        }

        CanvasStack(CanvasStack&& rhs) noexcept
            : dat_(rhs.dat_),
              cur_(rhs.cur_),
              tal_(rhs.tal_)
        {
            rhs.dat_ = nullptr;
            rhs.cur_ = 0;
            rhs.tal_ = 0;
        }

        ~CanvasStack() {
            delete[] dat_;
        }

        CanvasStack& operator=(const CanvasStack& rhs) {
            delete[] dat_;
            cur_ = rhs.cur_;
            tal_ = rhs.tal_;

            if (tal_) {
                dat_ = new Ty[tal_];
            } else {
                dat_ = nullptr;
            }

            if (rhs.dat_) {
                std::copy(rhs.dat_, rhs.dat_ + cur_, dat_);
            }
            return *this;
        }

        CanvasStack& operator=(CanvasStack&& rhs) noexcept {
            delete[] dat_;
            dat_ = rhs.dat_;
            cur_ = rhs.cur_;
            tal_ = rhs.tal_;
            return *this;
        }

        Ty& push() {
            assert(cur_ < tal_);
            ++cur_;
            return dat_[cur_ - 1];
        }

        void push(const Ty& val) {
            if (cur_ + 1 > tal_) {
                expand(cur_ + cur_ / 2 + 1);
            }
            dat_[cur_] = val;
            ++cur_;
        }

        void push(Ty&& val) {
            if (cur_ + 1 > tal_) {
                expand(cur_ + cur_ / 2 + 1);
            }
            dat_[cur_] = std::move(val);
            ++cur_;
        }

        void pop() noexcept {
            --cur_;
        }

        Ty& top() {
            return dat_[cur_ - 1];
        }

        const Ty& top() const {
            return dat_[cur_ - 1];
        }

        void clear() noexcept {
            cur_ = 0;
        }

        void shrink() {
            delete[] dat_;
            dat_ = nullptr;
            cur_ = 0;
            tal_ = 0;
        }

        void reserve(size_t cnt) {
            if (cnt > tal_) {
                expand(cnt);
            }
        }

        void resize(size_t new_size) {
            if (new_size > tal_) {
                expand(new_size);
            } else {
                cur_ = new_size;
            }
        }

        size_t capacity() const noexcept {
            return tal_;
        }

        size_t size() const noexcept {
            return cur_;
        }

        bool has_space() const noexcept {
            return cur_ < tal_;
        }

        bool empty() const noexcept {
            return cur_ == 0;
        }

    private:
        void expand(size_t new_size) {
            Ty* tmp = new Ty[new_size];
            if (dat_) {
                std::copy(dat_, dat_ + cur_, tmp);
                delete[] dat_;
            }
            dat_ = tmp;
            tal_ = new_size;
        }

        Ty* dat_;
        size_t cur_;
        size_t tal_;
    };

}

#endif  // UKIVE_GRAPHICS_CANVAS_STACK_HPP_