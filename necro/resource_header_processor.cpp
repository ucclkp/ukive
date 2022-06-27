// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of necro project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "necro/resource_header_processor.h"

#include <fstream>

#include "utils/log.h"
#include "utils/strings/string_utils.hpp"


namespace necro {

    ResourceHeaderProcessor::ResourceHeaderProcessor()
        : indent_(Indent::Space),
          indent_count_(4),
          line_break_(LineBreak::CRLF) {}

    void ResourceHeaderProcessor::setIndent(Indent indent, uint32_t count) {
        indent_ = indent;
        indent_count_ = count;
    }

    void ResourceHeaderProcessor::setLineBreak(LineBreak lb) {
        line_break_ = lb;
    }

    bool ResourceHeaderProcessor::write(
        const fs::path& path, const IdMap& view_id_map, const IdMap& layout_id_map)
    {
        std::ofstream writer(path, std::ios::binary | std::ios::ate);
        if (writer.fail()) {
            jour_e("Cannot open file: %s", path);
            return false;
        }

        std::error_code ec;
        if (!fs::create_directories(path.parent_path(), ec) && ec) {
            jour_e("Failed to make dir: %s", path.parent_path());
            return false;
        }
        auto name_macro = path.filename().u16string();
        utl::tolatu(&name_macro);
        if (name_macro.empty()) {
            jour_e("Invalid out file name: %s", path);
            return false;
        }

        utl::replaceAllTokens(&name_macro, u".", u"_");
        name_macro.append(u"_");

        std::string out_str;
        generateOutput(utl::u16to8(name_macro), view_id_map, layout_id_map, &out_str);

        writer.write(out_str.data(), out_str.length());

        return true;
    }

    void ResourceHeaderProcessor::generateOutput(
        const std::string_view& name_macro,
        const IdMap& view_id_map, const IdMap& layout_id_map, std::string* out)
    {
        ubassert(out != nullptr && out->empty());

        out->append("#ifndef RESOURCES_NECRO_").append(name_macro);
        out->append(getLineBreak());
        out->append("#define RESOURCES_NECRO_").append(name_macro);
        out->append(getLineBreak(2));
        out->append("#include <cstdint>");
        out->append(getLineBreak(3));
        out->append("namespace Res {").append(getLineBreak());
        out->append(getLineBreak());

        // view id
        out->append(getIndent(1)).append("namespace Id {").append(getLineBreak());
        out->append(getLineBreak());

        for (const auto& pair : view_id_map) {
            out->append(getIndent(2)).append("const int ");
            out->append(pair.first);
            out->append(" = ");
            out->append(std::to_string(pair.second));
            out->append(";");
            out->append(getLineBreak());
        }

        out->append(getLineBreak());
        out->append(getIndent(1)).append("}  // namespace Id").append(getLineBreak(2));

        // layout id
        out->append(getIndent(1)).append("namespace Layout {").append(getLineBreak());
        out->append(getLineBreak());

        for (const auto& pair : layout_id_map) {
            out->append(getIndent(2)).append("const int ");

            auto id = pair.first;
            utl::replaceAllTokens(&id, ".", "_");

            out->append(id);
            out->append(" = ");
            out->append(std::to_string(pair.second));
            out->append(";");
            out->append(getLineBreak());
        }

        out->append(getLineBreak());
        out->append(getIndent(1)).append("}  // namespace Layout").append(getLineBreak(2));

        out->append("}  // namespace Res").append(getLineBreak(2));
        out->append("#endif  // RESOURCES_NECRO_").append(name_macro);
    }

    std::string ResourceHeaderProcessor::getIndent(uint32_t layer) const {
        char indent_ch;
        std::string indent_str;
        switch (indent_) {
        case Indent::Space: indent_ch = ' '; break;
        case Indent::Tab: indent_ch = '\t'; break;
        default: indent_ch = ' '; break;
        }
        for (uint32_t i = 0; i < layer * indent_count_; ++i) {
            indent_str.push_back(indent_ch);
        }
        return indent_str;
    }

    std::string ResourceHeaderProcessor::getLineBreak(uint32_t count) const {
        if (count == 0) {
            return {};
        }

        std::string break_unit;
        std::string break_str;
        switch (line_break_) {
        case LineBreak::CR: break_unit = "\r"; break;
        case LineBreak::LF: break_unit = "\n"; break;
        case LineBreak::CRLF: break_unit = "\r\n"; break;
        default: break_unit = "\r\n"; break;
        }

        for (uint32_t i = 0; i < count; ++i) {
            break_str.append(break_unit);
        }
        return break_str;
    }

}
