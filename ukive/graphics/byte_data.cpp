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

        GPtr<ByteData> substantiate() override {
            GPtr<ByteData> out(this);
            out->add_ref();
            return out;
        }

        bool isRef() const override { return false; }
        Type getType() const override { return Type::String; }
        void* getData() override { return raw_.data(); }
        const void* getConstData() const override { return raw_.data(); }
        size_t getSize() const override { return raw_.size(); }

    private:
        std::string raw_;
    };

    class ByteData_StringRef :
        public ByteData,
        public GRefCountImpl
    {
    public:
        explicit ByteData_StringRef(const std::string& raw)
            : raw_(raw) {}

        GPtr<ByteData> substantiate() override {
            return GPtr<ByteData>(new ByteData_String(raw_));
        }

        bool isRef() const override { return true; }
        Type getType() const override { return Type::String; }
        void* getData() override { return nullptr; }
        const void* getConstData() const override { return raw_.data(); }
        size_t getSize() const override { return raw_.size(); }

    private:
        const std::string& raw_;
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

        GPtr<ByteData> substantiate() override {
            GPtr<ByteData> out(this);
            out->add_ref();
            return out;
        }

        bool isRef() const override { return false; }
        Type getType() const override { return Type::Vector; }
        void* getData() override { return raw_.data(); }
        const void* getConstData() const override { return raw_.data(); }
        size_t getSize() const override { return raw_.size(); }

    private:
        std::vector<Ty> raw_;
    };

    template <typename Ty>
    class ByteData_VectorRef :
        public ByteData,
        public GRefCountImpl
    {
    public:
        explicit ByteData_VectorRef(const std::vector<Ty>& raw)
            : raw_(raw) {}

        GPtr<ByteData> substantiate() override {
            return GPtr<ByteData>(new ByteData_Vector(raw_));
        }

        bool isRef() const override { return true; }
        Type getType() const override { return Type::Vector; }
        void* getData() override { return nullptr; }
        const void* getConstData() const override { return raw_.data(); }
        size_t getSize() const override { return raw_.size(); }

    private:
        const std::vector<Ty>& raw_;
    };

    template <typename Ty>
    class ByteData_Unique :
        public ByteData,
        public GRefCountImpl
    {
    public:
        ByteData_Unique(std::unique_ptr<Ty[]>&& raw, size_t len)
            : raw_(std::move(raw)),
              len_(len) {}

        GPtr<ByteData> substantiate() override {
            GPtr<ByteData> out(this);
            out->add_ref();
            return out;
        }

        bool isRef() const override { return false; }
        Type getType() const override { return Type::Unique; }
        void* getData() override { return raw_.get(); }
        const void* getConstData() const override { return raw_.get(); }
        size_t getSize() const override { return len_; }

    private:
        std::unique_ptr<Ty[]> raw_;
        size_t len_;
    };

    template <typename Ty>
    class ByteData_UniqueRef :
        public ByteData,
        public GRefCountImpl
    {
    public:
        ByteData_UniqueRef(const std::unique_ptr<Ty[]>& raw, size_t len)
            : raw_(raw),
              len_(len) {}

        GPtr<ByteData> substantiate() override {
            if (raw_ && len_) {
                std::unique_ptr<Ty[]> raw2(new Ty[len_]);
                std::memcpy(raw2.get(), raw_.get(), len_);
                return GPtr<ByteData>(new ByteData_Unique(std::move(raw2), len_));
            }
            return GPtr<ByteData>(new ByteData_Unique<Ty>({}, 0));
        }

        bool isRef() const override { return true; }
        Type getType() const override { return Type::Unique; }
        void* getData() override { return nullptr; }
        const void* getConstData() const override { return raw_.get(); }
        size_t getSize() const override { return len_; }

    private:
        const std::unique_ptr<Ty[]>& raw_;
        size_t len_;
    };

    template <typename Ty>
    class ByteData_Pointer :
        public ByteData,
        public GRefCountImpl
    {
    public:
        ByteData_Pointer(Ty* raw, size_t len)
            : raw_(raw),
              len_(len) {}
        ~ByteData_Pointer() {
            delete[] raw_;
        }

        GPtr<ByteData> substantiate() override {
            GPtr<ByteData> out(this);
            out->add_ref();
            return out;
        }

        bool isRef() const override { return false; }
        Type getType() const override { return Type::Pointer; }
        void* getData() override { return raw_; }
        const void* getConstData() const override { return raw_; }
        size_t getSize() const override { return len_; }

    private:
        Ty* raw_;
        size_t len_;
    };

    template <typename Ty>
    class ByteData_PointerRef :
        public ByteData,
        public GRefCountImpl
    {
    public:
        ByteData_PointerRef(const Ty* raw, size_t len)
            : raw_(raw),
              len_(len) {}
        ~ByteData_PointerRef() {}

        GPtr<ByteData> substantiate() override {
            if (raw_ && len_) {
                auto raw2 = new Ty[len_];
                std::memcpy(raw2, raw_, len_);
                return GPtr<ByteData>(new ByteData_Pointer(raw2, len_));
            }
            return GPtr<ByteData>(new ByteData_Pointer<Ty>(nullptr, 0));
        }

        bool isRef() const override { return true; }
        Type getType() const override { return Type::Pointer; }
        void* getData() override { return nullptr; }
        const void* getConstData() const override { return raw_; }
        size_t getSize() const override { return len_; }

    private:
        const Ty* raw_;
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
    GPtr<ByteData> ByteData::ownPtr(void* raw, size_t len) {
        return GPtr<ByteData>(
            new ByteData_Pointer(
                static_cast<unsigned char*>(raw), len));
    }

    // static
    GPtr<ByteData> ByteData::refStr(const std::string& raw) {
        return GPtr<ByteData>(new ByteData_StringRef(raw));
    }

    // static
    GPtr<ByteData> ByteData::refVec(const std::vector<char>& raw) {
        return GPtr<ByteData>(new ByteData_VectorRef(raw));
    }

    // static
    GPtr<ByteData> ByteData::refVec(const std::vector<unsigned char>& raw) {
        return GPtr<ByteData>(new ByteData_VectorRef(raw));
    }

    // static
    GPtr<ByteData> ByteData::refUni(const std::unique_ptr<char[]>& raw, size_t len) {
        return GPtr<ByteData>(new ByteData_UniqueRef(raw, len));
    }

    // static
    GPtr<ByteData> ByteData::refUni(const std::unique_ptr<unsigned char[]>& raw, size_t len) {
        return GPtr<ByteData>(new ByteData_UniqueRef(raw, len));
    }

    // static
    GPtr<ByteData> ByteData::refPtr(const void* raw, size_t len) {
        return GPtr<ByteData>(
            new ByteData_PointerRef(
                static_cast<const unsigned char*>(raw), len));
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
    GPtr<ByteData> ByteData::copyUni(const std::unique_ptr<char[]>& raw, size_t len) {
        if (raw && len) {
            std::unique_ptr<char[]> raw2(new char[len]);
            std::memcpy(raw2.get(), raw.get(), len);
            return GPtr<ByteData>(new ByteData_Unique(std::move(raw2), len));
        }
        return GPtr<ByteData>(new ByteData_Unique<char>({}, 0));
    }

    // static
    GPtr<ByteData> ByteData::copyUni(const std::unique_ptr<unsigned char[]>& raw, size_t len) {
        if (raw && len) {
            std::unique_ptr<unsigned char[]> raw2(new unsigned char[len]);
            std::memcpy(raw2.get(), raw.get(), len);
            return GPtr<ByteData>(new ByteData_Unique(std::move(raw2), len));
        }
        return GPtr<ByteData>(new ByteData_Unique<unsigned char>({}, 0));
    }

    // static
    GPtr<ByteData> ByteData::copyPtr(const void* raw, size_t len) {
        if (raw && len) {
            auto raw2 = new unsigned char[len];
            std::memcpy(raw2, raw, len);
            return GPtr<ByteData>(new ByteData_Pointer(raw2, len));
        }
        return GPtr<ByteData>(new ByteData_Pointer<unsigned char>(nullptr, 0));
    }

}
