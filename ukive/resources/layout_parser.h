// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_RESOURCES_LAYOUT_PARSER_H_
#define UKIVE_RESOURCES_LAYOUT_PARSER_H_

#include <filesystem>
#include <functional>
#include <map>

#include "utils/xml/xml_structs.h"

#include "ukive/views/layout/layout_view.h"

#define UKIVE_ADD_VIEW_NAME(name)  \
    ::ukive::LayoutParser::addViewName(  \
        ::ukive::LayoutParser::genNameByType(#name),  \
        std::is_base_of<::ukive::LayoutView, name>::value,  \
        [](::ukive::Context c, ::ukive::AttrsRef attrs)->::ukive::View* {  \
            return new ::name(c, attrs);  \
        });


namespace ukive {

    class View;
    class LayoutView;

    class LayoutParser {
    public:
        using Creator = std::function<View* (Context c, AttrsRef attrs)>;

        struct ViewInfo {
            bool is_layout;
            Creator creator;
        };

        using ViewMap = std::map<std::string, ViewInfo, std::less<>>;

        LayoutParser();

        static void initialize();

        static std::string genNameByType(const std::string_view& type);

        static void addViewName(
            const std::string_view& name, bool is_layout, Creator&& ctor);
        static void removeViewName(const std::string_view& name);

        static const ViewMap& getViewMap() { return view_map_; }
        static const ViewMap& getViewMap2() { return view_map2_; }

        static View* createView(
            const std::string_view& name, Context c, AttrsRef attrs);

        static View* from(Context c, LayoutView* parent, int layout_id);

    private:
        using ElementPtr = std::shared_ptr<utl::xml::Element>;

        View* parse(Context c, LayoutView* parent, int layout_id);
        bool fetchLayoutFileName(int layout_id, std::filesystem::path* file_name);
        bool traverseTree(const ElementPtr& element, View** parent);

        bool has_read_lim_;
        Context context_;
        LayoutView* root_parent_;
        std::map<int, std::filesystem::path> layout_id_map_;

        static ViewMap view_map_;
        static ViewMap view_map2_;
    };

}

#endif  // UKIVE_RESOURCES_LAYOUT_PARSER_H_