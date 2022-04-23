// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_REBUILDABLE_H_
#define UKIVE_GRAPHICS_REBUILDABLE_H_

#include <atomic>
#include <mutex>


namespace ukive {

    class Rebuildable {
    public:
        Rebuildable();
        virtual ~Rebuildable();

        void demolish();
        void rebuild();

        void setRbEnabled(bool enable);
        bool isRbEnabled() const { return enabled_; }

        Rebuildable* getRbNext() { return next_; }

    protected:
        virtual void onDemolish() {}
        virtual void onRebuild() {}

    private:
        std::atomic_bool enabled_;
        std::recursive_mutex* sync_;
        Rebuildable* next_ = nullptr;
        Rebuildable* prev_ = nullptr;
    };

}

#endif  // UKIVE_GRAPHICS_REBUILDABLE_H_