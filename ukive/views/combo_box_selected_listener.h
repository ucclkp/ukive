// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_COMBO_BOX_SELECTED_LISTENER_H_
#define UKIVE_VIEWS_COMBO_BOX_SELECTED_LISTENER_H_


namespace ukive {

    class ComboBox;

    class ComboBoxSelectedListener {
    public:
        virtual ~ComboBoxSelectedListener() = default;

        virtual void onComboItemSelected(ComboBox* cb) = 0;
    };

}

#endif  // UKIVE_VIEWS_COMBO_BOX_SELECTED_LISTENER_H_