// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_COMPTR_HPP_
#define UKIVE_SYSTEM_WIN_COMPTR_HPP_

#include <cstddef>


namespace ukive {
namespace win {

    template <class Ty>
    class ComPtr {
    public:
        ComPtr()
            : ptr_(nullptr) {}

        explicit ComPtr(Ty* real)
            : ptr_(real) {}

        ComPtr(const ComPtr& rhs) {
            if (rhs.ptr_) {
                rhs.ptr_->AddRef();
            }
            ptr_ = rhs.ptr_;
        }

        ~ComPtr() {
            reset();
        }

        ComPtr& operator=(const ComPtr& rhs) {
            // 防止自身给自身赋值。
            if (this == &rhs) {
                return *this;
            }

            if (rhs.ptr_) {
                rhs.ptr_->AddRef();
            }

            if (ptr_) {
                ptr_->Release();
            }

            ptr_ = rhs.ptr_;
            return *this;
        }

        ComPtr& operator=(Ty* real) {
            if (real == ptr_) {
                return *this;
            }

            if (ptr_) {
                ptr_->Release();
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

        bool operator==(const ComPtr<Ty>& rhs) const {
            return ptr_ == rhs.ptr_;
        }

        bool operator!=(std::nullptr_t) const {
            return ptr_ != nullptr;
        }

        bool operator!=(Ty* rhs) const {
            return ptr_ != rhs;
        }

        bool operator!=(const ComPtr<Ty>& rhs) const {
            return ptr_ != rhs.ptr_;
        }

        explicit operator bool() const {
            return ptr_ != nullptr;
        }

        Ty* get() const {
            return ptr_;
        }

        template<class Ct>
        ComPtr<Ct> cast() const {
            Ct* casted = nullptr;
            ptr_->template QueryInterface<Ct>(&casted);
            return ComPtr<Ct>(casted);
        }

        Ty* detach() {
            auto tmp = ptr_;
            ptr_ = nullptr;
            return tmp;
        }

        void reset() {
            if (ptr_) {
                ptr_->Release();
                ptr_ = nullptr;
            }
        }

    private:
        Ty* ptr_;
    };

}
}

#endif  // UKIVE_SYSTEM_WIN_COMPTR_HPP_