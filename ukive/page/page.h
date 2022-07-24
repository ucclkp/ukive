// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_PAGE_PAGE_H_
#define UKIVE_PAGE_PAGE_H_

#include "ukive/views/view.h"


namespace ukive {

    class Window;

    class Page {
    public:
        virtual ~Page() = default;

        View* create(Context c);
        void initialize();
        void show(bool show);
        void destroy();

        template <typename Ty>
        Ty* findView(View* v, int id) const {
            return v ? static_cast<Ty*>(v->findView(id)) : nullptr;
        }
        template <typename Ty>
        Ty* findView(int id) const {
            return findView<Ty>(content_view_, id);
        }

        bool isCreated() const;
        bool isShowing() const;
        Window* getWindow() const;
        View* getContentView() const;

    protected:
        Page();

        virtual View* onCreate(Context c) = 0;
        virtual void onCreated(View* v) {}
        virtual void onShow(bool show) {}
        virtual void onDestroy() {}

    private:
        View* content_view_ = nullptr;

        bool is_created_ = false;
        bool is_showing_ = false;
    };

}

#endif  // UKIVE_PAGE_PAGE_H_
