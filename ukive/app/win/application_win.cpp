// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/app/application.h"

#include <Windows.h>

#include "utils/convert.h"
#include "utils/log.h"


namespace {

    LPTOP_LEVEL_EXCEPTION_FILTER prev_unhandled_filter = nullptr;
    PVOID cur_vectored_filter = nullptr;
    _purecall_handler prev_purecall_handler = nullptr;
    _invalid_parameter_handler prev_invalid_param_handler = nullptr;

}

namespace ukive {

    LONG WINAPI handleUnhandledException(EXCEPTION_POINTERS* info) {
        if (info->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT ||
            info->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
        {
            if (prev_unhandled_filter) {
                return prev_unhandled_filter(info);
            }
            return EXCEPTION_CONTINUE_SEARCH;
        }

        LOG(Log::ERR) << "App crashed!!! Unhandled exception: " << info->ExceptionRecord->ExceptionCode;
        if (prev_unhandled_filter) {
            return prev_unhandled_filter(info);
        }
        return EXCEPTION_EXECUTE_HANDLER;
    }

    LONG WINAPI handleVectoredException(EXCEPTION_POINTERS* info) {
        // 堆损坏
        if (info->ExceptionRecord->ExceptionCode == 0xC0000374) {
            LOG(Log::ERR) << "App crashed!!! Vectored exception: " << info->ExceptionRecord->ExceptionCode;
        }

        return EXCEPTION_CONTINUE_SEARCH;
    }

    void handleInvalidParameter(
        const wchar_t* expression,
        const wchar_t* function,
        const wchar_t* file,
        unsigned int line,
        uintptr_t reserved)
    {
        LOG(Log::ERR) << "ERROR: Invalid parameter: "
            << utl::UTF16ToUTF8(std::u16string_view(reinterpret_cast<const char16_t*>(expression))) << " "
            << utl::UTF16ToUTF8(std::u16string_view(reinterpret_cast<const char16_t*>(function))) << " "
            << utl::UTF16ToUTF8(std::u16string_view(reinterpret_cast<const char16_t*>(file))) << " "
            << std::to_string(line);
        abort();
    }

    void handlePureCall() {
        LOG(Log::ERR) << "ERROR: Pure call!!!";
        abort();
    }


    void Application::initPlatform() {
        ::SetDllDirectoryW(L"");

        prev_unhandled_filter = ::SetUnhandledExceptionFilter(handleUnhandledException);
        cur_vectored_filter = ::AddVectoredExceptionHandler(1, handleVectoredException);
        prev_purecall_handler = _set_purecall_handler(handlePureCall);
        prev_invalid_param_handler = _set_invalid_parameter_handler(handleInvalidParameter);

        HRESULT hr = ::CoInitialize(nullptr);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to initialize COM";
        }
    }

    void Application::cleanPlatform() {
        ::CoUninitialize();

        ::SetUnhandledExceptionFilter(prev_unhandled_filter);
        ::RemoveVectoredExceptionHandler(cur_vectored_filter);
        _set_purecall_handler(prev_purecall_handler);
        _set_invalid_parameter_handler(prev_invalid_param_handler);

        prev_unhandled_filter = nullptr;
        cur_vectored_filter = nullptr;
        prev_purecall_handler = nullptr;
        prev_invalid_param_handler = nullptr;
    }

}