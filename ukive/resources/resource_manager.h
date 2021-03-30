// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_RESOURCES_RESOURCE_MANAGER_H_
#define UKIVE_RESOURCES_RESOURCE_MANAGER_H_

#include <filesystem>


namespace ukive {

    class ResourceManager {
    public:
        ResourceManager();

        bool initialize();
        void destroy();

        const std::filesystem::path& getExeDirPath() const;
        const std::filesystem::path& getResRootPath() const;

        bool getFileData(const std::filesystem::path& p, std::string* out) const;

    private:
        std::filesystem::path exe_dir_path_;
        std::filesystem::path resource_root_path_;
    };

}

#endif  // UKIVE_RESOURCES_RESOURCE_MANAGER_H_