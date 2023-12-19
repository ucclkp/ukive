// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "attr_utils.h"

#include "utils/log.h"
#include "utils/strings/float_conv.h"
#include "utils/strings/int_conv.hpp"
#include "utils/strings/string_utils.hpp"

#include "ukive/views/view.h"
#include "ukive/window/context.h"

#include "necro/layout_constants.h"


namespace ukive {

    bool hasAttr(AttrsRef attrs, const std::string& key) {
        auto it = attrs.find(key);
        return it != attrs.end();
    }

    bool resolveDimension(
        const Context& c, const std::string_view& dm, int* out)
    {
        if (utl::endWith(dm, "dp", false)) {
            int val = 0;
            if (utl::stoi(dm.substr(0, dm.length() - 2), &val)) {
                *out = c.dp2pxi(val);
                return true;
            }
        } else if (utl::endWith(dm, "px", false)) {
            int val = 0;
            if (utl::stoi(dm.substr(0, dm.length() - 2), &val)) {
                *out = val;
                return true;
            }
        } else {
            int val = 0;
            if (utl::stoi(dm, &val)) {
                *out = c.dp2pxi(val);
                return true;
            }
        }
        return false;
    }

    bool resolveDimension(
        const Context& c, const std::string_view& dm, float* out)
    {
        if (utl::endWith(dm, "dp", false)) {
            float val = 0;
            if (utl::stof(dm.substr(0, dm.length() - 2), &val) == utl::SCR_OK) {
                *out = c.dp2px(val);
                return true;
            }
        } else if (utl::endWith(dm, "px", false)) {
            float val = 0;
            if (utl::stof(dm.substr(0, dm.length() - 2), &val) == utl::SCR_OK) {
                *out = val;
                return true;
            }
        } else {
            float val = 0;
            if (utl::stof(dm, &val) == utl::SCR_OK) {
                *out = c.dp2px(val);
                return true;
            }
        }
        return false;
    }

    bool resolveDimensionRaw(
        const std::string_view& dm, dim* out)
    {
        if (utl::endWith(dm, "dp", false)) {
            float val = 0;
            if (utl::stof(dm.substr(0, dm.length() - 2), &val) == utl::SCR_OK) {
                out->set_dp(val);
                return true;
            }
        } else if (utl::endWith(dm, "px", false)) {
            float val = 0;
            if (utl::stof(dm.substr(0, dm.length() - 2), &val) == utl::SCR_OK) {
                out->set_px(val);
                return true;
            }
        } else {
            float val = 0;
            if (utl::stof(dm, &val) == utl::SCR_OK) {
                out->set_dp(val);
                return true;
            }
        }
        return false;
    }

