// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_EFFECTS_CYRO_EFFECT_H_
#define UKIVE_GRAPHICS_EFFECTS_CYRO_EFFECT_H_


namespace ukive {

    class Canvas;
    class Context;
    class ImageFrame;
    class OffscreenBuffer;

    class CyroEffect {
    public:
        virtual ~CyroEffect() = default;

        virtual bool initialize() = 0;
        virtual void destroy() = 0;

        /**
         * 生成可与指定 Canvas 兼容的效果缓存。
         */
        virtual bool generate(Canvas* c) = 0;

        /**
         * 绘制效果到指定 Canvas。如果当前未生成缓存，则生成之。
         */
        virtual bool draw(Canvas* c) = 0;

        virtual bool setContent(OffscreenBuffer* content) = 0;

        virtual ImageFrame* getOutput() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_EFFECTS_CYRO_EFFECT_H_