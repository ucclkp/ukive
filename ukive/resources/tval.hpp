// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_RESOURCES_TVAL_HPP_
#define UKIVE_RESOURCES_TVAL_HPP_

#include <assert.h>


namespace ukive {

    class tval {
    public:
        enum class vt {
            none,
            absolute,
            width,
            height,
            diagonal,
            automatic,
        };

        tval()
            : type_(vt::none) {}

        static tval ofReal(double r)   { tval tv; tv.absolute(r);  return tv; }
        static tval ofWidth(double r)  { tval tv; tv.width(r);     return tv; }
        static tval ofHeight(double r) { tval tv; tv.height(r);    return tv; }
        static tval ofDiag(double r)   { tval tv; tv.diagonal(r);  return tv; }
        static tval ofAuto(int u = 0)  { tval tv; tv.automatic(u); return tv; }

        void absolute(double v)   { type_ = vt::absolute;  d_val_ = v; }
        void width(double v)      { type_ = vt::width;     d_val_ = v; }
        void height(double v)     { type_ = vt::height;    d_val_ = v; }
        void diagonal(double v)   { type_ = vt::diagonal;  d_val_ = v; }
        void automatic(int u = 0) { type_ = vt::automatic; i_val_ = u; }

        double absolute() const { assert(type_ == vt::absolute);  return d_val_; }
        double width() const    { assert(type_ == vt::width);     return d_val_; }
        double height() const   { assert(type_ == vt::height);    return d_val_; }
        double diagonal() const { assert(type_ == vt::diagonal);  return d_val_; }
        int automatic() const   { assert(type_ == vt::automatic); return i_val_; }

        vt type() const { return type_; }

    private:
        vt type_;
        union {
            int i_val_;
            double d_val_;
        };
    };

    using tvalcr = const tval&;

}

#endif  // UKIVE_RESOURCES_TVAL_HPP_