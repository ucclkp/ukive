// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GREF_COUNT_H_
#define UKIVE_GRAPHICS_GREF_COUNT_H_


namespace ukive {

    class GRefCount {
    public:
        virtual ~GRefCount() = default;

        virtual void add_ref() = 0;
        virtual void sub_ref() = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GREF_COUNT_H_