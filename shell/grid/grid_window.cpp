// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "grid_window.h"

#include "ukive/diagnostic/grid_view.h"


namespace {

    struct Data {
        uint32_t x;
        uint32_t y;
        ukive::Color color;
    };

}

namespace shell {

    void GridWindow::onCreated() {
        super::onCreated();

        showTitleBar();

        int padding = getContext().dp2pxi(0);

        grid_view_ = new ukive::GridView(getContext());
        grid_view_->setFlippedY(true);
        grid_view_->setPadding(padding, padding, padding, padding);
        setContentView(grid_view_);

        grid_view_->addPixel(10, 10, ukive::Color::Yellow100);
        grid_view_->addPixel(11, 11, ukive::Color::Yellow100);

        //grid_view_->addLine({ 1, 1 }, { 3, 3 }, ukive::Color::Yellow700);
        grid_view_->addQBezierLine({ 2, 2 }, { 10, 10 }, { 15, 0 }, ukive::Color::Yellow700);

        grid_view_->addPoint(2, 2, getContext().dp2pxi(3), ukive::Color::Pink400, u"p1", ukive::Color::Black);
        grid_view_->addPoint(10, 10, getContext().dp2pxi(3), ukive::Color::Pink400, u"p2", ukive::Color::Black);
        grid_view_->addPoint(15, 0, getContext().dp2pxi(3), ukive::Color::Pink400, u"p3", ukive::Color::Black);
    }

    bool GridWindow::onDataCopy(unsigned id, unsigned size, void* data) {
        if (id == 0 && size == sizeof(Data)) {
            auto info = static_cast<const Data*>(data);
            grid_view_->addPixel(info->x, info->y, info->color);
            return true;
        }
        return false;
    }

}
