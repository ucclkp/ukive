// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "editable.h"

#include "utils/log.h"

#include "ukive/text/span/span.h"
#include "ukive/text/text_breaker.h"


namespace ukive {

    Editable::Editable(const std::u16string& text)
        : text_(text),
          sel_beg_(0),
          sel_end_(0) {
    }

    Editable::~Editable() {
    }

    // 对文本的操作包括插入、删除和替换。
    // 举例来说，如果你将 "text" 中的第一个 "t" 替换为 "T"，acpStart=0, acpOldEnd=1, and acpNewEnd=1.
    // 如果你删除最后一个 "t"，acpStart=3, acpOldEnd=4, and acpNewEnd=3.
    // 如果将 "a" 放在 "e" 和 "x" 之间，acpStart=2, acpOldEnd=2, and acpNewEnd=3.
    void Editable::notifyTextChanged(
        size_t start, size_t old_end, size_t new_end, Reason r)
    {
        for (auto span : spans_) {
            auto span_start = span->getStart();
            auto span_end = span->getEnd();

            if (span_start >= start && span_start < old_end) {
            }
        }

        for (auto watcher : watchers_) {
            watcher->onTextChanged(this, start, old_end, new_end, r);
        }
    }

    void Editable::notifySelectionChanged(
        size_t ns, size_t ne, size_t os, size_t oe, Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onSelectionChanged(ns, ne, os, oe, r);
        }
    }

    void Editable::notifyEditWatcher(
        size_t text_start, size_t old_text_end, size_t new_text_end,
        size_t new_sel_start, size_t new_sel_end,
        size_t old_sel_start, size_t old_sel_end, Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onTextChanged(this, text_start, old_text_end, new_text_end, r);
            watcher->onSelectionChanged(new_sel_start, new_sel_end, old_sel_start, old_sel_end, r);
        }
    }

    void Editable::notifySpanChanged(
        Span* span, EditWatcher::SpanChange action, Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onSpanChanged(span, action, r);
        }
    }

    size_t Editable::length() const {
        return text_.length();
    }

    void Editable::append(const std::u16string& text, Reason r) {
        insert(text, length(), r);
    }

    void Editable::insert(const std::u16string& text, size_t position, Reason r) {
        if (!text.empty()) {
            text_.insert(position, text);
            notifyTextChanged(position, position, position + text.length(), r);
        }
    }

    void Editable::remove(size_t start, size_t length, Reason r) {
        if (!text_.empty()) {
            replace(u"", start, length, r);
        }
    }

    void Editable::replace(const std::u16string& text, size_t start, size_t length, Reason r) {
        text_.replace(start, length, text);
        notifyTextChanged(start, start + length, start + text.length(), r);
    }

    void Editable::clear(Reason r) {
        if (!text_.empty()) {
            auto old_start = sel_beg_;
            auto old_end = sel_end_;
            auto old_length = length();

            text_.clear();

            sel_beg_ = 0;
            sel_end_ = 0;

            if (old_start != 0 || old_end != 0) {
                notifyEditWatcher(
                    0, old_length, 0,
                    sel_beg_, sel_end_,
                    old_start, old_end, r);
            } else {
                notifyTextChanged(0, old_length, 0, r);
            }
        }
    }

    void Editable::replace(const std::u16string& find, const std::u16string& rep, Reason r) {
        auto first = text_.find(find);
        if (first != std::u16string::npos) {
            text_.replace(first, find.length(), rep);
            notifyTextChanged(
                first,
                first + find.length(),
                first + rep.length(), r);
        }
    }

    void Editable::insert(const std::u16string& text, Reason r) {
        auto old_start = sel_beg_;
        auto old_end = sel_end_;

        if (old_start == old_end) {
            if (length() < old_end) {
                ubassert(false);
                text_.insert(text_.length(), text);
            } else {
                text_.insert(old_end, text);
            }
            sel_beg_ += text.length();
            sel_end_ += text.length();

            notifyEditWatcher(
                old_start, old_start, old_start + text.length(),
                sel_beg_, sel_end_, old_start, old_end, r);
        }
    }

    void Editable::remove(Reason r) {
        replace({}, r);
    }

    void Editable::replace(const std::u16string& text, Reason r) {
        auto old_start = sel_beg_;
        auto old_end = sel_end_;

        bool has_selection = old_start != old_end;

        if (has_selection) {
            text_.replace(old_start, old_end - old_start, text);
            sel_beg_ = sel_end_ = old_start + text.length();

            notifyEditWatcher(
                old_start, old_end, old_start + text.length(),
                sel_beg_, sel_end_, old_start, old_end, r);
        }
    }

    void Editable::setSelection(size_t selection, Reason r) {
        setSelection(selection, selection, r);
    }

    void Editable::setSelection(size_t start, size_t end, Reason r) {
        if (start == sel_beg_ && end == sel_end_) {
            return;
        }

        if (start > length()) start = length();
        if (end > length()) end = length();

        if (start > end) {
            auto tmp = start;
            start = end;
            end = tmp;
        }

        auto old_se_start = sel_beg_;
        auto old_se_end = sel_end_;

        sel_beg_ = start;
        sel_end_ = end;

        notifySelectionChanged(start, end, old_se_start, old_se_end, r);
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

    size_t Editable::getSelectionStart() const {
        return sel_beg_;
    }

    size_t Editable::getSelectionEnd() const {
        return sel_end_;
    }

    bool Editable::hasSelection() const {
        return sel_beg_ != sel_end_;
    }

    std::u16string Editable::getSelection() const {
        if (sel_beg_ == sel_end_) {
            return {};
        }

        return text_.substr(sel_beg_, sel_end_ - sel_beg_);
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

    void Editable::addEditWatcher(EditWatcher* watcher) {
        watchers_.push_back(watcher);
    }

    void Editable::removeEditWatcher(EditWatcher* watcher) {
        for (auto it = watchers_.begin(); it != watchers_.end();) {
            if (watcher == (*it)) {
                it = watchers_.erase(it);
            } else {
                ++it;
            }
        }
    }

}
