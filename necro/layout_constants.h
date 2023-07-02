// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of necro project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef NECRO_LAYOUT_CONSTANTS_H_
#define NECRO_LAYOUT_CONSTANTS_H_


namespace necro {

    const long long kXmlLayoutIdStart = 100ll;

    const char16_t kLayoutIdFileName[] = u"necro_layout_id";
    const char16_t kLayoutHistoryFileName[] = u"necro_layout_histories";

    // View
    const char kAttrViewId[] = "id";
    const char kAttrViewPadding[] = "padding";
    const char kAttrViewVisibility[] = "visibility";
    const char kAttrViewShadowRadius[] = "shadow_radius";
    const char kAttrViewClickable[] = "clickable";
    const char kAttrViewDoubleClickable[] = "double_clickable";
    const char kAttrViewFocusable[] = "focusable";
    const char kAttrViewTouchCapturable[] = "touch_capturable";
    const char kAttrViewEnabled[] = "enabled";
    const char kAttrViewMinWidth[] = "min_width";
    const char kAttrViewMinHeight[] = "min_height";
    const char kAttrViewBackground[] = "background";
    const char kAttrViewForeground[] = "foreground";

    const char kAttrValViewShow[] = "show";
    const char kAttrValViewHide[] = "hide";
    const char kAttrValViewVanished[] = "vanished";

    // TextView
    const char kAttrTextViewText[] = "text";
    const char kAttrTextViewFont[] = "font";
    const char kAttrTextViewTextSize[] = "text_size";
    const char kAttrTextViewTextColor[] = "text_color";
    const char kAttrTextViewTextStyle[] = "text_style";
    const char kAttrTextViewTextWeight[] = "text_weight";
    const char kAttrTextViewAutoWrap[] = "auto_wrap";
    const char kAttrTextViewIsSelectable[] = "selectable";
    const char kAttrTextViewIsEditable[] = "editable";
    const char kAttrTextViewVertAlign[] = "text_vert_align";
    const char kAttrTextViewHoriAlign[] = "text_hori_align";

    const char kAttrValTextViewStyleNormal[] = "normal";
    const char kAttrValTextViewStyleItalic[] = "italic";

    const char kAttrValTextViewWeightThin[] = "thin";
    const char kAttrValTextViewWeightNormal[] = "normal";
    const char kAttrValTextViewWeightBold[] = "bold";

    const char kAttrValTextViewAlignStart[] = "start";
    const char kAttrValTextViewAlignCenter[] = "center";
    const char kAttrValTextViewAlignEnd[] = "end";

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
    const char kAttrSeqLayoutVertAlign[] = "vert_align";
    const char kAttrSeqLayoutHoriAlign[] = "hori_align";

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