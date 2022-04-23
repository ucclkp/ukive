// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_PATH_WIN_H_
#define UKIVE_GRAPHICS_WIN_PATH_WIN_H_

#include "ukive/graphics/path.h"

#include "utils/memory/win/com_ptr.hpp"

#include <d2d1.h>


namespace ukive {
namespace win {

    class PathWin : public Path {
    public:
        PathWin();

        bool open() override;
        void begin(const PointF& start, bool fill) override;
        void end(bool close) override;
        bool close() override;

        void addLine(const PointF& p) override;
        void addQBezier(const PointF& p1, const PointF& p2) override;

        ID2D1PathGeometry* getNative() const { return path_geo_.get(); }

    private:
        utl::win::ComPtr<ID2D1PathGeometry> path_geo_;
        utl::win::ComPtr<ID2D1GeometrySink> sink_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_PATH_WIN_H_