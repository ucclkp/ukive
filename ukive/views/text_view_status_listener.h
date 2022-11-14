// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TEXT_VIEW_STATUS_LISTENER_H_
#define UKIVE_VIEWS_TEXT_VIEW_STATUS_LISTENER_H_

#include "ukive/text/editable.h"
#include "ukive/text/range.hpp"
#include "ukive/text/selection.hpp"


namespace ukive {

    class TextView;

    class TextViewStatusListener {
    public:
        virtual ~TextViewStatusListener() = default;

        virtual void onBeforeTextChanged(
            TextView* v,
            const RangeChg& chg,
            Editable::Reason r) {}
        virtual void onBeforeSelectionChanged(
            TextView* v,
            const Selection& nsel,
            const Selection& osel,
            Editable::Reason r) {}
        virtual void onBeforeSpanChanged(
            TextView* v,
            Span* span,
            Editable::EditWatcher::SpanChange action,
            Editable::Reason r) {}

        virtual void onAfterTextChanged(
            TextView* v,
            const RangeChg& chg,
            Editable::Reason r) {}
        virtual void onAfterSelectionChanged(
            TextView* v,
            const Selection& nsel,
            const Selection& osel,
            Editable::Reason r) {}
        virtual void onAfterSpanChanged(
            TextView* v,
            Span* span,
            Editable::EditWatcher::SpanChange action,
            Editable::Reason r) {}
    };

}

#endif  // UKIVE_VIEWS_TEXT_VIEW_STATUS_LISTENER_H_