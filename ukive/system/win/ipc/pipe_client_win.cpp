// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "pipe_client_win.h"

#include "utils/log.h"


namespace ukive {

    PipeClientWin::PipeClientWin()
        : pipe_(INVALID_HANDLE_VALUE) {}

    PipeClientWin::~PipeClientWin() {
        if (pipe_ != INVALID_HANDLE_VALUE) {
            ::CloseHandle(pipe_);
        }
    }

    bool PipeClientWin::connect(const std::u16string& name) {
        if (pipe_ != INVALID_HANDLE_VALUE) {
            return false;
        }

        LPCWSTR pipe_name = reinterpret_cast<const wchar_t*>(name.c_str());

        for (;;) {
            pipe_ = ::CreateFile(
                pipe_name,      // pipe name
                GENERIC_READ |  // read and write access
                GENERIC_WRITE,
                0,              // no sharing
                nullptr,        // default security attributes
                OPEN_EXISTING,  // opens existing pipe
                0,              // default attributes
                nullptr);       // no template file

            // Break if the pipe handle is valid.
            if (pipe_ != INVALID_HANDLE_VALUE) {
                break;
            }

            // Exit if an error other than ERROR_PIPE_BUSY occurs.
            if (::GetLastError() != ERROR_PIPE_BUSY) {
                LOG(Log::ERR) << "Could not open pipe. GLE=" << ::GetLastError();
                return false;
            }

            // All pipe instances are busy, so wait for 20 seconds.
            if (!::WaitNamedPipe(pipe_name, 20000)) {
                LOG(Log::ERR) << "Could not open pipe: 20 second wait timed out.";
                return false;
            }
        }

        DWORD dwMode = PIPE_READMODE_BYTE;
        BOOL fSuccess = ::SetNamedPipeHandleState(
            pipe_,    // pipe handle
            &dwMode,  // new pipe mode
            nullptr,  // don't set maximum bytes
            nullptr); // don't set maximum time
        if (!fSuccess) {
            disconnect();
            LOG(Log::ERR) << "Could not set pipe state, GLE=" << ::GetLastError();
            return false;
        }
        return true;
    }

    void PipeClientWin::disconnect() {
        if (pipe_ != INVALID_HANDLE_VALUE) {
            ::CloseHandle(pipe_);
            pipe_ = INVALID_HANDLE_VALUE;
        }
    }

    bool PipeClientWin::read(std::string* buf) {
        if (pipe_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        char chBuf[512];
        DWORD cbRead = 0;
        BOOL fSuccess;
        do {
            // Read from the pipe.
            fSuccess = ::ReadFile(
                pipe_,                  // pipe handle
                chBuf,                  // buffer to receive reply
                512 * sizeof(char),  // size of buffer
                &cbRead,                // number of bytes read
                nullptr);               // not overlapped

            if (!fSuccess && ::GetLastError() != ERROR_MORE_DATA) {
                break;
            }
            buf->append(chBuf, cbRead);
        } while (!fSuccess);  // repeat loop if ERROR_MORE_DATA

        if (!fSuccess) {
            LOG(Log::ERR) << "ReadFile from pipe failed. GLE=" << ::GetLastError();
            return false;
        }
        return true;
    }

    bool PipeClientWin::write(const std::string_view& buf) {
        ubassert(buf.size() <= std::numeric_limits<DWORD>::max());

        if (pipe_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD cbWritten = 0;
        BOOL fSuccess = ::WriteFile(
            pipe_,                  // pipe handle
            buf.data(),             // message
            DWORD(buf.size()),             // message length
            &cbWritten,             // bytes written
            nullptr);               // not overlapped

        if (!fSuccess) {
            LOG(Log::ERR) << "WriteFile to pipe failed. GLE=" << ::GetLastError();
            return false;
        }
        return true;
    }

}
