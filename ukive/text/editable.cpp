// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "editable.h"

#include "utils/log.h"
#include "utils/multi_callbacks.hpp"

#include "ukive/text/span/span.h"
#include "ukive/text/text_breaker.h"


namespace ukive {

    Editable::Editable(const std::u16string_view& text, void* ctx)
        : ctx_(ctx),
          text_(text)
    {
    }

    Editable::~Editable() {
    }

    // 对文本的操作包括插入、删除和替换。
    // 举例来说，如果你将 "text" 中的第一个 "t" 替换为 "T"，acpStart=0, acpOldEnd=1, and acpNewEnd=1.
    // 如果你删除最后一个 "t"，acpStart=3, acpOldEnd=4, and acpNewEnd=3.
    // 如果将 "a" 放在 "e" 和 "x" 之间，acpStart=2, acpOldEnd=2, and acpNewEnd=3.
    void Editable::notifyTextChanged(
        const RangeChg& rc, Reason r)
    {
        for (auto span : spans_) {
            auto span_start = span->getStart();
            auto span_end = span->getEnd();

            if (span_start >= rc.pos && span_start < rc.old_end()) {
            }
        }

        for (auto watcher : watchers_) {
            watcher->onTextChanged(this, rc, r);
        }
    }

    void Editable::notifySelectionChanged(
        const Selection& nsl, const Selection& osl, Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onSelectionChanged(this, nsl, osl, r);
        }
    }

    void Editable::notifyEditWatcher(
        const RangeChg& text_rc,
        const Selection& nsl, const Selection& osl,
        Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onTextChanged(this, text_rc, r);
            watcher->onSelectionChanged(this, nsl, osl, r);
        }
    }

    void Editable::notifySpanChanged(
        Span* span, EditWatcher::SpanChange action, Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onSpanChanged(this, span, action, r);
        }
    }

    size_t Editable::length() const {
        return text_.length();
    }

    void Editable::append(const std::u16string_view& text, Reason r) {
        insert(text, length(), r);
    }

    void Editable::insert(
        const std::u16string_view& text, size_t position, Reason r)
    {
        if (!text.empty()) {
            text_.insert(position, text);
            notifyTextChanged({ position, 0, text.length() }, r);
        }
    }

    void Editable::remove(size_t start, size_t length, Reason r) {
        if (!text_.empty()) {
            replace(u"", start, length, r);
        }
    }

    void Editable::replace(
        const std::u16string_view& text, size_t start, size_t length, Reason r)
    {
        text_.replace(start, length, text);
        notifyTextChanged({ start, length, text.length() }, r);
    }

    void Editable::clear(Reason r) {
        if (!text_.empty()) {
            auto old_sel = sel_;
            auto old_length = length();

            text_.clear();
            sel_.zero();

            if (!old_sel.is_zero()) {
                notifyEditWatcher({ 0, old_length, 0 }, sel_, old_sel, r);
            } else {
                notifyTextChanged({ 0, old_length, 0 }, r);
            }
        }
    }

    void Editable::replace(
        const std::u16string_view& find, const std::u16string_view& rep, Reason r)
    {
        auto first = text_.find(find);
        if (first != std::u16string::npos) {
            text_.replace(first, find.length(), rep);
            notifyTextChanged({ first, find.length(), rep.length() }, r);
        }
    }

    void Editable::insert(const std::u16string_view& text, Reason r) {
        auto old_sel = sel_;
        if (old_sel.empty()) {
            if (length() < old_sel.end) {
                ubassert(false);
                text_.insert(text_.length(), text);
            } else {
                text_.insert(old_sel.end, text);
            }

            sel_.offset(text.length());

            notifyEditWatcher({ old_sel.start, 0, text.length() }, sel_, old_sel, r);
        }
    }

    void Editable::remove(Reason r) {
        replace({}, r);
    }

    void Editable::replace(const std::u16string_view& text, Reason r) {
        auto old_sel = sel_;
        if (!old_sel.empty()) {
            text_.replace(old_sel.start, old_sel.length(), text);
            sel_.set(old_sel.start + text.length());

            notifyEditWatcher(
                { old_sel.start, old_sel.length(), text.length() }, sel_, old_sel, r);
        }
    }

    void Editable::setSelection(size_t selection, Reason r) {
        setSelection(selection, selection, r);
    }

    void Editable::setSelection(size_t start, size_t end, Reason r) {
        if (sel_.equal(start, end)) {
            return;
        }

        if (start > length()) start = length();
        if (end > length()) end = length();

        if (start > end) {
            auto tmp = start;
            start = end;
            end = tmp;
        }

        auto old_sel = sel_;
        sel_.set(start, end);

        notifySelectionChanged({ start, end }, old_sel, r);
    }

    void Editable::addSpan(Span* span, Reason r) {
        spans_.push_back(std::shared_ptr<Span>(span));

        notifySpanChanged(
            span, EditWatcher::SpanChange::ADD, r);
    }

    void Editable::removeSpan(size_t index, Reason r) {
        Span* span = getSpan(index);
        if (span) {
            notifySpanChanged(
                span, EditWatcher::SpanChange::REMOVE, r);

            spans_.erase(spans_.begin() + index);
        }
    }

    void Editable::removeAllSpans(Reason r) {
        while (!spans_.empty()) {
            removeSpan(0, r);
        }
    }

    bool Editable::isInteractable() const {
        for (const auto& span : spans_) {
            if (span->getType() == Span::INTERACTABLE) {
                return true;
            }
        }
        return false;
    }

    bool Editable::hasSelection() const {
        return !sel_.empty();
    }

    const Selection& Editable::getSelection() const {
        return sel_;
    }

    std::u16string_view Editable::getSelectionString() const {
        if (sel_.empty()) {
            return {};
        }

        std::u16string_view r(text_);
        return r.substr(sel_.start, sel_.length());
    }

    char16_t Editable::at(size_t pos) const {
        return text_.at(pos);
    }

    const std::u16string& Editable::getString() const {
        return text_;
    }

    size_t Editable::getPrevOffset(size_t cur) const {
        CharacterBreaker breaker(&text_);
        breaker.setCur(cur);
        breaker.prev();
        return breaker.getPrev() - breaker.getCur();
    }

    size_t Editable::getNextOffset(size_t cur) const {
        CharacterBreaker breaker(&text_);
        breaker.setCur(cur);
        breaker.next();
        return breaker.getCur() - breaker.getPrev();
    }

    Span* Editable::getSpan(size_t index) const {
        return spans_.at(index).get();
    }

    size_t Editable::getSpanCount() const {
        return spans_.size();
    }

    void* Editable::getContext() const {
        return ctx_;
    }

    void Editable::addEditWatcher(EditWatcher* watcher) {
        utl::addCallbackTo(watchers_, watcher);
    }

    void Editable::removeEditWatcher(EditWatcher* watcher) {
        utl::removeCallbackFrom(watchers_, watcher);
    }

}
