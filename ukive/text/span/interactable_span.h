// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_SPAN_INTERACTABLE_SPAN_H_
#define UKIVE_TEXT_SPAN_INTERACTABLE_SPAN_H_

#include "ukive/text/span/span.h"
#include "ukive/event/input_event.h"


namespace ukive {

    class InteractableSpan : public Span {
    public:
        InteractableSpan(size_t start, size_t end);
        ~InteractableSpan();

        Type getType() const override;

        virtual bool onInputEvent(InputEvent* e) { return false; }
    };

}

#endif  // UKIVE_TEXT_SPAN_INTERACTABLE_SPAN_H_