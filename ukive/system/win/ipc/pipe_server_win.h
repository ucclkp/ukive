// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_IPC_PIPE_SERVER_WIN_H_
#define UKIVE_SYSTEM_WIN_IPC_PIPE_SERVER_WIN_H_

#include <string>

#include "ukive/system/ipc/pipe_server.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>


namespace ukive {
namespace win {

    class PipeServerWin : public PipeServer {
    public:
        PipeServerWin();
        ~PipeServerWin();

        bool create(const std::u16string_view& name) override;
        bool wait() override;
        bool read(std::string* buf) override;
        bool write(const std::string_view& buf) override;
        bool disconnect() override;
        void destroy() override;

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
}

#endif  // UKIVE_SYSTEM_WIN_IPC_PIPE_SERVER_WIN_H_