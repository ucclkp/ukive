// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "path_win.h"

#include "utils/log.h"

#include "ukive/app/application.h"


namespace ukive {

    PathWin::PathWin() {}

    bool PathWin::open() {
        if (!path_geo_) {
            HRESULT hr = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->getD2DFactory()
                ->CreatePathGeometry(&path_geo_);
            if (FAILED(hr)) {
                ubassert(false);
                return false;
            }
        }

        if (sink_) {
            ubassert(false);
            return false;
        }

        HRESULT hr = path_geo_->Open(&sink_);
        if (FAILED(hr)) {
            ubassert(false);
            return false;
        }

        return true;
    }

    void PathWin::begin(const PointF& start, bool fill) {
        ubassert(sink_);
        sink_->BeginFigure(
            D2D1::Point2F(start.x, start.y),
            fill ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW);
    }

    void PathWin::end(bool close) {
        ubassert(sink_);
        sink_->EndFigure(close ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
    }

    bool PathWin::close() {
        if (!sink_) {
            ubassert(false);
            return false;
        }

        HRESULT hr = sink_->Close();
        if (FAILED(hr)) {
            ubassert(false);
            return false;
        }

        return true;
    }

    void PathWin::addLine(const PointF& p) {
        ubassert(sink_);
        sink_->AddLine(D2D1::Point2F(p.x, p.y));
    }

    void PathWin::addQBezier(const PointF& p1, const PointF& p2) {
        ubassert(sink_);
        sink_->AddQuadraticBezier(
            D2D1::QuadraticBezierSegment(
                D2D1::Point2F(p1.x, p1.y), D2D1::Point2F(p2.x, p2.y)));
    }

}
