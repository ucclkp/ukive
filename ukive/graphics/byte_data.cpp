// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "byte_data.h"

#include "ukive/graphics/gref_count_impl.h"


namespace ukive {

    class ByteData_String :
        public ByteData,
        public GRefCountImpl
    {
    public:
        explicit ByteData_String(std::string&& raw)
            : raw_(std::move(raw)) {}
        explicit ByteData_String(const std::string& raw)
            : raw_(raw) {}

        Type getType() const override { return Type::String; }
        void* getData() override { return raw_.data(); }
        const void* getData() const override { return raw_.data(); }
        size_t getSize() const override { return raw_.size(); }

    private:
        std::string raw_;
    };

    template <typename Ty>
    class ByteData_Vector :
        public ByteData,
        public GRefCountImpl
    {
    public:
        explicit ByteData_Vector(std::vector<Ty>&& raw)
            : raw_(std::move(raw)) {}
        explicit ByteData_Vector(const std::vector<Ty>& raw)
            : raw_(raw) {}

        Type getType() const override { return Type::Vector; }
        void* getData() override { return raw_.data(); }
        const void* getData() const override { return raw_.data(); }
        size_t getSize() const override { return raw_.size(); }

    private:
        std::vector<Ty> raw_;
    };

    template <typename Ty>
    class ByteData_Unique :
        public ByteData,
        public GRefCountImpl
    {
    public:
        explicit ByteData_Unique(std::unique_ptr<Ty[]>&& raw, size_t len)
            : raw_(std::move(raw)),
            len_(len) {}

        Type getType() const override { return Type::Unique; }
        void* getData() override { return raw_.get(); }
        const void* getData() const override { return raw_.get(); }
        size_t getSize() const override { return len_; }

    private:
        std::unique_ptr<Ty[]> raw_;
        size_t len_;
    };

    template <typename Ty>
    class ByteData_Pointer :
        public ByteData,
        public GRefCountImpl
    {
    public:
        explicit ByteData_Pointer(Ty* raw, size_t len)
            : raw_(raw),
            len_(len) {}
        ~ByteData_Pointer() {
            delete[] raw_;
        }

        Type getType() const override { return Type::Pointer; }
        void* getData() override { return raw_; }
        const void* getData() const override { return raw_; }
        size_t getSize() const override { return len_; }

    private:
        Ty* raw_;
        size_t len_;
    };


    // static
    GPtr<ByteData> ByteData::ownStr(std::string&& raw) {
        return GPtr<ByteData>(new ByteData_String(raw));
    }

    // static
    GPtr<ByteData> ByteData::ownVec(std::vector<char>&& raw) {
        return GPtr<ByteData>(new ByteData_Vector(raw));
    }

    // static
    GPtr<ByteData> ByteData::ownVec(std::vector<unsigned char>&& raw) {
        return GPtr<ByteData>(new ByteData_Vector(raw));
    }

    // static
    GPtr<ByteData> ByteData::ownUni(std::unique_ptr<char[]>&& raw, size_t len) {
        return GPtr<ByteData>(
            new ByteData_Unique(
                std::forward<std::unique_ptr<char[]>>(raw), len));
    }

    // static
    GPtr<ByteData> ByteData::ownUni(std::unique_ptr<unsigned char[]>&& raw, size_t len) {
        return GPtr<ByteData>(
            new ByteData_Unique(
                std::forward<std::unique_ptr<unsigned char[]>>(raw), len));
    }

    // static
    GPtr<ByteData> ByteData::ownPtr(char* raw, size_t len) {
        return GPtr<ByteData>(new ByteData_Pointer(raw, len));
    }

    // static
    GPtr<ByteData> ByteData::ownPtr(unsigned char* raw, size_t len) {
        return GPtr<ByteData>(new ByteData_Pointer(raw, len));
    }

    // static
    GPtr<ByteData> ByteData::copyStr(const std::string& raw) {
        return GPtr<ByteData>(new ByteData_String(raw));
    }

    // static
    GPtr<ByteData> ByteData::copyVec(const std::vector<char>& raw) {
        return GPtr<ByteData>(new ByteData_Vector(raw));
    }

    // static
    GPtr<ByteData> ByteData::copyVec(const std::vector<unsigned char>& raw) {
        return GPtr<ByteData>(new ByteData_Vector(raw));
    }

    // static
    GPtr<ByteData> ByteData::copyPtr(const char* raw, size_t len) {
        auto raw2 = new char[len];
        std::memcpy(raw2, raw, len);
        return GPtr<ByteData>(new ByteData_Pointer(raw2, len));
    }

    // static
    GPtr<ByteData> ByteData::copyPtr(const unsigned char* raw, size_t len) {
        auto raw2 = new unsigned char[len];
        std::memcpy(raw2, raw, len);
        return GPtr<ByteData>(new ByteData_Pointer(raw2, len));
    }

}
