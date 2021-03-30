// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "interactable_span.h"


namespace ukive {

    InteractableSpan::InteractableSpan(size_t start, size_t end)
        : Span(start, end) {
    }

    InteractableSpan::~InteractableSpan() {
    }

    Span::Type InteractableSpan::getType() const {
        return INTERACTABLE;
    }

}