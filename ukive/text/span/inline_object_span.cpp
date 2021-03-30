// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "inline_object_span.h"


namespace ukive {

    InlineObjectSpan::InlineObjectSpan(size_t start, size_t end)
        : Span(start, end) {
    }

    InlineObjectSpan::~InlineObjectSpan() {
    }

    Span::Type InlineObjectSpan::getType() const {
        return INLINE_OBJECT;
    }

}