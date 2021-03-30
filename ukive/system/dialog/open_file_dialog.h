// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_DIALOG_OPEN_FILE_DIALOG_H_
#define UKIVE_SYSTEM_DIALOG_OPEN_FILE_DIALOG_H_

#include <string>
#include <vector>


namespace ukive {

    class Window;

    class OpenFileDialog {
    public:
        enum Flags : uint32_t {
            OFD_OPEN_FILE = 0,
            OFD_OPEN_DIRECTORY = 1 << 0,
            OFD_MULTI_SELECT = 1 << 1,
            OFD_SHOW_HIDDEN = 1 << 2,
        };

        static OpenFileDialog* create();

        virtual ~OpenFileDialog() = default;

        virtual int show(Window* parent, uint32_t flags) = 0;

        virtual void addType(std::u16string types, std::u16string desc) = 0;
        virtual void clearTypes() = 0;

        virtual const std::vector<std::u16string>& getSelectedFiles() const = 0;
    };

}

#endif  // UKIVE_SYSTEM_DIALOG_OPEN_FILE_DIALOG_H_
