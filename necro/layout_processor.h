// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of necro project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef NECRO_LAYOUT_PROCESSOR_H_
#define NECRO_LAYOUT_PROCESSOR_H_

#include <filesystem>
#include <map>

#include "utils/xml/xml_structs.h"


namespace necro {

    namespace fs = std::filesystem;

    class LayoutProcessor {
    public:
        using IdMap = std::map<std::string, int>;
        using ElementPtr = std::shared_ptr<utl::xml::Element>;

        LayoutProcessor();

        bool process(const fs::path& res_dir, const fs::path& out_dir, bool* changed);

        const IdMap& getViewIdMap() const;
        const IdMap& getLayoutIdMap() const;

    private:
        struct History {
            fs::path xml_path;
            uint64_t ts;
        };

        void readHistory(const fs::path& out_dir, std::vector<History>* histories);
        void writeHistory(const fs::path& out_dir, const std::vector<History>& histories);
        bool traverseTree(const ElementPtr& element, bool is_first, IdMap* cur_map);

        bool need_second_;
        int cur_view_id_;
        int cur_layout_id_;
        IdMap view_id_map_;
        IdMap layout_id_map_;
    };

}

#endif  // NECRO_LAYOUT_PROCESSOR_H_
