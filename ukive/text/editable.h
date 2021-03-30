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
                Editable* editable,
                size_t start, size_t old_end, size_t new_end, Reason r) = 0;

            virtual void onSelectionChanged(
                size_t ns, size_t ne,
                size_t os, size_t oe, Reason r) = 0;

            virtual void onSpanChanged(
                Span* span, SpanChange action, Reason r) = 0;
        };

        explicit Editable(const std::u16string& text);
        ~Editable();

        size_t length() const;

        void append(const std::u16string& text, Reason r = API);
        void insert(const std::u16string& text, size_t position, Reason r = API);
        void remove(size_t start, size_t length, Reason r = API);
        void replace(const std::u16string& text, size_t start, size_t length, Reason r = API);
        void clear(Reason r = API);
        void replace(const std::u16string& find, const std::u16string& rep, Reason r = API);
        void insert(const std::u16string& text, Reason r = API);
        void remove(Reason r = API);
        void replace(const std::u16string& text, Reason r = API);
        void setSelection(size_t selection, Reason r = API);
        void setSelection(size_t start, size_t end, Reason r = API);
        void addSpan(Span* span, Reason r = API);
        void removeSpan(size_t index, Reason r = API);
        void removeAllSpans(Reason r = API);

        bool isInteractable() const;
        size_t getSelectionStart() const;
        size_t getSelectionEnd() const;
        bool hasSelection() const;
        std::u16string getSelection() const;
        char16_t at(size_t pos) const;
        const std::u16string& getString() const;
        size_t getPrevOffset(size_t cur) const;
        size_t getNextOffset(size_t cur) const;
        Span* getSpan(size_t index) const;
        size_t getSpanCount() const;

        void addEditWatcher(EditWatcher* watcher);
        void removeEditWatcher(EditWatcher* watcher);

    private:
        void notifyTextChanged(
            size_t start, size_t old_end, size_t new_end, Reason r);
        void notifySelectionChanged(
            size_t ns, size_t ne, size_t os, size_t oe, Reason r);
        void notifyEditWatcher(
            size_t text_start, size_t old_text_end, size_t new_text_end,
            size_t new_sel_start, size_t new_sel_end, size_t old_sel_start, size_t old_sel_end, Reason r);
        void notifySpanChanged(
            Span* span, EditWatcher::SpanChange action, Reason r);

        std::u16string text_;
        std::list<EditWatcher*> watchers_;
        std::vector<std::shared_ptr<Span>> spans_;

        size_t sel_beg_;
        size_t sel_end_;
    };

}

#endif  // UKIVE_TEXT_EDITABLE_H_
