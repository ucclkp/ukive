// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_VIEW_REF_H_
#define UKIVE_VIEWS_VIEW_REF_H_

#include <memory>


namespace ukive {

    class View;

    class ViewRef {
    public:
        ViewRef();
        explicit ViewRef(View* v);

        ViewRef& operator=(View* v);

        bool operator==(const ViewRef& rhs) const;
        bool operator!=(const ViewRef& rhs) const;

        explicit operator bool() const;

        bool equal(const ViewRef& rhs) const;
        bool valid() const;

        View* operator->() const;

        View* get() const;

    private:
        View* view_;
        std::weak_ptr<void> token_;
    };

}

#endif  // UKIVE_VIEWS_VIEW_REF_H_