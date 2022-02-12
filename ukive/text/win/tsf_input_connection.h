// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_WIN_TSF_INPUT_CONNECTION_H_
#define UKIVE_TEXT_WIN_TSF_INPUT_CONNECTION_H_

#include <string>

#include <msctf.h>

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/text/input_method_connection.h"


namespace ukive {

    class Window;
    class TextInputClient;

namespace win {

    class TsfEditor;
    class TsfManager;

    class TsfInputConnection : public InputMethodConnection {
    public:
        explicit TsfInputConnection(TextInputClient* tic);
        ~TsfInputConnection();

        bool initialization() override;

        bool pushEditor() override;
        bool popEditor() override;

        bool requestFocus() override;
        bool discardFocus() override;

        bool terminateComposition() override;

        bool isCompositing() const override;

        void notifyStatusChanged(uint32_t flags) override;
        void notifyTextChanged(bool correction, const TextChangeInfo& info) override;
        void notifyTextLayoutChanged(TextLayoutChangeReason r) override;
        void notifyTextSelectionChanged() override;

        void onBeginProcess();
        void onEndProcess();

        bool isReadOnly() const;

        void determineInsert(
            LONG start, LONG end, ULONG repLength,
            LONG* resStart, LONG* resEnd);
        bool getSelection(
            ULONG startIndex, ULONG maxCount,
            TS_SELECTION_ACP* selections, ULONG* fetchedCount);
        bool setSelection(ULONG count, const TS_SELECTION_ACP* selections);
        void getText(LONG start, LONG end, ULONG maxLength, WCHAR* text, ULONG* length);
        void setText(LONG start, LONG end, const WCHAR* text, ULONG length);
        void insertTextAtSelection(
            DWORD dwFlags, const WCHAR* text, ULONG length,
            LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange);

        LONG getTextLength() const;
        bool getTextPositionAtPoint(const POINT* pt, DWORD dwFlags, LONG* pacp) const;
        bool getTextBound(LONG start, LONG end, RECT* prc, BOOL* pfClipped) const;

        void getTextViewBound(RECT* prc) const;

        Window* getWindow() const;

    private:
        TextInputClient* client_;

        TsfEditor* tsf_editor_;
        TfEditCookie editor_cookie_;
        utl::win::ComPtr<ITfContext> editor_context_;
        utl::win::ComPtr<ITfDocumentMgr> doc_mgr_;
        utl::win::ComPtr<ITfContextOwnerCompositionServices> comp_service_;

        bool is_initialized_;
        bool is_editor_pushed_;
    };

}
}

#endif  // UKIVE_TEXT_WIN_TSF_INPUT_CONNECTION_H_