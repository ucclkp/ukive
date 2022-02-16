// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "tsf_input_connection.h"

#include "utils/log.h"
#include "utils/numbers.hpp"

#include "ukive/app/application.h"
#include "ukive/text/win/tsf_editor.h"
#include "ukive/text/win/tsf_manager.h"
#include "ukive/views/text_view.h"
#include "ukive/window/window.h"


namespace ukive {
namespace win {

    TsfInputConnection::TsfInputConnection(TextInputClient* tic)
        : client_(tic),
          tsf_editor_(nullptr),
          editor_cookie_(0),
          is_initialized_(false),
          is_editor_pushed_(false) {
    }

    TsfInputConnection::~TsfInputConnection() {
    }

    bool TsfInputConnection::initialization() {
        if (is_initialized_) {
            return true;
        }

        auto imm = Application::getInputMethodManager();
        auto tsf_mgr = static_cast<TsfManager*>(imm);

        tsf_editor_ = new TsfEditor(0xbeef);
        tsf_editor_->setInputConnection(this);

        HRESULT hr = tsf_mgr->getThreadManager()->CreateDocumentMgr(&doc_mgr_);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "Create doc mgr failed: " << hr;
            return false;
        }

        hr = doc_mgr_->CreateContext(
            tsf_mgr->getClientId(), 0,
            static_cast<ITextStoreACP*>(tsf_editor_),
            &editor_context_, &editor_cookie_);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "Create context failed: " << hr;
            return false;
        }

