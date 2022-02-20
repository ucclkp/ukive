// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gravity.h"


namespace ukive {

    inline bool E_GV_MID_Selector(
        const Rect& root, const Rect& anchor, int target_width,
        bool is_end, int& x, int& adj_gravity)
    {
        if (is_end) {
            if (root.right >= anchor.right) {
                // GV_MID_END
                x = anchor.right - target_width;
                adj_gravity = GV_MID_END;
                return true;
            }
        } else {
            if (root.right >= anchor.left + target_width) {
                // GV_MID_START
                x = anchor.left;
                adj_gravity = GV_MID_START;
                return true;
            }
        }
        return false;
    }

    inline bool S_GV_MID_Selector(
        const Rect& root, const Rect& anchor, int target_width,
        bool is_end, int& x, int& adj_gravity)
    {
        if (is_end) {
            if (root.left < anchor.right - target_width) {
                // GV_MID_END
                x = anchor.right - target_width;
                adj_gravity = GV_MID_END;
            }
        } else {
            if (root.left <= anchor.left) {
                // GV_MID_START
                x = anchor.left;
                adj_gravity = GV_MID_START;
            }
        }
        return false;
    }

    inline bool B_GV_MID_Selector(
        const Rect& root, const Rect& anchor, int target_height,
        bool is_bottom, int& y, int& adj_gravity)
    {
        if (is_bottom) {
            if (root.bottom >= anchor.bottom) {
                // GV_MID_BOTTOM
                y = anchor.bottom - target_height;
                adj_gravity = GV_MID_BOTTOM;
            }
        } else {
            if (root.bottom >= anchor.top + target_height) {
                // GV_MID_TOP
                y = anchor.top;
                adj_gravity = GV_MID_TOP;
            }
        }
        return false;
    }

    inline bool T_GV_MID_Selector(
        const Rect& root, const Rect& anchor, int target_height,
        bool is_bottom, int& y, int& adj_gravity)
    {
        if (is_bottom) {
            if (root.top < anchor.bottom - target_height) {
                // GV_MID_BOTTOM
                y = anchor.bottom - target_height;
                adj_gravity = GV_MID_BOTTOM;
            }
        } else {
            if (root.top <= anchor.top) {
                // GV_MID_TOP
                y = anchor.top;
                adj_gravity = GV_MID_TOP;
            }
        }
        return false;
    }

    int adjustX(
        const Rect& root, const Rect& anchor, int target_width,
        int x, bool is_discretized, int* adj_gravity)
    {
        // 先看右侧
        if (x + target_width > root.right) {
            if (is_discretized) {
                bool is_beyond = target_width > anchor.width();

                if (E_GV_MID_Selector(root, anchor, target_width, !is_beyond, x, *adj_gravity)) {
                } else if (root.right >= anchor.right + (target_width - anchor.width()) / 2) {
                    // GV_MID_HORI
                    x = anchor.left - (target_width - anchor.width()) / 2;
                    *adj_gravity = GV_MID_HORI;
                } else if (E_GV_MID_Selector(root, anchor, target_width, is_beyond, x, *adj_gravity)) {
                } else if (root.right > anchor.left) {
                    // GV_START
                    x = anchor.left - target_width;
                    *adj_gravity = GV_START;
                } else {
                    // anchor 完全超出 root 右侧
                    x = root.right - target_width;
                    *adj_gravity = GV_START;
                }
            } else {
                x = root.right - target_width;
                *adj_gravity = GV_START;
            }
        }

        // 再看左侧
        if (x < root.left) {
            if (is_discretized) {
                bool is_beyond = target_width > anchor.width();

                if (S_GV_MID_Selector(root, anchor, target_width, !is_beyond, x, *adj_gravity)) {
                } else if (root.left <= anchor.left - (target_width - anchor.width()) / 2) {
                    // GV_MID_HORI
                    x = anchor.left - (target_width - anchor.width()) / 2;
                    *adj_gravity = GV_MID_HORI;
                } else if (S_GV_MID_Selector(root, anchor, target_width, is_beyond, x, *adj_gravity)) {
                } else if (root.left < anchor.right) {
                    // GV_END
                    x = anchor.right;
                    *adj_gravity = GV_END;
                } else {
                    // anchor 完全超出 root 左侧
                    x = root.left;
                    *adj_gravity = GV_END;
                }
            } else {
                x = root.left;
                *adj_gravity = GV_END;
            }
        }

        // 确保左侧一定可见
        if (x >= root.right) {
            x = (std::max)(root.left, root.right - target_width);
            *adj_gravity = GV_START;
        }

        return x;
    }

