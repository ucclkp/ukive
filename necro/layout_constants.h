// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of necro project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef NECRO_LAYOUT_CONSTANTS_H_
#define NECRO_LAYOUT_CONSTANTS_H_


namespace necro {

    const char16_t kLayoutIdFileName[] = u"necro_layout_id";
    const char16_t kLayoutHistoryFileName[] = u"necro_layout_histories";

    // View
    const char kAttrViewId[] = "id";
    const char kAttrViewPadding[] = "padding";
    const char kAttrViewShadowRadius[] = "shadow_radius";
    const char kAttrViewClickable[] = "clickable";

    // TextView
    const char kAttrTextViewText[] = "text";
    const char kAttrTextViewTextSize[] = "text_size";
    const char kAttrTextViewAutoWrap[] = "auto_wrap";
    const char kAttrTextViewIsSelectable[] = "is_selectable";
    const char kAttrTextViewIsEditable[] = "is_editable";

    // SequenceLayout (View)
    const char kAttrSeqLayoutViewOri[] = "orientation";
    const char kAttrValSeqLayoutViewOriVert[] = "vertical";
    const char kAttrValSeqLayoutViewOriHori[] = "horizontal";

    // LayoutView
    const char kAttrLayoutWidth[] = "width";
    const char kAttrLayoutHeight[] = "height";
    const char kAttrLayoutMargin[] = "margin";

    const char kAttrValLayoutAuto[] = "auto";
    const char kAttrValLayoutFill[] = "fill";
    const char kAttrValLayoutFree[] = "free";

    // SequenceLayout
    const char kAttrSeqLayoutWeight[] = "weight";
    const char kAttrSeqLayoutVertPos[] = "vertical_pos";
    const char kAttrSeqLayoutHoriPos[] = "horizontal_pos";

    const char kAttrValSeqLayoutPosStart[] = "start";
    const char kAttrValSeqLayoutPosCenter[] = "center";
    const char kAttrValSeqLayoutPosEnd[] = "end";

    // RestraintLayout
    const char kAttrRestraintLayoutSH[] = "handle_start";
    const char kAttrRestraintLayoutEH[] = "handle_end";
    const char kAttrRestraintLayoutTH[] = "handle_top";
    const char kAttrRestraintLayoutBH[] = "handle_bottom";

    const char kAttrValRestraintLayoutHEStart[] = "start";
    const char kAttrValRestraintLayoutHEEnd[] = "end";
    const char kAttrValRestraintLayoutHETop[] = "top";
    const char kAttrValRestraintLayoutHEBottom[] = "bottom";


}

#endif  // NECRO_LAYOUT_CONSTANTS_H_