// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of necro project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef NECRO_COPY_PROCESSOR_H_
#define NECRO_COPY_PROCESSOR_H_

#include <filesystem>
#include <vector>


namespace necro {

    namespace fs = std::filesystem;

    class CopyProcessor {
    public:
        CopyProcessor();

        bool copy(const fs::path& src, const fs::path& dst);

    private:
        void traverseDst(const fs::path& path);
        void traverseSrc(const fs::path& path);

        fs::path src_path_;
        fs::path dst_path_;
        std::vector<fs::directory_entry> dst_files_;
    };

}

#endif  // NECRO_COPY_PROCESSOR_H_
