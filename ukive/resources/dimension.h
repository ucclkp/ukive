// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_RESOURCES_DIMENSION_H_
#define UKIVE_RESOURCES_DIMENSION_H_


namespace ukive {

    class Context;

    class Dimension {
    public:
        enum Type {
            PIXEL,
            DIP,
        };

        Dimension();
        explicit Dimension(int px);
        explicit Dimension(float dp);

        Dimension& operator=(int px);
        Dimension& operator=(float dp);

        void setPx(int px);
        void setDp(float dp);

        int toPxX(const Context& c) const;
        int toPxY(const Context& c) const;

        bool isDp() const;
        bool isPx() const;

        int px;
        float dp;
        Type type;
    };

    using DimCRef = const Dimension&;

}

#endif  // UKIVE_RESOURCES_DIMENSION_H_