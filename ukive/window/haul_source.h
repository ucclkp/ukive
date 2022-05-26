// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_HAUL_SOURCE_H_
#define UKIVE_WINDOW_HAUL_SOURCE_H_

#include "ukive/graphics/point.hpp"


namespace ukive {

    class View;
    class Window;
    class InputEvent;
    class HaulDelegate;

    class HaulSource {
    public:
        HaulSource(int id, HaulDelegate* d);
        virtual ~HaulSource() = default;

        void ignite(View* v, InputEvent* e);
        bool brake(InputEvent* e);
        void cancel();

        int getId() const;

    private:
        void stop();

        int id_;
        HaulDelegate* delegate_;
        Window* window_ = nullptr;

        Point start_pos_{};
        bool is_hauling_ = false;
    };

}

#endif  // UKIVE_WINDOW_HAUL_SOURCE_H_