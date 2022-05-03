// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "view_ref.h"

#include "ukive/views/view.h"


namespace ukive {

    ViewRef::ViewRef()
        : view_(nullptr) {}

    ViewRef::ViewRef(View* v)
        : view_(v)
    {
        if (v) {
            token_ = v->getCanary();
        }
    }

    ViewRef& ViewRef::operator=(View* v) {
        view_ = v;
        if (v) {
            token_ = v->getCanary();
        } else {
            token_.reset();
        }
        return *this;
    }

    bool ViewRef::operator==(const ViewRef& rhs) const {
        return equal(rhs);
    }

    bool ViewRef::operator!=(const ViewRef& rhs) const {
        return !equal(rhs);
    }

    ViewRef::operator bool() const {
        return valid();
    }

    bool ViewRef::equal(const ViewRef& rhs) const {
        if (token_.expired() != rhs.token_.expired()) {
            return false;
        }
        if (!token_.expired()) {
            if (view_ != rhs.view_) {
                return false;
            }
        }
        return true;
    }

    bool ViewRef::valid() const {
        return !token_.expired() && view_;
    }

    View* ViewRef::operator->() const {
        return get();
    }

    View* ViewRef::get() const {
        if (token_.expired()) {
            return nullptr;
        }
        return view_;
    }

}
