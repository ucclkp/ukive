// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of necro project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "copy_processor.h"

#include "utils/log.h"
#include "utils/string_utils.hpp"


namespace necro {

    CopyProcessor::CopyProcessor() {}

    bool CopyProcessor::copy(const fs::path& src, const fs::path& dst) {
        src_path_ = src;
        dst_path_ = dst;

        dst_files_.clear();
        traverseDst(dst);
        traverseSrc(src);

        std::error_code ec;
        for (const auto& f : dst_files_) {
            fs::remove(f.path(), ec);
            if (ec) {
                jour_e("Cannot access file: %s %s", f.path(), ec.message());
            }
        }

        fs::copy(
            src, dst,
            fs::copy_options::update_existing | fs::copy_options::recursive, ec);
        if (ec) {
            LOG(Log::ERR) << "Errors occurred during copy"
                << " " << ec.message();
        }

        LOG(Log::INFO) << "Copy operation succeeded";
        return true;
    }

    void CopyProcessor::traverseDst(const fs::path& path) {
        std::error_code ec;
        bool is_exist = fs::exists(path, ec);
        if (ec) {
            jour_e("Cannot access file: %s %s", path, ec.message());
            return;
        }

        if (!is_exist) {
            return;
        }

        auto dir_it = fs::directory_iterator(path, ec);
        if (ec) {
            jour_e("Cannot access file: %s %s", path, ec.message());
            return;
        }

        for (auto& f : dir_it) {
            bool is_dir = f.is_directory(ec);
            if (ec) {
                jour_e("Cannot access file: %s %s", f.path(), ec.message());
                continue;
            }

            dst_files_.push_back(f);

            if (is_dir) {
                traverseDst(f.path());
            }
        }
    }

    void CopyProcessor::traverseSrc(const fs::path& path) {
        std::error_code ec;
        auto dir_it = fs::directory_iterator(path, ec);
        if (ec) {
            jour_e("Cannot access file: %s %s", path, ec.message());
            return;
        }

        for (auto& f : dir_it) {
            bool is_dir = f.is_directory(ec);
            if (ec) {
                jour_e("Cannot access file: %s %s", f.path(), ec.message());
                continue;
            }

            for (auto it = dst_files_.begin(); it != dst_files_.end(); ++it) {
                if (it->path().lexically_relative(dst_path_) ==
                    f.path().lexically_relative(src_path_))
                {
                    dst_files_.erase(it);
                    break;
                }
            }

            if (is_dir) {
                traverseSrc(f.path());
            }
        }
    }

}
