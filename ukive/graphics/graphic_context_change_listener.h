// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GRAPHIC_CONTEXT_CHANGE_LISTENER_H_
#define UKIVE_GRAPHICS_GRAPHIC_CONTEXT_CHANGE_LISTENER_H_


namespace ukive {

    class GraphicContextChangeListener {
    public:
        virtual ~GraphicContextChangeListener() = default;

        virtual void onGraphDeviceLost() = 0;
        virtual void onGraphDeviceRestored() = 0;

        virtual void onHDRChanged(bool hdr) = 0;
        virtual void onDPIChanged(float dpi_x, float dpi_y) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GRAPHIC_CONTEXT_CHANGE_LISTENER_H_