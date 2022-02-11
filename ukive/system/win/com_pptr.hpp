// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_COMPPTR_HPP_
#define UKIVE_SYSTEM_WIN_COMPPTR_HPP_

#include <cassert>
#include <cstddef>

#include "com_ptr.hpp"


namespace ukive {
namespace win {

    template <class Ty>
    class ComPPtr {
    public:
        ComPPtr()
            : pptr_(nullptr),
              count_(0) {}

        explicit ComPPtr(Ty** real, size_t count)
            : pptr_(real),
              count_(count) {}

        ComPPtr(const ComPPtr& rhs) {
            rhs.AddRef();
            pptr_ = rhs.pptr_;
            count_ = rhs.count_;
        }

        ~ComPPtr() {
            reset();
        }

        ComPPtr& operator=(const ComPPtr& rhs) {
            // 防止自身给自身赋值。
            if (this == &rhs) {
                return *this;
            }

            rhs.AddRef();
            Release();

            pptr_ = rhs.pptr_;
            count_ = rhs.count_;
            return *this;
        }

        ComPPtr& operator=(Ty** real, size_t count) {
            if (real == pptr_) {
                return *this;
            }

            Release();

            pptr_ = real;
            count_ = count;
            return *this;
        }

        Ty*** operator&() {
            assert(!pptr_ && count_ == 0);
            return &pptr_;
        }

        bool operator==(std::nullptr_t) const {
            return pptr_ == nullptr;
        }

        bool operator==(Ty** rhs) const {
            return pptr_ == rhs;
        }

        bool operator==(const ComPPtr<Ty>& rhs) const {
            return pptr_ == rhs.pptr_;
        }

        bool operator!=(std::nullptr_t) const {
            return pptr_ != nullptr;
        }

        bool operator!=(Ty** rhs) const {
            return pptr_ != rhs;
        }

        bool operator!=(const ComPPtr<Ty>& rhs) const {
            return pptr_ != rhs.pptr_;
        }

        explicit operator bool() const {
            return pptr_ != nullptr;
        }

        Ty* operator[](size_t index) const {
            assert(index < count_);
            return pptr_[index];
        }

        ComPtr<Ty> operator()(size_t index) const {
            assert(index < count_);
            return ComPtr<Ty>(pptr_[index]);
        }

        Ty** get() const {
            return pptr_;
        }

        size_t size() const {
            return count_;
        }

        Ty** detach() {
            auto tmp = pptr_;
            pptr_ = nullptr;
            count_ = 0;
            return tmp;
        }

        void reset() {
            Release();
            pptr_ = nullptr;
            count_ = 0;
        }

    private:
        void AddRef() {
            if (pptr_) {
                for (size_t i = 0; i < count_; ++i) {
                    pptr_[i]->AddRef();
                }
            }
        }

        void Release() {
            if (pptr_) {
                for (size_t i = 0; i < count_; ++i) {
                    pptr_[i]->Release();
                }
            }
        }

        Ty** pptr_;
        size_t count_;
    };

}
}

#endif  // UKIVE_SYSTEM_WIN_COMPPTR_HPP_