// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "span.h"


namespace ukive {

    Span::Span(size_t start, size_t end)
        : start_(start), end_(end) {}

    void Span::resize(size_t start, size_t end) {
        start_ = start;
        end_ = end;
    }

    size_t Span::getStart() const {
        return start_;
    }

    size_t Span::getEnd() const {
        return end_;
    }

}