// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_IPC_PIPE_SERVER_WIN_H_
#define UKIVE_SYSTEM_IPC_PIPE_SERVER_WIN_H_

#include <string>

#include <Windows.h>


namespace ukive {

    class PipeServerWin {
    public:
        PipeServerWin();
        ~PipeServerWin();

        bool create(const std::u16string& name);
        bool wait();
        bool read(std::string* buf);
        bool write(const std::string_view& buf);
        bool disconnect();
        void destroy();

    private:
        enum class State {
            Idle,
            Connecting,
            Writing,
            Reading,
        };

        HANDLE pipe_;
    };

}

#endif  // UKIVE_SYSTEM_IPC_PIPE_SERVER_WIN_H_