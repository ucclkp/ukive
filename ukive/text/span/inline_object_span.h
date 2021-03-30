// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_SPAN_INLINE_OBJECT_SPAN_H_
#define UKIVE_TEXT_SPAN_INLINE_OBJECT_SPAN_H_

#include "ukive/text/span/span.h"
#include "ukive/text/text_inline_object.h"


namespace ukive {

    class InlineObjectSpan :
        public Span, public TextInlineObject
    {
    public:
        InlineObjectSpan(size_t start, size_t end);
        ~InlineObjectSpan();

        // Span
        Type getType() const override;

        // TextInlineObject
        void onDrawInlineObject(Canvas* c, float x, float y) override {}
        void onGetMetrics(float* width, float* height, float* baseline) override {}
    };

}

#endif  // UKIVE_TEXT_SPAN_INLINE_OBJECT_SPAN_H_