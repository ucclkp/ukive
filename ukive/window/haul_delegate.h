// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_HAUL_DELEGATE_H_
#define UKIVE_WINDOW_HAUL_DELEGATE_H_


namespace ukive {

    class InputEvent;
    class HaulSource;

    class HaulDelegate {
    public:
        virtual ~HaulDelegate() = default;

        virtual bool canHaul(HaulSource* src) const { return true; }
        virtual bool continueHaul(HaulSource* src, InputEvent* e) const { return true; }

        virtual void onHaulStarted(HaulSource* src) {}
        virtual void onHauling(HaulSource* src, InputEvent* e) {}
        virtual void onHaulStopped(HaulSource* src) {}
        virtual void onHaulCancelled(HaulSource* src) {}
    };

}

#endif  // UKIVE_WINDOW_HAUL_DELEGATE_H_