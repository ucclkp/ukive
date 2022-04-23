// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_NATIVE_RT_H_
#define UKIVE_GRAPHICS_NATIVE_RT_H_


namespace ukive {

    class NativeRT {
    public:
        virtual ~NativeRT() = default;

        virtual void destroy() = 0;
    };

}

#endif  // UKIVE_GRAPHICS_NATIVE_RT_H_