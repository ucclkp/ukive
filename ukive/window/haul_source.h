// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_HAUL_SOURCE_H_
#define UKIVE_WINDOW_HAUL_SOURCE_H_

#include <string>

#include "ukive/graphics/point.hpp"


namespace ukive {

    class View;
    class Window;
    class InputEvent;
    class HaulDelegate;

    class HaulSource {
    public:
        enum DataType {
            DT_NONE,
            DT_URL,
            DT_TEXT,
            DT_BYTES,
            DT_EX_DATA,
        };

        struct Data {
            DataType type;
            std::string exd;
            std::string raw;
        };

        HaulSource(int id, HaulDelegate* d);
        virtual ~HaulSource() = default;

        void ignite(View* v, InputEvent* e);
        bool brake(InputEvent* e);
        void cancel();

        void setURL(const std::string_view& url);
        void setText(const std::string_view& text);
        void setBytes(const std::string_view& bytes);
        void setExData(
            const std::string_view& exid,
            const std::string_view& data);

        const std::string& getRaw() const;

        bool isURL() const;
        bool isText() const;
        bool isBytes() const;
        bool isExData(const std::string_view& exd) const;

        int getSourceId() const;

    private:
        void stop();

        int id_;
        HaulDelegate* delegate_;
        Window* window_ = nullptr;

        Point start_pos_{};
        bool is_hauling_ = false;

        Data data_;
    };

}

#endif  // UKIVE_WINDOW_HAUL_SOURCE_H_