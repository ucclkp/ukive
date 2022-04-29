// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "size_info.h"

#include <algorithm>

#include "ukive/views/view.h"


namespace ukive {

    SizeInfo::SizeInfo() {}

    SizeInfo::SizeInfo(const Value& w, const Value& h)
        : width_(w), height_(h) {}

    bool SizeInfo::operator==(const SizeInfo& rhs) const {
        return width_ == rhs.width_ &&
            height_ == rhs.height_;
    }

    void SizeInfo::set(const Value& w, const Value& h) {
        width_ = w;
        height_ = h;
    }

    void SizeInfo::setWidth(const Value& w) {
        width_ = w;
    }

    void SizeInfo::setHeight(const Value& h) {
        height_ = h;
    }


    SizeInfo::Value SizeInfo::getChildSizeInfo(
        const Value& parent_val, int inset, int child_layout_size)
    {
        if (child_layout_size >= 0) {
            return Value(child_layout_size, DEFINED);
        }

        int size = (std::max)(0, parent_val.val - inset);
        Value result(size, CONTENT);

        switch (parent_val.mode) {
        case CONTENT:
            if (child_layout_size == View::LS_FILL) {
                result.mode = CONTENT;
            }
            else if (child_layout_size == View::LS_AUTO) {
                result.mode = CONTENT;
            }
            else if (child_layout_size == View::LS_FREE) {
                result.mode = FREEDOM;
            }
            break;

        case DEFINED:
            if (child_layout_size == View::LS_FILL) {
                result.mode = DEFINED;
            }
            else if (child_layout_size == View::LS_AUTO) {
                result.mode = CONTENT;
            }
            else if (child_layout_size == View::LS_FREE) {
                result.mode = FREEDOM;
            }
            break;

        case FREEDOM:
            result.mode = FREEDOM;
            break;

        default:
            break;
        }

        return result;
    }

}
