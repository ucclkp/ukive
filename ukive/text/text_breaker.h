// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_TEXT_BREAKER_H_
#define UKIVE_TEXT_TEXT_BREAKER_H_

#include <string>


namespace ukive {

    class CharacterBreaker {
    public:
        explicit CharacterBreaker(const std::u16string* text);

        void setCur(size_t cur_pos);

        bool next();
        bool prev();

        size_t getCur() const { return cur_; }
        size_t getPrev() const { return prev_; }

    private:
        size_t cur_;
        size_t prev_;
        const std::u16string* text_;
    };

}

#endif  // UKIVE_TEXT_TEXT_BREAKER_H_