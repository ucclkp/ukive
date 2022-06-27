// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "rebuildable.h"

#include "ukive/app/application.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    Rebuildable::Rebuildable()
        : enabled_(true)
    {
        sync_ = Application::getGraphicDeviceManager()->getRbSync();

        std::lock_guard<std::recursive_mutex> lg(*sync_);
        auto head = Application::getGraphicDeviceManager()->getRbHead();
        if (*head) {
            (*head)->prev_ = this;
        }
        next_ = *head;
        *head = this;
    }

    Rebuildable::~Rebuildable() {
        std::lock_guard<std::recursive_mutex> lg(*sync_);
        auto head = Application::getGraphicDeviceManager()->getRbHead();
        if (prev_) {
            prev_->next_ = next_;
        } else {
            *head = next_;
        }
        if (next_) {
            next_->prev_ = prev_;
        }
    }

    void Rebuildable::demolish() {
        if (enabled_.load(std::memory_order_relaxed)) {
            onDemolish();
        }
    }

    void Rebuildable::rebuild(bool succeeded) {
        if (enabled_.load(std::memory_order_relaxed)) {
            onRebuild(succeeded);
        }
    }

    void Rebuildable::setRebuildEnabled(bool enable) {
        enabled_.store(enable, std::memory_order_relaxed);
    }

}
