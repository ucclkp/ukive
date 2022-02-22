// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gravity.h"

#include <array>


namespace ukive {

    int adjustX(
        const Rect& root, const Rect& anchor, int target_max_width,
        int x, bool is_discretized, int* adj_gravity)
    {
        struct Info {
            int d;
            int x;
            int gravity;
        };

        if (x + target_max_width > root.right || x < root.left) {
            if (is_discretized) {
                auto mid_x = (std::max)((
                    anchor.left - (target_max_width - anchor.width()) / 2), root.left);
                auto mid_r_x = (std::max)(anchor.right - target_max_width, root.left);
                auto left_x = (std::max)(anchor.left - target_max_width, root.left);

                /**
                 * 事先计算好这几种离散的位置的容纳空间、y 值和 gravity。
                 * 最后一种情况用于稳定计算结果，防止 target 不断减小的过程中出现的跳变。
                 */
                Info i_END{       root.right - anchor.right, anchor.right, GV_END };
                Info i_MID_START{ root.right - anchor.left,  anchor.left,  GV_MID_START };
                Info i_MID_HORI{  root.right - mid_x,        mid_x,        GV_MID_HORI };
                Info i_MID_END{   anchor.right - root.left,  mid_r_x,      GV_MID_END };
                Info i_START{     anchor.left - root.left,   left_x,       GV_START };
                Info i_FULL{      root.right - root.left,    root.left,    GV_NONE };

                size_t space_c = 0, occup_c = 0;
                std::array<Info, 6> space;
                std::array<Info, 6> occup;
                std::array<Info, 6> is{ i_END, i_MID_START, i_MID_HORI, i_MID_END, i_START, i_FULL };
                for (const auto& i : is) {
                    if (i.d >= target_max_width) {
                        space[space_c++] = i;
                    } else {
                        occup[occup_c++] = i;
                    }
                }


                if (space_c) {
                    /**
                     * 存在能完全容纳 target 的项目。
                     * 找到一个对当前 x 值变动最小的一项。
                     */
                    auto it = std::min_element(
                        space.begin(), space.begin() + space_c,
                        [x](const Info& i1, const Info& i2) -> bool
                        { return std::abs(x - i1.x) < std::abs(x - i2.x); });
                    x = it->x;
                    if (it->gravity != GV_NONE) {
                        *adj_gravity = it->gravity;
                    }
                } else {
                    /**
                     * 不存在能完全容纳 target 的项目。
                     * 找到一个容纳空间最大的一项。
                     */
                    auto it = std::max_element(
                        occup.begin(), occup.begin() + occup_c,
                        [](const Info& i1, const Info& i2) -> bool
                        { return i1.d < i2.d; });
                    x = it->x;
                    if (it->gravity != GV_NONE) {
                        *adj_gravity = it->gravity;
                    }
                }
            } else {
                auto d1 = root.right - root.left;
                if (d1 >= target_max_width) {
                    if (x < root.left) {
                        x = root.left;
                    } else {
                        x = root.right - target_max_width;
                    }
                } else {
                    x = root.left;
                }
            }
        }

        return x;
    }

