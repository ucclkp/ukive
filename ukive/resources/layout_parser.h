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

#include "ukive/views/view.h"

#define UKIVE_ADD_VIEW_NAME(name)  \
    ::ukive::LayoutParser::addViewName(  \
        #name,  \
        [](::ukive::Context c, ::ukive::AttrsRef attrs)->::ukive::View* {  \
            return new ::name(c, attrs);  \
        });


namespace ukive {

    class View;
    class LayoutView;

    class LayoutParser {
    public:
        using Handler = std::function<View* (Context c, AttrsRef attrs)>;

        LayoutParser();

        static void initialize();

        static void addViewName(const std::string& name, const Handler& ctor);
        static void removeViewName(const std::string& name);

        static View* from(Context c, LayoutView* parent, int layout_id);

    private:
        using Map = std::map<std::string, Handler>;
        using ElementPtr = std::shared_ptr<utl::xml::Element>;

        View* parse(Context c, LayoutView* parent, int layout_id);
        bool fetchLayoutFileName(int layout_id, std::filesystem::path* file_name);
        bool traverseTree(const ElementPtr& element, View** parent);

        bool has_read_lim_;
        Context context_;
        LayoutView* root_parent_;
        std::map<int, std::filesystem::path> layout_id_map_;

        static Map handler_map_;
        static Map handler_map2_;
    };

}

#endif  // UKIVE_RESOURCES_LAYOUT_PARSER_H_