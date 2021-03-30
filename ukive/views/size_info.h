// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_SIZE_INFO_H_
#define UKIVE_VIEWS_SIZE_INFO_H_


namespace ukive {

    class SizeInfo {
    public:
        enum Mode {
            CONTENT,
            DEFINED,
            FREEDOM,
        };

        struct Value {
            int val;
            Mode mode;

            Value()
                : val(0), mode(CONTENT) {}

            Value(int val, Mode mode)
                : val(val), mode(mode) {}

            bool operator==(const Value& rhs) const {
                return val == rhs.val && mode == rhs.mode;
            }

            void set(int val, Mode mode) {
                this->val = val;
                this->mode = mode;
            }
        };

        SizeInfo();
        SizeInfo(const Value& w, const Value& h);

        bool operator==(const SizeInfo& rhs) const;

        void set(const Value& w, const Value& h);
        void setWidth(const Value& w);
        void setHeight(const Value& h);

        static Value getChildSizeInfo(
            const Value& parent_val, int inset, int child_layout_size);

        Value width;
        Value height;
    };

}

#endif  // UKIVE_VIEWS_SIZE_INFO_H_