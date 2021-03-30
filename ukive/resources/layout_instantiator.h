// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_RESOURCES_LAYOUT_INSTANTIATOR_H_
#define UKIVE_RESOURCES_LAYOUT_INSTANTIATOR_H_

#include <filesystem>
#include <functional>
#include <map>

#include "utils/xml/xml_structs.h"

#include "ukive/views/view.h"


namespace ukive {

    class View;
    class LayoutView;

    class LayoutInstantiator {
    public:
        using Handler = std::function<View*(Context c, const Attributes& attrs)>;
        using ElementPtr = std::shared_ptr<utl::xml::Element>;

        LayoutInstantiator();

        static void init();
        static View* from(Context c, LayoutView* parent, int layout_id);

        View* instantiate(Context c, LayoutView* parent, int layout_id);

    private:
        bool fetchLayoutFileName(int layout_id, std::filesystem::path* file_name);
        bool traverseTree(const ElementPtr& element, View** parent);

        bool has_read_lim_;
        Context context_;
        LayoutView* root_parent_;
        std::map<int, std::filesystem::path> layout_id_map_;

        static std::map<std::string, Handler> handler_map_;
    };

}

#endif  // UKIVE_RESOURCES_LAYOUT_INSTANTIATOR_H_