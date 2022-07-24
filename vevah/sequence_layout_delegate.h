// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef VEVAH_SEQUENCE_INPUT_DELEGATE_H_
#define VEVAH_SEQUENCE_INPUT_DELEGATE_H_

#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"
#include "ukive/views/view_delegate.h"


namespace ukive {
    class SequenceLayout;
}

namespace vevah {

    class OnViewSelectedListener;

    class SequenceLayoutDelegate :
        public ukive::ViewDelegate
    {
    public:
        explicit SequenceLayoutDelegate(OnViewSelectedListener* l);

        bool onHookInputReceived(
            ukive::LayoutView* v, ukive::InputEvent* e, bool* ret) override;
        bool onInputReceived(
            ukive::View* v, ukive::InputEvent* e, bool* ret) override;
        void onDrawOverChildrenReceived(
            ukive::View* v, ukive::Canvas* c) override;

    private:
        enum class PlaceType {
            None,
            Next,
            Insert,
        };

        struct Placement {
            PlaceType type = PlaceType::None;
            ukive::Rect place;
            size_t insert_pos = 0;
        };

        struct Selection {
            bool valid = false;
            size_t pos = 0;
        };

        void determinePlacement(
            ukive::SequenceLayout* v, const ukive::Point& p);

        Selection sel_;
        Placement placement_;
        OnViewSelectedListener* listener_ = nullptr;
    };

}

#endif  // VEVAH_SEQUENCE_INPUT_DELEGATE_H_