        hr = editor_context_->QueryInterface(IID_PPV_ARGS(&comp_service_));
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "Create composition service failed: " << hr;
            return false;
        }

        is_initialized_ = true;

        return true;
    }

    bool TsfInputConnection::pushEditor() {
        if (!is_initialized_ || is_editor_pushed_) {
            return false;
        }

        HRESULT hr = doc_mgr_->Push(editor_context_.get());
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "pushEditor() failed: " << hr;
            return false;
        }

        is_editor_pushed_ = true;
        return true;
    }

    bool TsfInputConnection::popEditor() {
        if (!is_initialized_ || !is_editor_pushed_) {
            return false;
        }

        HRESULT hr = doc_mgr_->Pop(TF_POPF_ALL);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "popEditor() failed: " << hr;
            return false;
        }

        is_editor_pushed_ = false;
        return true;
    }

    bool TsfInputConnection::requestFocus() {
        if (!is_initialized_) {
            return false;
        }

        auto imm = Application::getInputMethodManager();
        auto tsf_mgr = static_cast<TsfManager*>(imm);

        HRESULT hr = tsf_mgr->getThreadManager()->SetFocus(doc_mgr_.get());
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "mount() failed: " << hr;
            return false;
        }

        return true;
    }

    bool TsfInputConnection::discardFocus() {
        if (!is_initialized_) {
            return false;
        }

        auto imm = Application::getInputMethodManager();
        auto tsf_mgr = static_cast<TsfManager*>(imm);

        HRESULT hr = tsf_mgr->getThreadManager()->SetFocus(nullptr);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "unmount() failed: " << hr;
            return false;
        }

        return true;
    }

    bool TsfInputConnection::terminateComposition() {
        if (!is_initialized_) {
            return false;
        }

        HRESULT hr = comp_service_->TerminateComposition(nullptr);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "terminateComposition() failed: " << hr;
            return false;
        }

        return true;
    }

    bool TsfInputConnection::isCompositing() const {
        if (!is_initialized_) {
            return false;
        }
        return tsf_editor_->isCompositing();
    }

    void TsfInputConnection::notifyStatusChanged(uint32_t flags) {
        tsf_editor_->notifyStatusChanged(flags);
    }

    void TsfInputConnection::notifyTextChanged(bool correction, const TextChangeInfo& info) {
        tsf_editor_->notifyTextChanged(
            correction,
            utl::num_cast<LONG>(info.start),
            utl::num_cast<LONG>(info.old_end),
            utl::num_cast<LONG>(info.new_end));
    }

    void TsfInputConnection::notifyTextLayoutChanged(TextLayoutChangeReason r) {
        TsLayoutCode tsf_r;
        switch (r) {
        case TLC_CREATE: tsf_r = TS_LC_CREATE; break;
        case TLC_CHANGE: tsf_r = TS_LC_CHANGE; break;
        case TLC_DESTROY: tsf_r = TS_LC_DESTROY; break;
        default: tsf_r = TS_LC_CHANGE; break;
        }

        tsf_editor_->notifyTextLayoutChanged(tsf_r);
    }

    void TsfInputConnection::notifyTextSelectionChanged() {
        tsf_editor_->notifyTextSelectionChanged();
    }

    void TsfInputConnection::onBeginProcess() {
        client_->onTICBeginProcess();
    }

    void TsfInputConnection::onEndProcess() {
        client_->onTICEndProcess();
    }

    bool TsfInputConnection::isReadOnly() const {
        return !client_->isTICEditable();
    }

    void TsfInputConnection::determineInsert(
        LONG start, LONG end, ULONG repLength,
        LONG* resStart, LONG* resEnd)
    {
        *resStart = start;
        *resEnd = end;
    }

    bool TsfInputConnection::getSelection(
        ULONG startIndex, ULONG maxCount,
        TS_SELECTION_ACP* selections, ULONG* fetchedCount)
    {
        if (startIndex != TF_DEFAULT_SELECTION || maxCount != 1) {
            return false;
        }

        auto sel_start = client_->getTICEditable()->getSelectionStart();
        auto sel_end = client_->getTICEditable()->getSelectionEnd();

        selections[0].acpStart = utl::num_cast<LONG>(sel_start);
        selections[0].acpEnd = utl::num_cast<LONG>(sel_end);
        selections[0].style.fInterimChar = FALSE;
        selections[0].style.ase = TS_AE_END;

        *fetchedCount = 1;

        return true;
    }

    bool TsfInputConnection::setSelection(ULONG count, const TS_SELECTION_ACP* selections) {
        if (count != 1) {
            return false;
        }

        size_t sel_start = utl::num_cast<size_t>(selections[0].acpStart);
        size_t sel_end = utl::num_cast<size_t>(selections[0].acpEnd);
        if (sel_start == client_->getTICEditable()->getSelectionStart() &&
            sel_end == client_->getTICEditable()->getSelectionEnd())
        {
            client_->onTICRedrawSelection();
            return true;
        }

        if (sel_start == sel_end) {
            client_->getTICEditable()->setSelection(sel_start, Editable::Reason::USER_INPUT);
        } else {
            client_->getTICEditable()->setSelection(sel_start, sel_end, Editable::Reason::USER_INPUT);
        }

        return true;
    }

    void TsfInputConnection::getText(LONG start, LONG end, ULONG maxLength, WCHAR* text, ULONG* length) {
        auto& u16_text = client_->getTICEditable()->getString();
        auto u_start = utl::num_cast<ULONG>(start);

        if (end == -1) {
            if (u_start + maxLength <= u16_text.length()) {
                for (ULONG i = 0; i < maxLength; ++i) {
                    text[i] = u16_text[i + u_start];
                }
                *length = maxLength;
            } else {
                auto len = utl::num_cast<ULONG>(u16_text.length() - start);
                for (ULONG i = 0; i < len; ++i) {
                    text[i] = u16_text[i + u_start];
                }
                *length = len;
            }
            return;
        }

        auto cr = utl::num_cast<ULONG>(std::abs(end - start));
        if (cr >= maxLength) {
            for (ULONG i = 0; i < maxLength; ++i) {
                text[i] = u16_text[i + u_start];
            }
            *length = maxLength;
        } else {
            for (ULONG i = 0; i < cr; ++i) {
                text[i] = u16_text[i + u_start];
            }
            *length = cr;
        }
    }

    void TsfInputConnection::setText(LONG start, LONG end, const WCHAR* text, ULONG length) {
        std::u16string u16_text(text, text + length);

        if (start == end) {
            client_->getTICEditable()->insert(u16_text, start, Editable::Reason::USER_INPUT);
        } else {
            client_->getTICEditable()->replace(u16_text, start, end - start, Editable::Reason::USER_INPUT);
            if (u16_text.empty()) {
                // 输入法在候选阶段时，如果窗口失去焦点，当前的输入会被抹掉，但 TSF 并不会通知光标变动，
                // 所以我们在这种情况下自己设置光标的正确位置。
                client_->getTICEditable()->setSelection(start, Editable::Reason::USER_INPUT);
            }
        }
    }

    LONG TsfInputConnection::getTextLength() const {
        return utl::num_cast<LONG>(client_->getTICEditable()->length());
    }

    bool TsfInputConnection::getTextPositionAtPoint(const POINT* pt, DWORD dwFlags, LONG* pacp) const {
        return false;
    }

    bool TsfInputConnection::getTextBound(LONG start, LONG end, RECT* prc, BOOL* pfClipped) const {
        Rect bounds;
        bool clipped;
        client_->getTICSelectionBounds(start, end, &bounds, &clipped);

        prc->left = bounds.left;
        prc->top = bounds.top;
        prc->right = bounds.right;
        prc->bottom = bounds.bottom;

        return true;
    }

    void TsfInputConnection::getTextViewBound(RECT* prc) const {
        Rect bounds;
        client_->getTICBounds(&bounds);

        prc->left = bounds.left;
        prc->top = bounds.top;
        prc->right = bounds.right;
        prc->bottom = bounds.bottom;
    }

    void TsfInputConnection::insertTextAtSelection(
        DWORD dwFlags, const WCHAR* text, ULONG length,
        LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange)
    {
        switch (dwFlags) {
        case 0: {
            auto sel_start = utl::num_cast<LONG>(client_->getTICEditable()->getSelectionStart());
            auto sel_end = utl::num_cast<LONG>(client_->getTICEditable()->getSelectionEnd());

            *pacpStart = sel_start;
            *pacpEnd = sel_end;

            pChange->acpStart = sel_start;
            pChange->acpOldEnd = sel_end;
            pChange->acpNewEnd = sel_start + (length - (sel_end - sel_start));
            break;
        }

        case TF_IAS_NOQUERY: {
            auto sel_start = utl::num_cast<LONG>(client_->getTICEditable()->getSelectionStart());
            auto sel_end = utl::num_cast<LONG>(client_->getTICEditable()->getSelectionEnd());

            pChange->acpStart = sel_start;
            pChange->acpOldEnd = sel_end;
            pChange->acpNewEnd = sel_end;
            break;
        }

        case TF_IAS_QUERYONLY:
            *pacpStart = utl::num_cast<LONG>(client_->getTICEditable()->getSelectionStart());
            *pacpEnd = utl::num_cast<LONG>(client_->getTICEditable()->getSelectionEnd());
            break;

        default:
            ubassert(false);
            break;
        }
    }

    Window* TsfInputConnection::getWindow() const {
        return client_->getTICHostWindow();
    }

}
}