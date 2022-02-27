// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_DIALOGS_MESSAGE_DIALOG_H_
#define UKIVE_DIALOGS_MESSAGE_DIALOG_H_

#include "ukive/dialogs/dialog.h"
#include "ukive/views/click_listener.h"


namespace ukive {

    class Button;
    class CircleColorButton;
    class TextView;

    class MessageDialog :
        public Dialog,
        public OnClickListener
    {
    public:
        explicit MessageDialog(Context c);

        static void showDialog(
            Window* parent,
            const std::u16string_view& title,
            const std::u16string_view& text,
            const std::u16string_view& btn_text);

        void setTitle(const std::u16string_view& title);
        void setText(const std::u16string_view& text);
        void setButtonText(const std::u16string_view& text);

    protected:
        // Dialog
        View* onCreate(Context c) override;
        void onDestroy() override;

        // OnClickListener
        void onClick(View* v) override;

    private:
        std::u16string title_;
        std::u16string text_;
        std::u16string btn_text_;

        TextView* title_tv_ = nullptr;
        TextView* text_tv_ = nullptr;
        Button* btn_ = nullptr;
        CircleColorButton* close_btn_ = nullptr;
    };

}

#endif  // UKIVE_DIALOGS_MESSAGE_DIALOG_H_