    int resolveAttrInt(
        AttrsRef attrs, const std::string& key, int def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            int val = 0;
            if (utl::stoi(it->second, &val)) {
                return val;
            }
            LOG(Log::WARNING) << "Cannot resolve int attr " << key
                << ", val: " << it->second;
        }
        return def_val;
    }

    float resolveAttrFloat(
        AttrsRef attrs, const std::string& key, float def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            float val = 0;
            if (utl::stof(it->second, &val) == utl::SCR_OK) {
                return val;
            }
            LOG(Log::WARNING) << "Cannot resolve float attr " << key
                << ", val: " << it->second;
        }
        return def_val;
    }

    float resolveAttrDimension(
        const Context& c, AttrsRef attrs, const std::string& key, float def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            float val = 0;
            if (resolveDimension(c, it->second, &val)) {
                return val;
            }
            LOG(Log::WARNING) << "Cannot resolve dimension attr " << key
                << ", val: " << it->second;
        }
        return def_val;
    }

    dim resolveAttrDimensionRaw(
        AttrsRef attrs, const std::string& key, dimcref def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            dim val;
            if (resolveDimensionRaw(it->second, &val)) {
                return val;
            }
            LOG(Log::WARNING) << "Cannot resolve dimension attr " << key
                << ", val: " << it->second;
        }
        return def_val;
    }

    bool resolveAttrBool(
        AttrsRef attrs, const std::string& key, bool def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            if (utl::isLitEqual(it->second, "true")) {
                return true;
            }
            if (utl::isLitEqual(it->second, "false")) {
                return false;
            }
            LOG(Log::WARNING) << "Cannot resolve bool attr " << key
                << ", val: " << it->second;
        }
        return def_val;
    }

    std::string resolveAttrString(
        AttrsRef attrs, const std::string& key, const std::string& def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            return it->second;
        }
        return def_val;
    }

    bool resolveAttrVisibility(
        AttrsRef attrs, const std::string& key, int* visibility)
    {
        auto v_str = resolveAttrString(attrs, key, {});
        if (v_str.empty()) {
            return false;
        }

        if (v_str == necro::kAttrValViewShow) {
            *visibility = View::SHOW;
        } else if (v_str == necro::kAttrValViewHide) {
            *visibility = View::HIDE;
        } else if (v_str == necro::kAttrValViewVanished) {
            *visibility = View::VANISHED;
        } else {
            return false;
        }

        return true;
    }

    bool resolveAttrPadding(
        const Context& c, AttrsRef attrs, const std::string& key, Padding* padding)
    {
        auto padding_str = resolveAttrString(attrs, key, {});
        if (!padding_str.empty()) {
            auto paddings = utl::split(padding_str, ",");
            if (paddings.size() > 0) {
                int val;
                auto padding_start = paddings[0];
                utl::trim_self(&padding_start);
                if (resolveDimension(c, padding_start, &val)) {
                    padding->start(val);
                    if (paddings.size() == 1) {
                        padding->top(padding->start());
                        padding->end(padding->start());
                        padding->bottom(padding->start());
                    }
                }
            }
            if (paddings.size() > 1) {
                int val;
                auto padding_top = paddings[1];
                utl::trim_self(&padding_top);
                if (resolveDimension(c, padding_top, &val)) {
                    padding->top(val);
                }
            }
            if (paddings.size() > 2) {
                int val;
                auto padding_end = paddings[2];
                utl::trim_self(&padding_end);
                if (resolveDimension(c, padding_end, &val)) {
                    padding->end(val);
                }
            }
            if (paddings.size() > 3) {
                int val;
                auto padding_bottom = paddings[3];
                utl::trim_self(&padding_bottom);
                if (resolveDimension(c, padding_bottom, &val)) {
                    padding->bottom(val);
                }
            }
            return true;
        }
        return false;
    }

    bool resolveAttrMargin(
        const Context& c, AttrsRef attrs, const std::string& key, Margin* margin)
    {
        auto margin_str = resolveAttrString(attrs, key, {});
        if (!margin_str.empty()) {
            auto margins = utl::split(margin_str, ",");
            if (margins.size() > 0) {
                int val;
                auto margin_start = margins[0];
                utl::trim_self(&margin_start);
                if (resolveDimension(c, margin_start, &val)) {
                    margin->start(val);
                    if (margins.size() == 1) {
                        margin->top(val);
                        margin->end(val);
                        margin->bottom(val);
                    }
                }
            }
            if (margins.size() > 1) {
                int val;
                auto margin_top = margins[1];
                utl::trim_self(&margin_top);
                if (resolveDimension(c, margin_top, &val)) {
                    margin->top(val);
                }
            }
            if (margins.size() > 2) {
                int val;
                auto margin_end = margins[2];
                utl::trim_self(&margin_end);
                if (resolveDimension(c, margin_end, &val)) {
                    margin->end(val);
                }
            }
            if (margins.size() > 3) {
                int val;
                auto margin_bottom = margins[3];
                utl::trim_self(&margin_bottom);
                if (resolveDimension(c, margin_bottom, &val)) {
                    margin->bottom(val);
                }
            }
            return true;
        }
        return false;
    }

    void resolveAttrLayoutSize(
        const Context& c, AttrsRef attrs, Size* size)
    {
        auto width_attr = resolveAttrString(
            attrs, necro::kAttrLayoutWidth, necro::kAttrValLayoutAuto);

        int width;
        if (!resolveDimension(c, width_attr, &width)) {
            if (width_attr == necro::kAttrValLayoutAuto) {
                size->width(View::LS_AUTO);
            }
            else if (width_attr == necro::kAttrValLayoutFill) {
                size->width(View::LS_FILL);
            }
            else if (width_attr == necro::kAttrValLayoutFree) {
                size->width(View::LS_FREE);
            }
            else {
                DLOG(Log::ERR) << "Unknown attr: " << width_attr;
                size->width(View::LS_AUTO);
            }
        } else {
            size->width(width);
        }

        auto height_attr = resolveAttrString(
            attrs, necro::kAttrLayoutHeight, necro::kAttrValLayoutAuto);

        int height;
        if (!resolveDimension(c, height_attr, &height)) {
            if (height_attr == necro::kAttrValLayoutAuto) {
                size->height(View::LS_AUTO);
            }
            else if (height_attr == necro::kAttrValLayoutFill) {
                size->height(View::LS_FILL);
            }
            else if (height_attr == necro::kAttrValLayoutFree) {
                size->height(View::LS_FREE);
            }
            else {
                DLOG(Log::ERR) << "Unknown attr: " << height_attr;
                size->height(View::LS_AUTO);
            }
        } else {
            size->height(height);
        }
    }

    bool resolveAttrColor(
        AttrsRef attrs, const std::string& key, Color* c)
    {
        auto str = resolveAttrString(attrs, key, {});
        if (str.empty()) {
            return false;
        }

        if (key[0] == '#') {
            return Color::parseARGB(str, c);
        }

        return Color::parseName(str, c);
    }

}