    int adjustY(
        const Rect& root, const Rect& anchor, int target_max_height,
        int y, bool is_discretized, int* adj_gravity)
    {
        struct Info {
            int d;
            int y;
            int gravity;
        };

        if (y + target_max_height > root.bottom || y < root.top) {
            if (is_discretized) {
                auto mid_y = (std::max)((
                    anchor.top - (target_max_height - anchor.height()) / 2), root.top);
                auto mid_b_y = (std::max)(anchor.bottom - target_max_height, root.top);
                auto top_y = (std::max)(anchor.top - target_max_height, root.top);

                Info i_BOTTOM{     root.bottom - anchor.bottom, anchor.bottom, GV_BOTTOM };
                Info i_MID_TOP{    root.bottom - anchor.top,    anchor.top,    GV_MID_TOP };
                Info i_MID_VERT{   root.bottom - mid_y,         mid_y,         GV_MID_VERT };
                Info i_MID_BOTTOM{ anchor.bottom - root.top,    mid_b_y,       GV_MID_BOTTOM };
                Info i_TOP{        anchor.top - root.top,       top_y,         GV_TOP };
                Info i_FULL{       root.bottom - root.top,      root.top,      GV_NONE };

                size_t space_c = 0, occup_c = 0;
                std::array<Info, 6> space;
                std::array<Info, 6> occup;
                std::array<Info, 6> is{ i_BOTTOM, i_MID_TOP, i_MID_VERT, i_MID_BOTTOM, i_TOP, i_FULL };
                for (const auto& i : is) {
                    if (i.d >= target_max_height) {
                        space[space_c++] = i;
                    } else {
                        occup[occup_c++] = i;
                    }
                }

                if (space_c) {
                    auto it = std::min_element(
                        space.begin(), space.begin() + space_c,
                        [y](const Info& i1, const Info& i2) -> bool
                        { return std::abs(y - i1.y) < std::abs(y - i2.y); });
                    y = it->y;
                    if (it->gravity != GV_NONE) {
                        *adj_gravity = it->gravity;
                    }
                } else {
                    auto it = std::max_element(
                        occup.begin(), occup.begin() + occup_c,
                        [](const Info& i1, const Info& i2) -> bool
                        { return i1.d < i2.d; });
                    y = it->y;
                    if (it->gravity != GV_NONE) {
                        *adj_gravity = it->gravity;
                    }
                }
            } else {
                auto d1 = root.bottom - root.top;
                if (d1 >= target_max_height) {
                    if (y < root.top) {
                        y = root.top;
                    } else {
                        y = root.bottom - target_max_height;
                    }
                } else {
                    y = root.top;
                }
            }
        }

        return y;
    }

    void calculateGravityBounds(
        const Rect& root, bool is_max_visible, bool is_discretized,
        const Rect& anchor, int gravity, const Size& target_max_size, Rect* out, int* adj_gravity)
    {
        if (gravity == 0 || !out) {
            return;
        }

        int x, adj_gravity_x;
        if (gravity & GV_START) {
            x = anchor.left - target_max_size.width;
            adj_gravity_x = GV_START;
        } else if (gravity & GV_MID_START) {
            x = anchor.left;
            adj_gravity_x = GV_MID_START;
        } else if (gravity & GV_MID_HORI) {
            x = anchor.left + (anchor.width() - target_max_size.width) / 2;
            adj_gravity_x = GV_MID_HORI;
        } else if (gravity & GV_MID_END) {
            x = anchor.right - target_max_size.width;
            adj_gravity_x = GV_MID_END;
        } else {
            x = anchor.right;
            adj_gravity_x = GV_END;
        }

        if (is_max_visible) {
            x = adjustX(root, anchor, target_max_size.width, x, is_discretized, &adj_gravity_x);
        }

        int y, adj_gravity_y;
        if (gravity & GV_TOP) {
            y = anchor.top - target_max_size.height;
            adj_gravity_y = GV_TOP;
        } else if (gravity & GV_MID_TOP) {
            y = anchor.top;
            adj_gravity_y = GV_MID_TOP;
        } else if (gravity & GV_MID_VERT) {
            y = anchor.top + (anchor.height() - target_max_size.height) / 2;
            adj_gravity_y = GV_MID_VERT;
        } else if (gravity & GV_MID_BOTTOM) {
            y = anchor.bottom - target_max_size.height;
            adj_gravity_y = GV_MID_BOTTOM;
        } else {
            y = anchor.bottom;
            adj_gravity_y = GV_BOTTOM;
        }

        if (is_max_visible) {
            y = adjustY(root, anchor, target_max_size.height, y, is_discretized, &adj_gravity_y);
        }

        out->setPos(x, y);
        out->setSize(target_max_size);

        if (adj_gravity) {
            *adj_gravity = adj_gravity_x | adj_gravity_y;
        }
    }

}
