// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_RESOURCES_DIMENSION_UTILS_H_
#define UKIVE_RESOURCES_DIMENSION_UTILS_H_

#include "ukive/graphics/colors/color.h"
#include "ukive/graphics/padding.hpp"
#include "ukive/graphics/size.hpp"
#include "ukive/resources/attributes.h"
#include "ukive/resources/dim.hpp"


namespace ukive {

    class Context;

    bool hasAttr(
        AttrsRef attrs, const std::string& key);
    bool resolveDimension(
        const Context& c, const std::string_view& dm, int* out);
    bool resolveDimension(
        const Context& c, const std::string_view& dm, float* out);
    bool resolveDimensionRaw(
        const std::string_view& dm, dim* out);

    int resolveAttrInt(
        AttrsRef attrs, const std::string& key, int def_val);
    float resolveAttrFloat(
        AttrsRef attrs, const std::string& key, float def_val);
    float resolveAttrDimension(
        const Context& c, AttrsRef attrs, const std::string& key, float def_val);
    dim resolveAttrDimensionRaw(
        AttrsRef attrs, const std::string& key, dimcref def_val);
    bool resolveAttrBool(
        AttrsRef attrs, const std::string& key, bool def_val);
    std::string resolveAttrString(
        AttrsRef attrs, const std::string& key, const std::string& def_val);

    bool resolveAttrVisibility(
        AttrsRef attrs, const std::string& key, int* visibility);
    bool resolveAttrPadding(
        const Context& c, AttrsRef attrs, const std::string& key, Padding* padding);
    bool resolveAttrMargin(
        const Context& c, AttrsRef attrs, const std::string& key, Margin* margin);
    bool resolveAttrColor(
        AttrsRef attrs, const std::string& key, Color* c);

    void resolveAttrLayoutSize(
        const Context& c, AttrsRef attrs, Size* size);

}

#endif  // UKIVE_RESOURCES_DIMENSION_UTILS_H_