    int adjustY(
        const Rect& root, const Rect& anchor, int target_height,
        int y, bool is_discretized, int* adj_gravity)
    {
        // 先看底部
        if (y + target_height > root.bottom) {
            if (is_discretized) {
                bool is_beyond = target_height > anchor.height();

                if (B_GV_MID_Selector(root, anchor, target_height, !is_beyond, y, *adj_gravity)) {
                } else if (root.bottom >= anchor.bottom + (target_height - anchor.height()) / 2) {
                    // GV_MID_VERT
                    y = anchor.top - (target_height - anchor.height()) / 2;
                    *adj_gravity = GV_MID_VERT;
                } else if (B_GV_MID_Selector(root, anchor, target_height, is_beyond, y, *adj_gravity)) {
                } else if (root.bottom > anchor.top) {
                    // GV_TOP
                    y = anchor.top - target_height;
                    *adj_gravity = GV_TOP;
                } else {
                    // anchor 完全超出 root 底部
                    y = root.bottom - target_height;
                    *adj_gravity = GV_TOP;
                }
            } else {
                y = root.bottom - target_height;
                *adj_gravity = GV_TOP;
            }
        }

        // 再看顶部
        if (y < root.top) {
            if (is_discretized) {
                bool is_beyond = target_height > anchor.height();

                if (T_GV_MID_Selector(root, anchor, target_height, !is_beyond, y, *adj_gravity)) {
                } else if (root.top <= anchor.top - (target_height - anchor.height()) / 2) {
                    // GV_MID_VERT
                    y = anchor.top - (target_height - anchor.height()) / 2;
                    *adj_gravity = GV_MID_VERT;
                } else if (T_GV_MID_Selector(root, anchor, target_height, is_beyond, y, *adj_gravity)) {
                } else if (root.top < anchor.bottom) {
                    // GV_END
                    y = anchor.bottom;
                    *adj_gravity = GV_BOTTOM;
                } else {
                    // anchor 完全超出 root 顶部
                    y = root.top;
                    *adj_gravity = GV_BOTTOM;
                }
            } else {
                y = root.top;
                *adj_gravity = GV_BOTTOM;
            }
        }

        // 确保顶部一定可见
        if (y >= root.bottom) {
            y = (std::max)(root.top, root.bottom - target_height);
            *adj_gravity = GV_TOP;
        }

        return y;
    }

    void calculateGravityBounds(
        const Rect& root, bool is_max_visible, bool is_discretized,
        const Rect& anchor, int gravity, const Size& target_size, Rect* out, int* adj_gravity)
    {
        if (gravity == 0 || !out) {
            return;
        }

        int x, adj_gravity_x;
        if (gravity & GV_START) {
            x = anchor.left - target_size.width;
            adj_gravity_x = GV_START;
        } else if (gravity & GV_MID_START) {
            x = anchor.left;
            adj_gravity_x = GV_MID_START;
        } else if (gravity & GV_MID_HORI) {
            x = anchor.left + (anchor.width() - target_size.width) / 2;
            adj_gravity_x = GV_MID_HORI;
        } else if (gravity & GV_MID_END) {
            x = anchor.right - target_size.width;
            adj_gravity_x = GV_MID_END;
        } else {
            x = anchor.right;
            adj_gravity_x = GV_END;
        }

        if (is_max_visible) {
            x = adjustX(root, anchor, target_size.width, x, is_discretized, &adj_gravity_x);
        }

        int y, adj_gravity_y;
        if (gravity & GV_TOP) {
            y = anchor.top - target_size.height;
            adj_gravity_y = GV_TOP;
        } else if (gravity & GV_MID_TOP) {
            y = anchor.top;
            adj_gravity_y = GV_MID_TOP;
        } else if (gravity & GV_MID_VERT) {
            y = anchor.top + (anchor.height() - target_size.height) / 2;
            adj_gravity_y = GV_MID_VERT;
        } else if (gravity & GV_MID_BOTTOM) {
            y = anchor.bottom - target_size.height;
            adj_gravity_y = GV_MID_BOTTOM;
        } else {
            y = anchor.bottom;
            adj_gravity_y = GV_BOTTOM;
        }

        if (is_max_visible) {
            y = adjustY(root, anchor, target_size.height, y, is_discretized, &adj_gravity_y);
        }

        out->setPos(x, y);
        out->setSize(target_size);

        if (adj_gravity) {
            *adj_gravity = adj_gravity_x | adj_gravity_y;
        }
    }

}
