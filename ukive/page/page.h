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
    class LayoutView;

    class Page {
    public:
        virtual ~Page() = default;

        View* create(LayoutView* parent);
        void initialize();
        void show(bool show);
        void destroy();

        template <typename T>
        T* findViewById(View* v, int id) const {
            return static_cast<T*>(v->findView(id));
        }

        bool isCreated() const;
        bool isShowing() const;
        Window* getWindow() const;
        View* getContentView() const;

    protected:
        explicit Page(Window* w);

        virtual View* onCreate(LayoutView* parent) = 0;
        virtual void onInitialize() {}
        virtual void onShow(bool show) {}
        virtual void onDestroy() {}

    private:
        Window* window_;
        View* content_view_ = nullptr;

        bool is_created_ = false;
        bool is_showing_ = false;
    };

}

#endif  // UKIVE_PAGE_PAGE_H_