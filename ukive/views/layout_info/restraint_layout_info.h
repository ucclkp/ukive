// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_INFO_RESTRAINT_LAYOUT_INFO_H_
#define UKIVE_VIEWS_LAYOUT_INFO_RESTRAINT_LAYOUT_INFO_H_

#include "ukive/views/layout_info/layout_info.h"
#include "ukive/views/size_info.h"


namespace ukive {

    class RestraintLayoutInfo : public LayoutInfo {
    public:
        enum Edge {
            START = 0,
            TOP,
            END,
            BOTTOM,
            NONE
        };

        enum CoupleHandleType {
            CH_FILL = 0,
            CH_WRAP = 1,
            CH_FIXED = 2,
        };

        RestraintLayoutInfo();
        ~RestraintLayoutInfo();

        // 保存在一次测量过程中与该 LayoutInfo 绑定的 View
        // 的测量信息。注意记得在测量过程开始前清除该标记。
        SizeInfo::Value width_info;
        SizeInfo::Value height_info;
        bool is_width_determined = false;
        bool is_height_determined = false;

        // 保存在一次布局过程中与该 LayoutInfo 绑定的 View
        // 的布局信息。注意记得在布局过程开始前清除该标记。
        int left, right;
        int top, bottom;
        bool is_vert_layouted = false;
        bool is_hori_layouted = false;

        int vert_weight = 0;
        int hori_weight = 0;

        float vert_percent = .5f;
        float hori_percent = .5f;

        CoupleHandleType hori_couple_handler_type;
        CoupleHandleType vert_couple_handler_type;

        int start_handle_id;
        Edge start_handle_edge = NONE;
        int top_handle_id;
        Edge top_handle_edge = NONE;
        int end_handle_id;
        Edge end_handle_edge = NONE;
        int bottom_handle_id;
        Edge bottom_handle_edge = NONE;

        RestraintLayoutInfo* startHandle(int handle_id, Edge handle_edge);
        RestraintLayoutInfo* topHandle(int handle_id, Edge handle_edge);
        RestraintLayoutInfo* endHandle(int handle_id, Edge handle_edge);
        RestraintLayoutInfo* bottomHandle(int handle_id, Edge handle_edge);

    public:
        class Builder {
        public:
            Builder()
                : li_(new RestraintLayoutInfo()) {}
            ~Builder() { if (!built_) delete li_; }

            Builder& start(int handle_id, Edge handle_edge = START) {
                li_->startHandle(handle_id, handle_edge);
                return *this;
            }

            Builder& top(int handle_id, Edge handle_edge = TOP) {
                li_->topHandle(handle_id, handle_edge);
                return *this;
            }

            Builder& end(int handle_id, Edge handle_edge = END) {
                li_->endHandle(handle_id, handle_edge);
                return *this;
            }

            Builder& bottom(int handle_id, Edge handle_edge = BOTTOM) {
                li_->bottomHandle(handle_id, handle_edge);
                return *this;
            }

            RestraintLayoutInfo* build() {
                built_ = true;
                return li_;
            }

        private:
            bool built_ = false;
            RestraintLayoutInfo* li_;
        };

        bool hasStart();
        bool hasTop();
        bool hasEnd();
        bool hasBottom();

        bool hasVertWeight();
        bool hasHoriWeight();

        bool hasVertCouple();
        bool hasHoriCouple();
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_INFO_RESTRAINT_LAYOUT_INFO_H_