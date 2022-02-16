// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_IPC_PIPE_CLIENT_H_
#define UKIVE_SYSTEM_IPC_PIPE_CLIENT_H_

#include <string>


namespace ukive {

    class PipeClient {
    public:
        static PipeClient* create();

        virtual ~PipeClient() = default;

        virtual bool connect(const std::u16string_view& name) = 0;
        virtual void disconnect() = 0;
        virtual bool read(std::string* buf) = 0;
        virtual bool write(const std::string_view& buf) = 0;
    };

}

#endif  // UKIVE_SYSTEM_IPC_PIPE_CLIENT_H_