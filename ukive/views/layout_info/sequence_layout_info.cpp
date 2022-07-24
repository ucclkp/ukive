// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "sequence_layout_info.h"


namespace ukive {

    // static
    SequenceLayoutInfo* SequenceLayoutInfo::ofWeight(int w) {
        return new SequenceLayoutInfo(w, Align::START, Align::START);
    }

    // static
    SequenceLayoutInfo* SequenceLayoutInfo::ofVertAlign(Align a) {
        return new SequenceLayoutInfo(0, a, Align::START);
    }

    // static
    SequenceLayoutInfo* SequenceLayoutInfo::ofHoriAlign(Align a) {
        return new SequenceLayoutInfo(0, Align::START, a);
    }

    SequenceLayoutInfo::SequenceLayoutInfo()
        : LayoutInfo(),
          weight(0),
          vert_align(Align::START),
          hori_align(Align::START) {}

    SequenceLayoutInfo::SequenceLayoutInfo(int w, Align v, Align h)
        : LayoutInfo(),
          weight(w),
          vert_align(v),
          hori_align(h) {}

}