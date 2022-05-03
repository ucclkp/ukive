// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gravity.h"

#include <array>

#include "utils/log.h"


namespace ukive {

    int gv_adjustX(
        const Rect& root, const Rect& anchor,
        int ref_gravity, int target_max_width,
        int x, bool is_discretized, int* adj_gravity)
    {
        struct Info {
            int d;
            int x;
            int gravity;
        };

        if (x + target_max_width > root.right() || x < root.x()) {
            if (is_discretized) {
                auto end_x = (std::max)(anchor.right(), root.x());
                auto end_right = root.right();

                auto mid_s_x = (std::max)(anchor.x(), root.x());
                auto mid_s_right = (std::min)(anchor.x() + target_max_width, root.right());

                auto mid_h_x = (std::max)((
                    anchor.x() - (target_max_width - anchor.width() + 1) / 2), root.x());
                auto mid_h_right = (std::min)((
                    anchor.right() + (target_max_width - anchor.width() + 1) / 2), root.right());

                auto mid_e_x = (std::max)(anchor.right() - target_max_width, root.x());
                auto mid_e_right = (std::min)(anchor.right(), root.right());

                auto start_x = (std::max)((std::min)(anchor.x(), root.right()) - target_max_width, root.x());
                auto start_right = root.right();

                /**
                 * 事先计算好这几种离散的位置的容纳空间、y 值和 gravity。
                 * 最后一种情况用于稳定计算结果，防止 target 不断减小的过程中出现的跳变。
                 */
                Info i_END{       end_right - end_x,     end_x,   GV_END };
                Info i_MID_START{ mid_s_right - mid_s_x, mid_s_x, GV_MID_START };
                Info i_MID_HORI{  mid_h_right - mid_h_x, mid_h_x, GV_MID_HORI };
                Info i_MID_END{   mid_e_right - mid_e_x, mid_e_x, GV_MID_END };
                Info i_START{     start_right - start_x, start_x, GV_START };

                size_t space_c = 0, occup_c = 0;
                std::array<Info, 5> space;
                std::array<Info, 5> occup;
                std::array<Info, 5> is{ i_END, i_MID_START, i_MID_HORI, i_MID_END, i_START };
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
                auto d1 = root.width();
                if (d1 >= target_max_width) {
                    if (x < root.x()) {
                        x = root.x();
                    } else {
                        x = root.right() - target_max_width;
                    }
                } else {
                    x = root.x();
                }
            }
        }

        return x;
    }

    int gv_adjustY(
        const Rect& root, const Rect& anchor,
        int ref_gravity, int target_max_height,
        int y, bool is_discretized, int* adj_gravity)
    {
        struct Info {
            int d;
            int y;
            int gravity;
        };

        if (y + target_max_height > root.bottom() || y < root.y()) {
            if (is_discretized) {
                auto bottom_y = (std::max)(anchor.bottom(), root.y());
                auto bottom_bottom = root.bottom();

                auto mid_t_y = (std::max)(anchor.y(), root.y());
                auto mid_t_bottom = (std::min)(anchor.y() + target_max_height, root.bottom());

                auto mid_v_y = (std::max)((
                    anchor.y() - (target_max_height - anchor.height() + 1) / 2), root.y());
                auto mid_v_bottom = (std::min)((
                    anchor.bottom() + (target_max_height - anchor.height() + 1) / 2), root.bottom());

                auto mid_b_y = (std::max)(anchor.bottom() - target_max_height, root.y());
                auto mid_b_bottom = (std::min)(anchor.bottom(), root.bottom());

                auto top_y = (std::max)((std::min)(anchor.y(), root.bottom()) - target_max_height, root.y());
                auto top_bottom = root.bottom();

                Info i_BOTTOM{     bottom_bottom - bottom_y, bottom_y, GV_BOTTOM,     };
                Info i_MID_TOP{    mid_t_bottom - mid_t_y,   mid_t_y,  GV_MID_TOP,    };
                Info i_MID_VERT{   mid_v_bottom - mid_v_y,   mid_v_y,  GV_MID_VERT,   };
                Info i_MID_BOTTOM{ mid_b_bottom - mid_b_y,   mid_b_y,  GV_MID_BOTTOM, };
                Info i_TOP{        top_bottom - top_y,       top_y,    GV_TOP,        };

                size_t space_c = 0, occup_c = 0;
                std::array<Info, 5> space;
                std::array<Info, 5> occup;
                std::array<Info, 5> is{ i_BOTTOM, i_MID_TOP, i_MID_VERT, i_MID_BOTTOM, i_TOP };
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
                auto d1 = root.height();
                if (d1 >= target_max_height) {
                    if (y < root.y()) {
                        y = root.y();
                    } else {
                        y = root.bottom() - target_max_height;
                    }
                } else {
                    y = root.y();
                }
            }
        }

        return y;
    }

