#ifndef UTILS_MSTREAM_HPP_
#define UTILS_MSTREAM_HPP_

#include <istream>


namespace utl {

    template <class _Elem, class _Traits, class _Alloc>
    class basic_memorybuf : public std::basic_streambuf<_Elem, _Traits> {
    public:
        basic_memorybuf(const _Elem* buffer, size_t size)
            : buf_(buffer),
              size_(size)
        {
            std::istringstream;
            this->setg((_Elem*)buffer, (_Elem*)buffer, (_Elem*)buffer + size);
        }

        pos_type __CLR_OR_THIS_CALL seekoff(
            off_type offset,
            std::ios_base::seekdir dir,
            std::ios_base::openmode mode) override
        {
            switch (dir) {
            case std::ios_base::beg:
                if (offset < 0 || offset > size_) {
                    return pos_type{ off_type{-1} };
                }
                this->setg((_Elem*)buf_, (_Elem*)buf_ + offset, (_Elem*)buf_ + size_);
                return pos_type{ offset };

            case std::ios_base::cur:
                if (gptr() - eback() + offset < 0 || gptr() - eback() + offset > size_) {
                    return pos_type{ off_type{-1} };
                }
                gbump(static_cast<int>(offset));
                return pos_type{ gptr() - eback() };

            case std::ios_base::end:
                if (offset > 0 || size_ + offset < 0) {
                    return pos_type{ off_type{-1} };
                }
                this->setg((_Elem*)buf_, (_Elem*)buf_ + size_ + offset, (_Elem*)buf_ + size_);
                return pos_type(size_ + offset);

            default:
                return pos_type{ off_type{-1} };
            }
        }

        pos_type __CLR_OR_THIS_CALL seekpos(
            pos_type pos,
            std::ios_base::openmode mode) override
        {
            if (pos < 0 || pos > size_) {
                return pos_type{ off_type{-1} };
            }
            this->setg((_Elem*)buf_, (_Elem*)buf_ + pos, (_Elem*)buf_ + size_);
            return pos;
        }

    private:
        const _Elem* buf_;
        size_t size_;
    };

    template <class _Elem, class _Traits, class _Alloc>
    class basic_imemorystream : public std::basic_istream<_Elem, _Traits> {
    public:
        using super = std::basic_istream<_Elem, _Traits>;

        basic_imemorystream(const _Elem* buffer, size_t size)
            : super(&_Buffer),
              _Buffer(buffer, size)
        {
            this->rdbuf(&_Buffer);
        }

    private:
        basic_memorybuf<_Elem, _Traits, _Alloc> _Buffer;
    };

    using imemorystream = basic_imemorystream<char, std::char_traits<char>, std::allocator<char>>;

}

#endif  // UTILS_MSTREAM_HPP_