// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_MAC_IPC_PIPE_CLIENT_MAC_H_
#define UKIVE_SYSTEM_MAC_IPC_PIPE_CLIENT_MAC_H_

#include <string>

#include "ukive/system/ipc/pipe_client.h"


namespace ukive {
namespace mac {

    class PipeClientMac : public PipeClient {
    public:
        PipeClientMac();
        ~PipeClientMac();

        bool connect(const std::u16string_view& name) override;
        void disconnect() override;
        bool read(std::string* buf) override;
        bool write(const std::string_view& buf) override;

    private:
    };

}
}

#endif  // UKIVE_SYSTEM_MAC_IPC_PIPE_CLIENT_MAC_H_
