// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_BYTE_DATA_H_
#define UKIVE_GRAPHICS_BYTE_DATA_H_

#include <memory>
#include <string>
#include <vector>

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gref_count.h"


namespace ukive {

    class ByteData : public virtual GRefCount {
    public:
        enum class Type {
            String,
            Vector,
            Unique,
            Pointer,
        };

        static GPtr<ByteData> ownStr(std::string&& raw);
        static GPtr<ByteData> ownVec(std::vector<char>&& raw);
        static GPtr<ByteData> ownVec(std::vector<unsigned char>&& raw);
        static GPtr<ByteData> ownUni(std::unique_ptr<char[]>&& raw, size_t len);
        static GPtr<ByteData> ownUni(std::unique_ptr<unsigned char[]>&& raw, size_t len);
        static GPtr<ByteData> ownPtr(void* raw, size_t len);

        static GPtr<ByteData> refStr(const std::string& raw);
        static GPtr<ByteData> refVec(const std::vector<char>& raw);
        static GPtr<ByteData> refVec(const std::vector<unsigned char>& raw);
        static GPtr<ByteData> refUni(const std::unique_ptr<char[]>& raw, size_t len);
        static GPtr<ByteData> refUni(const std::unique_ptr<unsigned char[]>& raw, size_t len);
        static GPtr<ByteData> refPtr(const void* raw, size_t len);

        static GPtr<ByteData> copyStr(const std::string& raw);
        static GPtr<ByteData> copyVec(const std::vector<char>& raw);
        static GPtr<ByteData> copyVec(const std::vector<unsigned char>& raw);
        static GPtr<ByteData> copyUni(const std::unique_ptr<char[]>& raw, size_t len);
        static GPtr<ByteData> copyUni(const std::unique_ptr<unsigned char[]>& raw, size_t len);
        static GPtr<ByteData> copyPtr(const void* raw, size_t len);

        virtual ~ByteData() = default;

        virtual GPtr<ByteData> substantiate() = 0;

        virtual bool isRef() const = 0;

        virtual Type getType() const = 0;
        virtual void* getData() = 0;
        virtual const void* getConstData() const = 0;
        virtual size_t getSize() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_BYTE_DATA_H_