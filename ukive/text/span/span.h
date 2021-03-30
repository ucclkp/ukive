// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_SPAN_SPAN_H_
#define UKIVE_TEXT_SPAN_SPAN_H_

#include <cstddef>


namespace ukive {

    class Span {
    public:
        Span(size_t start, size_t end);
        virtual ~Span() = default;

        enum Type {
            NONE = 0,
            FONT_ATTRIBUTES = 1,
            DRAWING_EFFECTS = 2,
            INLINE_OBJECT = 3,
            INTERACTABLE = 4,
        };

        virtual Type getType() const = 0;

        void resize(size_t start, size_t end);

        size_t getStart() const;
        size_t getEnd() const;

    private:
        size_t start_;
        size_t end_;
    };

}

#endif  // UKIVE_TEXT_SPAN_SPAN_H_