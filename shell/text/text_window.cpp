// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "text_window.h"

#include "utils/log.h"

#include "ukive/views/button.h"
#include "ukive/views/text_view.h"
#include "ukive/views/layout/sequence_layout.h"
#include "ukive/views/layout_info/sequence_layout_info.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/elements/color_element.h"
#include "ukive/graphics/color.h"
#include "ukive/elements/ripple_element.h"
#include "ukive/menu/context_menu.h"
#include "ukive/menu/menu.h"


namespace shell {

    TextWindow::TextWindow()
        : Window(),
          context_menu_(nullptr) {}

    TextWindow::~TextWindow() {
    }

    void TextWindow::onCreated() {
        Window::onCreated();

        using Rlp = ukive::RestraintLayoutInfo;

        //root view.
        ukive::RestraintLayout *rootLayout = new ukive::RestraintLayout(getContext());
        rootLayout->setId(ID_LAYOUT_ROOT);
        rootLayout->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_FILL);

        setContentView(rootLayout);

        //Toolbar.
        auto toolbar = inflateToolbar(rootLayout);
        toolbar->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);

        Rlp *toolbarLp = Rlp::Builder()
            .start(ID_LAYOUT_ROOT)
            .top(ID_LAYOUT_ROOT)
            .end(ID_LAYOUT_ROOT).build();
        toolbar->setExtraLayoutInfo(toolbarLp);

        rootLayout->addView(toolbar);


        //编辑器。
        ukive::TextView *editorTV = new ukive::TextView(getContext());
        editorTV->setId(ID_TV_EDITOR);
        editorTV->setIsEditable(true);
        editorTV->setIsSelectable(true);
        //editorTV->setFontFamilyName(L"Consolas");
        editorTV->setTextSize(getContext().dp2px(14));
        editorTV->setPadding(18, 18, 18, 18);
        editorTV->setLineSpacing(ukive::TextLayout::LineSpacing::PROPORTIONAL, 1.2f);
        editorTV->autoWrap(false);
        editorTV->requestFocus();
        editorTV->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_FILL);

        Rlp *editorTVLp = Rlp::Builder()
            .start(ID_LAYOUT_ROOT)
            .top(ID_TOOLBAR, Rlp::BOTTOM)
            .end(ID_LAYOUT_ROOT)
            .bottom(ID_LAYOUT_ROOT).build();
        editorTV->setExtraLayoutInfo(editorTVLp);

        rootLayout->addView(editorTV);
    }

    void TextWindow::onMove(int x, int y) {
        Window::onMove(x, y);
    }

    void TextWindow::onResize(int param, int width, int height) {
        Window::onResize(param, width, height);
    }

    ukive::View *TextWindow::inflateToolbar(ukive::View *parent) {
        ukive::SequenceLayout *toolbar = new ukive::SequenceLayout(getContext());
        toolbar->setId(ID_TOOLBAR);
        toolbar->setShadowRadius(1);
        toolbar->setOrientation(ukive::SequenceLayout::HORIZONTAL);
        toolbar->setBackground(new ukive::ColorElement(ukive::Color::White));

        ukive::TextView *font = new ukive::TextView(getContext());
        font->setId(ID_TOOLBAR_ITEM_FONT);
        font->setText(u"字体");
        font->setTextSize(getContext().dp2px(13));
        font->setPadding(16, 8, 16, 8);
        font->setBackground(new ukive::RippleElement());
        font->setOnClickListener(this);
        font->setClickable(true);
        font->setFocusable(true);
        font->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_FILL);
        toolbar->addView(font);

        ukive::TextView *format = new ukive::TextView(getContext());
        format->setId(ID_TOOLBAR_ITEM_FORMAT);
        format->setText(u"格式");
        format->setTextSize(getContext().dp2px(13));
        format->setPadding(16, 8, 16, 8);
        format->setBackground(new ukive::RippleElement());
        format->setOnClickListener(this);
        format->setClickable(true);
        format->setFocusable(true);
        format->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_FILL);
        toolbar->addView(format);

        return toolbar;
    }

    bool TextWindow::onCreateContextMenu(
        ukive::ContextMenu *contextMenu, ukive::Menu *menu)
    {
        menu->addItem(0, 0, u"Test");
        menu->addItem(1, 0, u"Test");
        menu->addItem(2, 0, u"Test");
        menu->addItem(3, 0, u"Test");

        return true;
    }

    bool TextWindow::onPrepareContextMenu(
        ukive::ContextMenu *contextMenu, ukive::Menu *menu)
    {
        return true;
    }

    bool TextWindow::onContextMenuItemClicked(
        ukive::ContextMenu *contextMenu, ukive::MenuItem *item)
    {
        contextMenu->close();
        return false;
    }

    void TextWindow::onDestroyContextMenu(ukive::ContextMenu *contextMenu) {
        context_menu_ = nullptr;
    }

    void TextWindow::onClick(ukive::View *v) {
        switch (v->getId()) {
        case ID_TOOLBAR_ITEM_FONT:
            if (context_menu_ == nullptr) {
                context_menu_ = startContextMenu(
                    this, v, ukive::View::Gravity::LEFT);

                findView(ID_TOOLBAR_ITEM_FONT)->requestFocus();
            }
            break;

        case ID_TOOLBAR_ITEM_FORMAT:
            if (context_menu_ == nullptr) {
                context_menu_ = startContextMenu(
                    this, v, ukive::View::Gravity::LEFT);

                findView(ID_TOOLBAR_ITEM_FORMAT)->requestFocus();
            }
            break;

        default:
            break;
        }
    }

}