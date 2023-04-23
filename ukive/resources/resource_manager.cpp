// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/resources/resource_manager.h"

#include <fstream>

#include "utils/files/file_utils.h"
#include "utils/numbers.hpp"
#include "utils/platform_utils.h"


namespace ukive {

    ResourceManager::ResourceManager() {
    }

    bool ResourceManager::initialize(const std::u16string_view& app_name) {
        exe_dir_path_ = utl::getExecFileName(true);
        if (exe_dir_path_.empty()) {
            return false;
        }

        app_name_ = app_name;

#ifdef OS_MAC
        resource_root_path_ = exe_dir_path_.parent_path() / u"Resources";
#else
        resource_root_path_ = exe_dir_path_;
#endif

        necro_path_ = resource_root_path_ / u"necro";
        if (!app_name_.empty()) {
            necro_path_ /= app_name_;
        }
        return true;
    }

    void ResourceManager::destroy() {
    }

    const std::filesystem::path& ResourceManager::getExeDirPath() const {
        return exe_dir_path_;
    }

    const std::filesystem::path& ResourceManager::getResRootPath() const {
        return resource_root_path_;
    }

    const std::filesystem::path& ResourceManager::getNecroPath() const {
        return necro_path_;
    }

    std::filesystem::path ResourceManager::getImagePath(
        const std::u16string_view& name) const
    {
        return resource_root_path_ / u"images" / name;
    }

    bool ResourceManager::getFileData(
        const std::filesystem::path& p, std::string* out) const
    {
        std::ifstream reader(p, std::ios::binary);
        if (!reader) {
            return false;
        }

        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        auto byte_size = utl::num_cast<size_t>(std::streamoff(reader.tellg()));
        reader.seekg(cpos);

        out->resize(byte_size);
        if (!reader.read(out->data(), byte_size)) {
            return false;
        }
        return true;
    }

}
