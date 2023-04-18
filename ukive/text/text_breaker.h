// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_TEXT_BREAKER_H_
#define UKIVE_TEXT_TEXT_BREAKER_H_

#include <string>


namespace ukive {

    class TextBreaker {
    public:
        explicit TextBreaker(const std::u16string* text);

        void setCur(size_t cur_pos);

        virtual bool next() = 0;
        virtual bool prev() = 0;

        size_t getCur() const { return cur_; }
        size_t getPrev() const { return prev_; }

    protected:
        size_t cur_;
        size_t prev_;
        const std::u16string* text_;
    };


    class CharacterBreaker : public TextBreaker {
    public:
        explicit CharacterBreaker(const std::u16string* text);

        bool next() override;
        bool prev() override;
    };


    class WordBreaker : public TextBreaker {
    public:
        explicit WordBreaker(const std::u16string* text);

        bool next() override;
        bool prev() override;
    };


    class ParagraphBreaker : public TextBreaker {
    public:
        explicit ParagraphBreaker(const std::u16string* text);

        bool next() override;
        bool prev() override;
    };

}

#endif  // UKIVE_TEXT_TEXT_BREAKER_H_