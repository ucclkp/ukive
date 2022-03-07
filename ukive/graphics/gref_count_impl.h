// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GREF_COUNT_IMPL_H_
#define UKIVE_GRAPHICS_GREF_COUNT_IMPL_H_

#include <atomic>

#include "ukive/graphics/gref_count.h"


namespace ukive {

    class GRefCountImpl : public virtual GRefCount {
    public:
        GRefCountImpl()
            : ref_count_(1) {}
        virtual ~GRefCountImpl() = default;

        void add_ref() override {
            ++ref_count_;
        }

        void sub_ref() override {
            auto rc = --ref_count_;
            if (rc == 0) {
                delete this;
            }
        }

    private:
        std::atomic_ulong ref_count_;
    };

}

#endif  // UKIVE_GRAPHICS_GREF_COUNT_IMPL_H_