    void calculateGravityBounds(
        const Rect& root, bool is_max_visible, bool is_discretized,
        const Rect& anchor, int gravity, const Size& target_max_size, Rect* out, int* adj_gravity)
    {
        int x, adj_gravity_x;
        if (gravity & GV_START) {
            x = anchor.x() - target_max_size.width();
            adj_gravity_x = GV_START;
        } else if (gravity & GV_MID_START) {
            x = anchor.x();
            adj_gravity_x = GV_MID_START;
        } else if (gravity & GV_MID_HORI) {
            x = anchor.x() + (anchor.width() - target_max_size.width()) / 2;
            adj_gravity_x = GV_MID_HORI;
        } else if (gravity & GV_MID_END) {
            x = anchor.right() - target_max_size.width();
            adj_gravity_x = GV_MID_END;
        } else {
            x = anchor.right();
            adj_gravity_x = GV_END;
        }

        if (is_max_visible) {
            x = gv_adjustX(
                root, anchor,
                gravity, target_max_size.width(), x, is_discretized, &adj_gravity_x);
        }

        int y, adj_gravity_y;
        if (gravity & GV_TOP) {
            y = anchor.y() - target_max_size.height();
            adj_gravity_y = GV_TOP;
        } else if (gravity & GV_MID_TOP) {
            y = anchor.y();
            adj_gravity_y = GV_MID_TOP;
        } else if (gravity & GV_MID_VERT) {
            y = anchor.y() + (anchor.height() - target_max_size.height()) / 2;
            adj_gravity_y = GV_MID_VERT;
        } else if (gravity & GV_MID_BOTTOM) {
            y = anchor.bottom() - target_max_size.height();
            adj_gravity_y = GV_MID_BOTTOM;
        } else {
            y = anchor.bottom();
            adj_gravity_y = GV_BOTTOM;
        }

        if (is_max_visible) {
            y = gv_adjustY(
                root, anchor,
                gravity, target_max_size.height(), y, is_discretized, &adj_gravity_y);
        }

        out->pos(x, y);
        out->size(target_max_size);

        if (adj_gravity) {
            *adj_gravity = adj_gravity_x | adj_gravity_y;
        }
    }

    int cr_adjustX(
        const Rect& root, const Rect& reco, int target_max_width)
    {
        int x = reco.x();
        if (x + target_max_width > root.right() || x < root.x()) {
            auto d1 = root.width();
            if (d1 >= target_max_width) {
                if (x < root.x()) {
                    x = root.x();
                } else {
                    x = root.right() - target_max_width;
                }
            } else {
                x = root.x();
            }
        }

        return x;
    }

    int cr_adjustY(
        const Rect& root, const Rect& reco, int target_max_height)
    {
        int y = reco.y();
        if (y + target_max_height > root.bottom() || y < root.y()) {
            auto d1 = root.height();
            if (d1 >= target_max_height) {
                if (y < root.y()) {
                    y = root.y();
                } else {
                    y = root.bottom() - target_max_height;
                }
            } else {
                y = root.y();
            }
        }

        return y;
    }

    void calculateCornerBounds(
        const Rect& root, bool is_max_visible, bool is_evaded,
        const Point& pos, const Padding& pp, int corner, const Size& size, Rect* out, int* adj_corner)
    {
        struct MidInfo {
            int d;
            int cor;
        };

        struct Result {
            Point pos;
            int cor;
        };

        Rect cor_pos(pos, {});
        cor_pos.extend(pp);

        MidInfo h[]{
            {cor_pos.right(), GV_START},
            {pos.x() - size.width() / 2, GV_MID_HORI},
            {cor_pos.x() - size.width(), GV_END},
        };
        constexpr auto sh = std::size(h);

        MidInfo v[]{
            {cor_pos.bottom(), GV_TOP},
            {pos.y() - size.height() / 2, GV_MID_VERT},
            {cor_pos.y() - size.height(), GV_BOTTOM},
        };
        constexpr auto sv = std::size(h);

        MidInfo* seq_h[sh];
        MidInfo* seq_v[sv];
        if ((corner & GV_END) || (corner & GV_MID_END)) {
            seq_h[0] = &h[2];
            seq_h[1] = &h[2];
            seq_h[2] = &h[0];
        } else if (corner & GV_MID_HORI) {
            seq_h[0] = &h[1];
            seq_h[1] = &h[0];
            seq_h[2] = &h[2];
        } else {
            seq_h[0] = &h[0];
            seq_h[1] = &h[0];
            seq_h[2] = &h[2];
        }
        if ((corner & GV_BOTTOM) || (corner & GV_MID_BOTTOM)) {
            seq_v[0] = &v[2];
            seq_v[1] = &v[2];
            seq_v[2] = &v[0];
        } else if (corner & GV_MID_VERT) {
            seq_v[0] = &v[1];
            seq_v[1] = &v[0];
            seq_v[2] = &v[2];
        } else {
            seq_v[0] = &v[0];
            seq_v[1] = &v[0];
            seq_v[2] = &v[2];
        }

        Result seq[sh * sv];
        for (size_t i = 0; i < sv; ++i) {
            for (size_t j = 0; j < sh; ++j) {
                Point p{ seq_h[j]->d, seq_v[i]->d };
                int cor = seq_h[j]->cor | seq_v[i]->cor;
                seq[i * sh + j] = { p, cor };
            }
        }

        size_t t = 0;
        if (is_evaded) {
            Rect rb[sh * sv];
            for (size_t i = 0; i < sh * sv; ++i) {
                Rect bounds(seq[i].pos, size);
                rb[i] = bounds & root;
                if (rb[i] == bounds) {
                    *out = bounds;
                    *adj_corner = seq[i].cor;
                    return;
                }
            }

            intmax_t max_area = 0;
            for (size_t i = 0; i < sh * sv; ++i) {
                intmax_t area = rb[i].width() * rb[i].height();
                if (area > max_area) {
                    max_area = area;
                    t = i;
                }
            }
        }

        Rect bounds(seq[t].pos, size);
        if (is_max_visible) {
            int x = cr_adjustX(root, bounds, size.width());
            int y = cr_adjustY(root, bounds, size.height());
            bounds.pos(x, y);
        }

        *out = bounds;
        *adj_corner = seq[t].cor;
    }

}
