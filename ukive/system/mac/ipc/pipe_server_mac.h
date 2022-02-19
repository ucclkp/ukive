// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_MAC_IPC_PIPE_SERVER_MAC_H_
#define UKIVE_SYSTEM_MAC_IPC_PIPE_SERVER_MAC_H_

#include <string>

#include "ukive/system/ipc/pipe_server.h"


namespace ukive {
namespace mac {

    class PipeServerMac : public PipeServer {
    public:
        PipeServerMac();
        ~PipeServerMac();

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
    };

}
}

#endif  // UKIVE_SYSTEM_MAC_IPC_PIPE_SERVER_MAC_H_
