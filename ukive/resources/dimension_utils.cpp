// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/resources/dimension_utils.h"

#include "utils/convert.h"
#include "utils/log.h"
#include "utils/string_utils.hpp"

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
            if (utl::stringToNumber(dm.substr(0, dm.length() - 2), &val)) {
                *out = c.dp2pxi(val);
                return true;
            }
        } else if (utl::endWith(dm, "px", false)) {
            int val = 0;
            if (utl::stringToNumber(dm.substr(0, dm.length() - 2), &val)) {
                *out = val;
                return true;
            }
        } else {
            int val = 0;
            if (utl::stringToNumber(dm, &val)) {
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
            if (utl::stringToNumber(dm.substr(0, dm.length() - 2), &val)) {
                *out = c.dp2px(val);
                return true;
            }
        } else if (utl::endWith(dm, "px", false)) {
            float val = 0;
            if (utl::stringToNumber(dm.substr(0, dm.length() - 2), &val)) {
                *out = val;
                return true;
            }
        } else {
            float val = 0;
            if (utl::stringToNumber(dm, &val)) {
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
            if (utl::stringToNumber(dm.substr(0, dm.length() - 2), &val)) {
                out->set_dp(val);
                return true;
            }
        } else if (utl::endWith(dm, "px", false)) {
            float val = 0;
            if (utl::stringToNumber(dm.substr(0, dm.length() - 2), &val)) {
                out->set_px(val);
                return true;
            }
        } else {
            float val = 0;
            if (utl::stringToNumber(dm, &val)) {
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
            if (utl::stringToNumber(it->second, &val)) {
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
            if (utl::stringToNumber(it->second, &val)) {
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
            if (utl::tolatl(it->second) == "true") {
                return true;
            }
            if (utl::tolatl(it->second) == "false") {
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

    bool resolveAttrPadding(
        const Context& c, AttrsRef attrs, const std::string& key, Padding* padding)
    {
        auto padding_str = resolveAttrString(attrs, key, {});
        if (!padding_str.empty()) {
            auto paddings = utl::split(padding_str, ",");
            if (paddings.size() > 0) {
                auto padding_start = paddings[0];
                utl::trim(&padding_start);
                if (resolveDimension(c, padding_start, &padding->start)) {
                    if (paddings.size() == 1) {
                        padding->top = padding->start;
                        padding->end = padding->start;
                        padding->bottom = padding->start;
                    }
                }
            }
            if (paddings.size() > 1) {
                auto padding_top = paddings[1];
                utl::trim(&padding_top);
                resolveDimension(c, padding_top, &padding->top);
            }
            if (paddings.size() > 2) {
                auto padding_end = paddings[2];
                utl::trim(&padding_end);
                resolveDimension(c, padding_end, &padding->end);
            }
            if (paddings.size() > 3) {
                auto padding_bottom = paddings[3];
                utl::trim(&padding_bottom);
                resolveDimension(c, padding_bottom, &padding->bottom);
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
                auto margin_start = margins[0];
                utl::trim(&margin_start);
                if (resolveDimension(c, margin_start, &margin->start)) {
                    if (margins.size() == 1) {
                        margin->top = margin->start;
                        margin->end = margin->start;
                        margin->bottom = margin->start;
                    }
                }
            }
            if (margins.size() > 1) {
                auto margin_top = margins[1];
                utl::trim(&margin_top);
                resolveDimension(c, margin_top, &margin->top);
            }
            if (margins.size() > 2) {
                auto margin_end = margins[2];
                utl::trim(&margin_end);
                resolveDimension(c, margin_end, &margin->end);
            }
            if (margins.size() > 3) {
                auto margin_bottom = margins[3];
                utl::trim(&margin_bottom);
                resolveDimension(c, margin_bottom, &margin->bottom);
            }
            return true;
        }
        return false;
    }

    void resolveAttrLayoutSize(
        const Context& c, AttrsRef attrs, Size* size)
    {
        auto width_attr = utl::tolatl(resolveAttrString(
            attrs, necro::kAttrLayoutWidth, necro::kAttrValLayoutAuto));

        if (!resolveDimension(c, width_attr, &size->width)) {
            if (width_attr == necro::kAttrValLayoutAuto) {
                size->width = View::LS_AUTO;
            }
            else if (width_attr == necro::kAttrValLayoutFill) {
                size->width = View::LS_FILL;
            }
            else if (width_attr == necro::kAttrValLayoutFree) {
                size->width = View::LS_FREE;
            }
            else {
                DLOG(Log::ERR) << "Unknown attr: " << width_attr;
                size->width = View::LS_AUTO;
            }
        }

        auto height_attr = utl::tolatl(resolveAttrString(
            attrs, necro::kAttrLayoutHeight, necro::kAttrValLayoutAuto));

        if (!resolveDimension(c, height_attr, &size->height)) {
            if (height_attr == necro::kAttrValLayoutAuto) {
                size->height = View::LS_AUTO;
            }
            else if (height_attr == necro::kAttrValLayoutFill) {
                size->height = View::LS_FILL;
            }
            else if (height_attr == necro::kAttrValLayoutFree) {
                size->height = View::LS_FREE;
            }
            else {
                DLOG(Log::ERR) << "Unknown attr: " << width_attr;
                size->height = View::LS_AUTO;
            }
        }
    }

}
