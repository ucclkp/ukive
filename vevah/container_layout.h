// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef VEVAH_CONTAINER_LAYOUT_H_
#define VEVAH_CONTAINER_LAYOUT_H_

#include "ukive/views/layout/simple_layout.h"


namespace ukive {
    class ViewDelegate;
}

namespace vevah {

    class ContainerLayout :
        public ukive::SimpleLayout
    {
    public:
        explicit ContainerLayout(ukive::Context c);
        ContainerLayout(ukive::Context c, ukive::AttrsRef attrs);

    protected:
        ukive::Size onDetermineSize(const ukive::SizeInfo& info) override;
        void onLayout(
            const ukive::Rect& new_bounds,
            const ukive::Rect& old_bounds) override;
        void onDrawOverChildren(ukive::Canvas* canvas) override;

        bool onHookInputEvent(ukive::InputEvent* e) override;
        bool onInputEvent(ukive::InputEvent* e) override;

    private:
        using super = SimpleLayout;

        enum class Mode {
            None,
            View,
            Layout,
        };

        Mode mode_ = Mode::None;
        bool is_haul_in_ = false;
        bool is_view_selected_ = false;
        std::unique_ptr<ukive::ViewDelegate> vd_;
    };

}

#endif  // VEVAH_CONTAINER_LAYOUT_H_