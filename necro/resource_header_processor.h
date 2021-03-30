// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of necro project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef NECRO_RESOURCE_HEADER_PROCESSOR_H_
#define NECRO_RESOURCE_HEADER_PROCESSOR_H_

#include <filesystem>
#include <map>


namespace necro {

    namespace fs = std::filesystem;

    class ResourceHeaderProcessor {
    public:
        using IdMap = std::map<std::string, int>;

        enum class Indent {
            Tab,
            Space,
        };

        enum class LineBreak {
            CR,
            LF,
            CRLF,
        };

        ResourceHeaderProcessor();

        void setIndent(Indent indent, uint32_t count);
        void setLineBreak(LineBreak lb);
        bool write(const fs::path& path, const IdMap& view_id_map, const IdMap& layout_id_map);

    private:
        void generateOutput(
            const std::string& name_macro,
            const IdMap& view_id_map, const IdMap& layout_id_map, std::string* out);
        std::string getIndent(uint32_t layer) const;
        std::string getLineBreak(uint32_t count = 1) const;

        Indent indent_;
        uint32_t indent_count_;
        LineBreak line_break_;
    };

}

#endif  // NECRO_RESOURCE_HEADER_PROCESSOR_H_
