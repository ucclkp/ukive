// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPTR_HPP_
#define UKIVE_GRAPHICS_GPTR_HPP_

#include <cstddef>

#include "ukive/graphics/gerror_code.hpp"


namespace ukive {

    template <class Ty>
    class GPtr {
    public:
        GPtr()
            : ptr_(nullptr) {}

        explicit GPtr(Ty* real)
            : ptr_(real) {}

        GPtr(const GPtr& rhs) {
            if (rhs.ptr_) {
                rhs.ptr_->add_ref();
            }
            ptr_ = rhs.ptr_;
        }

        ~GPtr() {
            reset();
        }

        GPtr& operator=(const GPtr& rhs) {
            // 防止自身给自身赋值。
            if (this == &rhs) {
                return *this;
            }

            if (rhs.ptr_) {
                rhs.ptr_->add_ref();
            }

            if (ptr_) {
                ptr_->sub_ref();
            }

            ptr_ = rhs.ptr_;
            return *this;
        }

        GPtr& operator=(Ty* real) {
            if (real == ptr_) {
                return *this;
            }

            if (ptr_) {
                ptr_->sub_ref();
            }

            ptr_ = real;
            return *this;
        }

        Ty* operator->() const {
            return ptr_;
        }

        Ty** operator&() {
            return &ptr_;
        }

        bool operator==(std::nullptr_t) const {
            return ptr_ == nullptr;
        }

        bool operator==(Ty* rhs) const {
            return ptr_ == rhs;
        }

        bool operator==(const GPtr<Ty>& rhs) const {
            return ptr_ == rhs.ptr_;
        }

        bool operator!=(std::nullptr_t) const {
            return ptr_ != nullptr;
        }

        bool operator!=(Ty* rhs) const {
            return ptr_ != rhs;
        }

        bool operator!=(const GPtr<Ty>& rhs) const {
            return ptr_ != rhs.ptr_;
        }

        explicit operator bool() const {
            return ptr_ != nullptr;
        }

        Ty* get() const {
            return ptr_;
        }

        template<class Ct>
        GPtr<Ct> cast() const {
            ptr_->add_ref();
            return GPtr<Ct>(static_cast<Ct*>(ptr_));
        }

        Ty* detach() {
            auto tmp = ptr_;
            ptr_ = nullptr;
            return tmp;
        }

        void reset() {
            if (ptr_) {
                ptr_->sub_ref();
                ptr_ = nullptr;
            }
        }

    private:
        Ty* ptr_;
    };

    template <class Ty>
    class GEcPtr : public GPtr<Ty> {
    public:
        using GPtr<Ty>::operator=;

        gerc code;
    };

}

#endif  // UKIVE_GRAPHICS_GPTR_HPP_