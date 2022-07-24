// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/tab/tab_view.h"

#include "utils/log.h"

#include "ukive/page/page.h"
#include "ukive/views/tab/tab_strip_view.h"
#include "ukive/views/tab/tab_selection_listener.h"


namespace ukive {

    // TabView
    TabView::TabView(Context c)
        : TabView(c, {}) {}

    TabView::TabView(Context c, AttrsRef attrs)
        : SimpleLayout(c, attrs) {}

    TabView::~TabView() {
        for (auto it = pages_.begin(); it != pages_.end(); ++it) {
            (*it)->destroy();
            delete *it;
        }
    }

    void TabView::onTabStripSelectionChanged(size_t index) {
        setSelectedPage(index);
    }

    void TabView::setStripView(TabStripView* tsv) {
        if (tsv == strip_view_) {
            return;
        }

        if (strip_view_) {
            strip_view_->setSelectionListener(nullptr);
            strip_view_->clearItems();
        }

        strip_view_ = tsv;

        if (strip_view_) {
            strip_view_->setSelectionListener(this);
        }
    }

    void TabView::setSelectedPage(size_t index) {
        if (index >= getPageCount()) {
            return;
        }

        if (sel_index_ != index) {
            pages_[sel_index_]->show(false);
            sel_index_ = index;

            auto next_page = pages_[sel_index_];
            if (!next_page->isCreated()) {
                auto v = next_page->create(getContext());
                addViewByPageId(v, index);
                next_page->initialize();
            }
            next_page->show(true);

            if (strip_view_) {
                strip_view_->setSelectedItem(index);
            }
            if (listener_) {
                listener_->onTabSelectionChanged(sel_index_);
            }

            requestDraw();
        }
    }

    void TabView::setSelectionListener(TabSelectionListener* l) {
        listener_ = l;
    }

    size_t TabView::addPage(Page* p, std::u16string title) {
        pages_.push_back(p);

        size_t index = getPageCount() - 1;
        if (index == sel_index_) {
            View* v = p->create(getContext());
            addView(v);
            p->initialize();
            p->show(true);
        }

        if (strip_view_) {
            strip_view_->addItem(std::move(title));
        }
        return index;
    }

    size_t TabView::addPage(size_t index, Page* p, std::u16string title) {
        if (index > getPageCount()) {
            index = getPageCount();
        }
        pages_.insert(pages_.begin() + index, p);

        if (index == sel_index_) {
            View* v = p->create(getContext());
            addViewByPageId(v, index);
            p->initialize();
            p->show(true);
        }

        if (strip_view_) {
            strip_view_->addItem(index, std::move(title));
        }
        return index;
    }

    void TabView::removePage(size_t index) {
        if (index >= getPageCount()) {
            return;
        }
        auto page = pages_[index];
        pages_.erase(pages_.begin() + index);

        if (page->isCreated()) {
            page->destroy();
            delete page;

            auto v = getChildAt(index);
            removeView(v);
        }

        if (strip_view_) {
            strip_view_->removeItem(index);
        }
    }

    size_t TabView::getPageCount() const {
        return pages_.size();
    }

    void TabView::addViewByPageId(View* v, size_t page_id) {
        for (size_t i = page_id + 1; i < pages_.size(); ++i) {
            auto page = pages_[i];
            if (page->isCreated()) {
                for (size_t j = 0; j < getChildCount(); ++j) {
                    if (page->getContentView() == getChildAt(j)) {
                        addView(j, v);
                        return;
                    }
                }

                ubassert(false);
                return;
            }
        }
        addView(v);
    }

}
