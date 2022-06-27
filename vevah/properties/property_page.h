// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef VEVAH_PROPERTY_PAGE_H_
#define VEVAH_PROPERTY_PAGE_H_

#include "ukive/page/page.h"


namespace vevah {

    class PropertyPage :
        public ukive::Page
    {
    public:
        PropertyPage();

        // ukive::Page
        ukive::View* onCreate(ukive::LayoutView* parent) override;
        void onCreated(ukive::View* v) override;
        void onShow(bool show) override;
        void onDestroy() override;
    };

}

#endif  // VEVAH_PROPERTY_PAGE_H_