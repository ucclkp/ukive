// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_IPC_PIPE_SERVER_H_
#define UKIVE_SYSTEM_IPC_PIPE_SERVER_H_

#include <string>


namespace ukive {

    class PipeServer {
    public:
        static PipeServer* create();

        virtual ~PipeServer() = default;

        virtual bool create(const std::u16string_view& name) = 0;
        virtual bool wait() = 0;
        virtual bool read(std::string* buf) = 0;
        virtual bool write(const std::string_view& buf) = 0;
        virtual bool disconnect() = 0;
        virtual void destroy() = 0;
    };

}

#endif  // UKIVE_SYSTEM_IPC_PIPE_SERVER_H_