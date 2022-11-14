// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_EDITABLE_H_
#define UKIVE_TEXT_EDITABLE_H_

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "ukive/text/range.hpp"
#include "ukive/text/selection.hpp"


namespace ukive {

    class Span;

    /**
     * 文本编辑类。
     * 使用字间定位。
     */
    class Editable {
    public:
        enum Reason {
            USER_INPUT,
            API
        };

        class EditWatcher {
        public:
            virtual ~EditWatcher() = default;

            enum SpanChange {
                ADD,
                REMOVE,
                EDIT,
            };

            virtual void onTextChanged(
                Editable* editable, const RangeChg& rc, Reason r) {}
            virtual void onSelectionChanged(
                Editable* editable,
                const Selection& nsl,
                const Selection& osl,
                Reason r) {}
            virtual void onSpanChanged(
                Editable* editable,
                Span* span, SpanChange action, Reason r) {}
        };

        explicit Editable(const std::u16string_view& text, void* ctx = nullptr);
        ~Editable();

        size_t length() const;

        void append(const std::u16string_view& text, Reason r = API);
        void insert(const std::u16string_view& text, size_t position, Reason r = API);
        void remove(size_t start, size_t length, Reason r = API);
        void replace(const std::u16string_view& text, size_t start, size_t length, Reason r = API);
        void clear(Reason r = API);
        void replace(const std::u16string_view& find, const std::u16string_view& rep, Reason r = API);
        void insert(const std::u16string_view& text, Reason r = API);
        void remove(Reason r = API);
        void replace(const std::u16string_view& text, Reason r = API);
        void setSelection(size_t selection, Reason r = API);
        void setSelection(size_t start, size_t end, Reason r = API);
        void addSpan(Span* span, Reason r = API);
        void removeSpan(size_t index, Reason r = API);
        void removeAllSpans(Reason r = API);

        bool isInteractable() const;
        bool hasSelection() const;
        const Selection& getSelection() const;
        std::u16string_view getSelectionString() const;
        char16_t at(size_t pos) const;
        const std::u16string& getString() const;
        size_t getPrevOffset(size_t cur) const;
        size_t getNextOffset(size_t cur) const;
        Span* getSpan(size_t index) const;
        size_t getSpanCount() const;
        void* getContext() const;

        void addEditWatcher(EditWatcher* watcher);
        void removeEditWatcher(EditWatcher* watcher);

    private:
        void notifyTextChanged(
            const RangeChg& rc, Reason r);
        void notifySelectionChanged(
            const Selection& nsl, const Selection& osl, Reason r);
        void notifyEditWatcher(
            const RangeChg& text_rc,
            const Selection& nsl, const Selection& osl, Reason r);
        void notifySpanChanged(
            Span* span, EditWatcher::SpanChange action, Reason r);

        void* ctx_;
        std::u16string text_;
        std::vector<EditWatcher*> watchers_;
        std::vector<std::shared_ptr<Span>> spans_;
        Selection sel_;
    };

}

#endif  // UKIVE_TEXT_EDITABLE_H_
