// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of necro project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <fstream>
#include <filesystem>

#include "utils/command_line.h"
#include "utils/log.h"
#include "utils/strings/string_utils.hpp"

#include "necro/copy_processor.h"
#include "necro/layout_processor.h"
#include "necro/layout_constants.h"
#include "necro/resource_header_processor.h"


namespace fs = std::filesystem;

int processXMLFiles(
    const fs::path& res_path,
    const fs::path& build_path,
    const std::u16string_view& app_name)
{
    fs::path layout_xml_file = res_path / u"layout";
    fs::path header_file = res_path / u"necro_resources_id.h";
    fs::path build_res_file = build_path / u"necro";

    if (!app_name.empty()) {
        build_res_file /= app_name;
    }

    LOG(Log::INFO) << "Start processing...";
    LOG(Log::INFO) << "Resource path: " << res_path;
    LOG(Log::INFO) << "Build path: " << build_path;

    bool xml_changed;
    auto layout_out_dir = build_res_file / u"layout";
    necro::LayoutProcessor layout_processor;
    if (layout_processor.process(layout_xml_file, layout_out_dir, &xml_changed)) {
        if (!xml_changed) {
            return 0;
        }

        LOG(Log::INFO) << "Generating resources header...";

        necro::ResourceHeaderProcessor header_processor;
        if (!header_processor.write(
            header_file, layout_processor.getViewIdMap(), layout_processor.getLayoutIdMap()))
        {
            LOG(Log::ERR) << "Failed to generate header: " << header_file;
            return 1;
        }

        if (xml_changed) {
            std::ofstream necro_cache(layout_out_dir / necro::kLayoutHistoryFileName, std::ios::binary | std::ios::in);
            if (necro_cache.good()) {
                uint8_t available = 1;
                necro_cache.write(reinterpret_cast<const char*>(&available), 1);
            }
        }
    } else {
        LOG(Log::ERR) << "Failed to process layout";
        return 1;
    }

    LOG(Log::INFO) << "Succeeded!";

    return 0;
}

int processGeneratorMode() {
    std::u16string res_path_param;
    if (utl::CommandLine::hasName("res_path")) {
        res_path_param = utl::CommandLine::getValue("res_path");
    } else if (utl::CommandLine::hasName("r")) {
        res_path_param = utl::CommandLine::getValue("r");
    } else {
        LOG(Log::ERR) << "Invalid params: please specify resource path! (Using -r or --res_path)";
        return 1;
    }

    std::u16string build_path_param;
    if (utl::CommandLine::hasName("build_path")) {
        build_path_param = utl::CommandLine::getValue("build_path");
    } else if (utl::CommandLine::hasName("b")) {
        build_path_param = utl::CommandLine::getValue("b");
    } else {
        LOG(Log::ERR) << "Invalid params: please specify build path! (Using -b or --build_path)";
        return 1;
    }

    std::u16string app_name;
    if (utl::CommandLine::hasName("app_name")) {
        app_name = utl::CommandLine::getValue("app_name");
    } else if (utl::CommandLine::hasName("n")) {
        app_name = utl::CommandLine::getValue("n");
    }

    utl::trim(&res_path_param, u"\"");
    utl::trim(&build_path_param, u"\"");
    utl::trim(&app_name, u"\"");
    utl::trim(&app_name);

    std::error_code ec;
    auto resource_path = std::filesystem::absolute(res_path_param, ec);
    auto build_path = std::filesystem::absolute(build_path_param, ec);
    if (!resource_path.empty() && !build_path.empty()) {
        return processXMLFiles(resource_path, build_path, app_name);
    }

    LOG(Log::ERR) << "Invalid params: Cannot parse path!";
    return 1;
}

int processCopyMode() {
    std::u16string src_path_param;
    if (utl::CommandLine::hasName("src_path")) {
        src_path_param = utl::CommandLine::getValue("src_path");
    } else if (utl::CommandLine::hasName("s")) {
        src_path_param = utl::CommandLine::getValue("s");
    } else {
        LOG(Log::ERR) << "Invalid params: please specify source dir path! (Using -s or --src_path)";
        return 1;
    }

    std::u16string dst_path_param;
    if (utl::CommandLine::hasName("dst_path")) {
        dst_path_param = utl::CommandLine::getValue("dst_path");
    } else if (utl::CommandLine::hasName("d")) {
        dst_path_param = utl::CommandLine::getValue("d");
    } else {
        LOG(Log::ERR) << "Invalid params: please specify dest dir path! (Using -d or --dst_path)";
        return 1;
    }

    utl::trim(&src_path_param, u"\"");
    utl::trim(&dst_path_param, u"\"");

    std::error_code ec;
    auto src_path = std::filesystem::absolute(src_path_param, ec);
    auto dst_path = std::filesystem::absolute(dst_path_param, ec);
    if (!src_path.empty() && !dst_path.empty()) {
        necro::CopyProcessor processor;
        processor.copy(src_path, dst_path);
        return 0;
    }

    LOG(Log::ERR) << "Invalid params: Cannot parse path!";
    return 1;
}

int main() {
    int result = 0;

    utl::Log::Params log_params;
    log_params.file_name = u"necro.log";
    log_params.short_file_name = true;
    log_params.target = utl::Log::OutputTarget::STANDARD;
    utl::InitLogging(log_params);

    utl::CommandLine::initialize();

    if (utl::CommandLine::hasName("c")) {
        if (result == 0) {
            result = processCopyMode();
        }
    } else if (utl::CommandLine::hasName("g")) {
        if (result == 0) {
            result = processGeneratorMode();
        }
    } else {
        LOG(Log::ERR) << "Unknown mode: please specify copy mode (-c) or generator mode (-g)";
        result = 1;
    }

    if (result != 0) {
        std::getchar();
    }

    utl::UninitLogging();
    return result;
}
