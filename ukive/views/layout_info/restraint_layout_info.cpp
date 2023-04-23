// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "restraint_layout_info.h"


namespace ukive {

    RestraintLayoutInfo::RestraintLayoutInfo() {}

    RestraintLayoutInfo::~RestraintLayoutInfo() {}

    RestraintLayoutInfo* RestraintLayoutInfo::startHandle(long long handle_id, Edge handle_edge) {
        start_handle_id = handle_id;
        start_handle_edge = handle_edge;
        return this;
    }

    RestraintLayoutInfo* RestraintLayoutInfo::topHandle(long long handle_id, Edge handle_edge) {
        top_handle_id = handle_id;
        top_handle_edge = handle_edge;
        return this;
    }

    RestraintLayoutInfo* RestraintLayoutInfo::endHandle(long long handle_id, Edge handle_edge) {
        end_handle_id = handle_id;
        end_handle_edge = handle_edge;
        return this;
    }

    RestraintLayoutInfo* RestraintLayoutInfo::bottomHandle(long long handle_id, Edge handle_edge) {
        bottom_handle_id = handle_id;
        bottom_handle_edge = handle_edge;
        return this;
    }

    bool RestraintLayoutInfo::hasStart() {
        return start_handle_edge != NONE;
    }

    bool RestraintLayoutInfo::hasTop() {
        return top_handle_edge != NONE;
    }

    bool RestraintLayoutInfo::hasEnd() {
        return end_handle_edge != NONE;
    }

    bool RestraintLayoutInfo::hasBottom() {
        return bottom_handle_edge != NONE;
    }

    bool RestraintLayoutInfo::hasVertWeight() {
        return vert_weight != 0;
    }

    bool RestraintLayoutInfo::hasHoriWeight() {
        return hori_weight != 0;
    }

    bool RestraintLayoutInfo::hasVertCouple() {
        return (top_handle_edge != NONE
            && bottom_handle_edge != NONE);
    }

    bool RestraintLayoutInfo::hasHoriCouple() {
        return (start_handle_edge != NONE
            && end_handle_edge != NONE);
    }

}