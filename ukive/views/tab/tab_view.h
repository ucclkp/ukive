// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TAB_TAB_VIEW_H_
#define UKIVE_VIEWS_TAB_TAB_VIEW_H_

#include "ukive/views/layout/simple_layout.h"
#include "ukive/views/tab/tab_strip_selection_listener.h"


namespace ukive {

    class Page;
    class TabStripView;
    class TabSelectionListener;

    class TabView : public SimpleLayout, public TabStripSelectionListener {
    public:
        explicit TabView(Context c);
        TabView(Context c, AttrsRef attrs);
        ~TabView();

        // TabStripSelectionListener
        void onTabStripSelectionChanged(size_t index) override;

        void setStripView(TabStripView* tsv);
        void setSelectedPage(size_t index);
        void setSelectionListener(TabSelectionListener* l);

        size_t addPage(Page* p, std::u16string title);
        size_t addPage(size_t index, Page* p, std::u16string title);
        void removePage(size_t index);
        size_t getPageCount() const;

    private:
        void addViewByPageId(View* v, size_t page_id);

        size_t sel_index_ = 0;
        std::vector<Page*> pages_;

        TabStripView* strip_view_ = nullptr;
        TabSelectionListener* listener_ = nullptr;
    };

}

#endif