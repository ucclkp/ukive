// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "pipe_server_win.h"

#include "utils/log.h"
#include "utils/numbers.hpp"


namespace ukive {
namespace win {

    PipeServerWin::PipeServerWin()
        : pipe_(INVALID_HANDLE_VALUE) {
    }

    PipeServerWin::~PipeServerWin() {
        if (pipe_ != INVALID_HANDLE_VALUE) {
            ::CloseHandle(pipe_);
        }
    }

    bool PipeServerWin::create(const std::u16string_view& name) {
        if (pipe_ != INVALID_HANDLE_VALUE) {
            return false;
        }

        std::wstring w_name(name.begin(), name.end());
        LPCWSTR pipe_name = w_name.c_str();

        pipe_ = ::CreateNamedPipe(
            pipe_name,               // pipe name
            PIPE_ACCESS_DUPLEX,      // read/write access
            PIPE_TYPE_BYTE |         // message-type pipe
            PIPE_READMODE_BYTE |     // message-read mode
            PIPE_WAIT,               // blocking mode
            1,                       // number of instances
            512 * sizeof(char),     // output buffer size
            512 * sizeof(char),     // input buffer size
            5000,                    // client time-out
            nullptr);                // default security attributes
        if (pipe_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        return true;
    }

    bool PipeServerWin::wait() {
        if (pipe_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        // Start an overlapped connection for this pipe instance.
        BOOL fConnected = ::ConnectNamedPipe(pipe_, nullptr);
        if (fConnected == 0) {
            auto err = ::GetLastError();
            if (err != ERROR_PIPE_CONNECTED) {
                LOG(Log::ERR) << "ConnectNamedPipe failed with " << ::GetLastError();
                return false;
            }
        }
        return true;
    }

    bool PipeServerWin::read(std::string* buf) {
        if (pipe_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        char chBuf[512];
        DWORD cbRead = 0;
        BOOL fSuccess;
        do {
            fSuccess = ::ReadFile(
                pipe_,
                chBuf,
                512 * sizeof(char),
                &cbRead, nullptr);

            if (!fSuccess) {
                auto err = ::GetLastError();
                if (err != ERROR_MORE_DATA) {
                    break;
                }
            }
            buf->append(chBuf, cbRead);
        } while (!fSuccess);

        if (!fSuccess) {
            return false;
        }

        return true;
    }

    bool PipeServerWin::write(const std::string_view& buf) {
        if (pipe_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD cbRet = 0;
        BOOL fSuccess = ::WriteFile(
            pipe_,
            buf.data(),
            utl::num_cast<DWORD>(buf.size()),
            &cbRet, nullptr);

        // The write operation completed successfully.
        if (!fSuccess) {
            return false;
        }

        return false;
    }

    bool PipeServerWin::disconnect() {
        if (!::DisconnectNamedPipe(pipe_)) {
            LOG(Log::ERR) << "DisconnectNamedPipe failed with " << ::GetLastError();
            return false;
        }
        return true;
    }

    void PipeServerWin::destroy() {
        if (pipe_ != INVALID_HANDLE_VALUE) {
            ::CloseHandle(pipe_);
            pipe_ = INVALID_HANDLE_VALUE;
        }
    }

}
}