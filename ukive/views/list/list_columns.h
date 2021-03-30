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
        void addItem(ListItem* item, int index);
        void removeItem(int index);
        void removeItems(int start);
        void removeItems(int start, int length);
        void clear();

        int getLeft() const;
        int getRight() const;
        int getWidth() const;
        int getItemsTop() const;
        int getItemsBottom() const;
        int getItemCount() const;
        int getFinalScroll(int dy) const;
        int getIndexOfItem(int data_pos) const;
        int getIndexOfFirstVisible(int dy) const;
        int getIndexOfLastVisible(int dy) const;
        ListItem* getFront() const;
        ListItem* getRear() const;
        ListItem* getItem(int index) const;
        ListItem* getFirstVisible(int dy = 0) const;
        ListItem* getLastVisible(int dy = 0) const;
        ListItem* getItemByPos(int data_pos) const;
        ListItem* findAndInsertItem(int start_index, int item_id);
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
        explicit ColumnCollection(int col_count);

        Column& operator[](int col);
        const Column& operator[](int col) const;

        void setVertical(int top, int bottom);
        void setHorizontal(int left, int right);

        void clear();

        int getFinalScroll(int dy) const;
        ListItem* getFirst() const;
        ListItem* getLast() const;
        ListItem* getTopStart() const;
        ListItem* getBottomStart() const;
        ListItem* getItemByPos(int data_pos) const;
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

        bool isAllAtCeil(int item_count) const;
        bool isAllAtFloor(int item_count) const;

    private:
        int col_count_;
        std::vector<Column> columns_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_COLUMNS_H_