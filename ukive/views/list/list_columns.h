// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LIST_COLUMNS_H_
#define UKIVE_VIEWS_LIST_LIST_COLUMNS_H_

#include <vector>

#include "ukive/views/list/list_source.h"


namespace ukive {

    // Column
    class Column {
    public:
        Column();

        void setVertical(int top, int bottom);
        void setHorizontal(int left, int right);

        void addItem(ListItem* item);
        void addItem(ListItem* item, size_t index);
        void removeItem(size_t index);
        void removeItems(size_t start);
        void removeItems(size_t start, size_t length);
        void clear();

        int getLeft() const;
        int getRight() const;
        int getWidth() const;
        int getItemsTop() const;
        int getItemsBottom() const;
        size_t getItemCount() const;
        int getFinalScroll(int dy) const;
        bool getIndexOfItem(size_t data_pos, size_t* index) const;
        bool getIndexOfFirstVisible(int dy, size_t* index) const;
        bool getIndexOfLastVisible(int dy, size_t* index) const;
        ListItem* getFront() const;
        ListItem* getRear() const;
        ListItem* getItem(size_t index) const;
        ListItem* getFirstVisible(int dy = 0) const;
        ListItem* getLastVisible(int dy = 0) const;
        ListItem* getItemByPos(size_t data_pos) const;
        ListItem* findAndInsertItem(size_t start_index, int item_id);
        ListItem* findItemFromView(View* v) const;

        bool atTop() const;
        bool atBottom() const;
        bool isTopFilled(int dy) const;
        bool isBottomFilled(int dy) const;

    private:
        int top_, bottom_;
        int left_, right_;
        std::vector<ListItem*> items_;
    };


    // ColumnCollection
    class ColumnCollection {
    public:
        explicit ColumnCollection(size_t col_count);

        Column& operator[](size_t col);
        const Column& operator[](size_t col) const;

        void setVertical(int top, int bottom);
        void setHorizontal(int left, int right);

        void clear();

        int getFinalScroll(int dy) const;
        ListItem* getFirst() const;
        ListItem* getLast() const;
        ListItem* getTopStart() const;
        ListItem* getBottomStart() const;
        ListItem* getItemByPos(size_t data_pos) const;
        ListItem* getTopmost() const;
        ListItem* getBottomost() const;
        ListItem* findItemFromView(View* v) const;

        bool isAllAtTop() const;
        bool isAllAtBottom() const;

        /**
         * 检测是否所有列中的 View 都超过顶部，
         * 如果是，返回 true，否则返回 false。
         */
        bool isTopFilled(int dy) const;

        /**
         * 检测是否所有列中的 View 都超过底部，
         * 如果是，返回 true，否则返回 false。
         */
        bool isBottomFilled(int dy) const;

        bool isBottomOneFilled(int dy) const;

        bool isAllAtCeil(size_t item_count) const;
        bool isAllAtFloor(size_t item_count) const;

    private:
        size_t col_count_;
        std::vector<Column> columns_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_COLUMNS_H_