// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_TEXT_INLINE_OBJECT_H_
#define UKIVE_TEXT_TEXT_INLINE_OBJECT_H_


namespace ukive {

    class Canvas;

    class TextInlineObject {
    public:
        virtual ~TextInlineObject() = default;

        // TODO: 可能需要更多的上下文信息
        virtual void onDrawInlineObject(Canvas* c, float x, float y) = 0;
        virtual void onGetMetrics(float* width, float* height, float* baseline) = 0;
    };

}

#endif  // UKIVE_TEXT_TEXT_INLINE_OBJECT_H_