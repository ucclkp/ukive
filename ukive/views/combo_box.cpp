// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "combo_box.h"

#include <algorithm>

#include "utils/numbers.hpp"
#include "utils/weak_bind.hpp"

#include "ukive/animation/animation_director.h"
#include "ukive/animation/view_animator.h"
#include "ukive/elements/ripple_element.h"
#include "ukive/event/input_event.h"
#include "ukive/views/button.h"
#include "ukive/views/combo_box_selected_listener.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/list/list_item.h"
#include "ukive/window/window.h"


namespace ukive {

    // ComboBox
    ComboBox::ComboBox(Context c)
        : ComboBox(c, {}) {
    }

    ComboBox::ComboBox(Context c, AttrsRef attrs)
        : LayoutView(c, attrs),
          text_view_(nullptr),
          button_(nullptr),
          weak_ref_nest_(this)
    {
        initViews();
    }

    ComboBox::~ComboBox() {
    }

    void ComboBox::initViews() {
        default_item_.title = u"ComboBox";

        auto c = getContext();

        // TextView
        text_view_ = new TextView(c);
        text_view_->setText(default_item_.title);
        text_view_->setLayoutSize(LS_AUTO, LS_AUTO);
        addView(text_view_);

        // DropdownButton
        button_ = new DropdownButton(c);
        button_->setOnClickListener(this);
        button_->setLayoutSize(LS_AUTO, LS_FILL);
        button_->setEnabled(false);
        addView(button_);

        list_view_ = new ListView(c);
        list_view_->setLayouter(new LinearListLayouter());
        list_view_->setSource(this);
        list_view_->setItemEventRouter(new ListItemEventRouter(this));

        auto shape_element = new Element(Element::SHAPE_RECT, Color::White);
        shape_element->setRadius(c.dp2pxi(2.f));

        levitator_ = std::make_shared<Levitator>();
        levitator_->setShadowRadius(c.dp2pxi(2.f));
        levitator_->setContentView(list_view_);
        levitator_->setOutsideTouchable(false);
        levitator_->setDismissByTouchOutside(true);
        levitator_->setBackground(shape_element);
        levitator_->setLayoutHeight(LS_AUTO);
        levitator_->setEventListener(this);
        levitator_->setLayoutMargin(
            { c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8) });
        min_dropdown_width_ = c.dp2pxi(100);
    }

    void ComboBox::addItem(const std::u16string_view& title) {
        addItemItl(title, {});
    }

    void ComboBox::addItem(size_t index, const std::u16string_view& title) {
        addItemItl(index, title, {});
    }

    bool ComboBox::modifyItem(size_t index, const std::u16string_view& title) {
        if (index >= items_.size()) {
            return false;
        }
        items_[index].title = title;
        notifyDataChanged();
        return true;
    }

    void ComboBox::removeItem(size_t index) {
        if (index >= items_.size()) {
            return;
        }
        if (selected_index_ >= 0) {
            if (utl::num_cast<size_t>(selected_index_) == index) {
                selected_index_ = -1;
                text_view_->setText(default_item_.title);
            } else if (index < utl::num_cast<size_t>(selected_index_)) {
                --selected_index_;
            }
        }

        items_.erase(items_.begin() + index);
        notifyDataChanged();

        if (items_.empty()) {
            button_->setEnabled(false);
        }
    }

    void ComboBox::clearItems() {
        items_.clear();
        selected_index_ = -1;
        text_view_->setText(default_item_.title);
        notifyDataChanged();
        button_->setEnabled(false);
    }

    void ComboBox::setDefaultItem(const std::u16string_view& title) {
        default_item_.title = title;
        if (selected_index_ < 0) {
            text_view_->setText(default_item_.title);
        }
    }

    void ComboBox::setDropdownMinWidth(int min_width) {
        if (min_dropdown_width_ != min_width) {
            min_dropdown_width_ = min_width;
            requestLayout();
        }
    }

    void ComboBox::setListener(ComboBoxSelectedListener* l) {
        listener_ = l;
    }

    void ComboBox::setSelectedIndex(int index) {
        if (index < 0) {
            if (selected_index_ < 0) {
                return;
            }
        } else if (index == selected_index_) {
            return;
        }

        if (index < 0 || items_.empty()) {
            selected_index_ = -1;
            text_view_->setText(default_item_.title);
        } else {
            selected_index_ = index;
            if (utl::num_cast<size_t>(selected_index_) >= items_.size()) {
                selected_index_ = utl::num_cast<int>(items_.size()) - 1u;
            }
            text_view_->setText(items_[selected_index_].title);
        }
    }

    size_t ComboBox::getItemCount() const {
        return items_.size();
    }

    const std::u16string& ComboBox::getItemTitle(size_t index) const {
        if (index > items_.size()) {
            index = items_.size();
        }
        return items_[index].title;
    }

    const std::u16string& ComboBox::getDefaultTitle(size_t index) const {
        return default_item_.title;
    }

    int ComboBox::getSelectedIndex() const {
        return selected_index_;
    }

    bool ComboBox::hasItemData(size_t index) const {
        if (index > items_.size()) {
            return false;
        }
        return !!items_[index].data;
    }

    bool ComboBox::hasDefaultData() const {
        return !!default_item_.data;
    }

    void ComboBox::determineViewsSize(const SizeInfo& info) {
        int padding_w = getPadding().hori();
        int padding_h = getPadding().vert();

        // DropdownButton
        auto& db_margin = button_->getLayoutMargin();

        int db_margin_w = db_margin.hori();
        auto db_width = SizeInfo::getChildSizeInfo(
            info.width(),
            padding_w + db_margin_w,
            getMinimumSize().width(),
            LS_AUTO);

        int db_margin_h = db_margin.vert();
        auto db_height = SizeInfo::getChildSizeInfo(
            info.height(),
            padding_h + db_margin_h,
            getMinimumSize().height(),
            LS_AUTO);

        button_->determineSize(SizeInfo(db_width, db_height));

        // TextView
        auto& tv_margin = text_view_->getLayoutMargin();

        int tv_margin_w = tv_margin.hori();
        auto tv_width = SizeInfo::getChildSizeInfo(
            SizeInfo::Value(
                info.width().val - button_->getDeterminedSize().width() - db_margin_w, info.width().mode),
            padding_w + tv_margin_w,
            getMinimumSize().width(),
            LS_FILL);

        int tv_margin_h = tv_margin.vert();
        auto tv_height = SizeInfo::getChildSizeInfo(
            info.height(),
            padding_h + tv_margin_h,
            getMinimumSize().height(),
            LS_AUTO);

        text_view_->determineSize(SizeInfo(tv_width, tv_height));
    }

    void ComboBox::addItemItl(const std::u16string_view& title, const DataPtr& data) {
        items_.push_back({ std::u16string(title), data });
        itemAdded();
    }

    void ComboBox::addItemItl(size_t index, const std::u16string_view& title, const DataPtr& data) {
        if (index > items_.size()) {
            index = items_.size();
        }
        if (selected_index_ >= 0 &&
            index <= utl::num_cast<size_t>(selected_index_))
        {
            ++selected_index_;
        }

        items_.insert(items_.begin() + index, { std::u16string(title), data });
        itemAdded();
    }

    void ComboBox::itemAdded() {
        notifyDataChanged();
        button_->setEnabled(true);
    }

    Size ComboBox::onDetermineSize(const SizeInfo& info) {
        determineViewsSize(info);

        int f_width, f_height;
        int padding_w = getPadding().hori();
        int padding_h = getPadding().vert();

        auto& db_size = button_->getDeterminedSize();
        auto& db_margin = button_->getLayoutMargin();
        int db_w = db_size.width() + db_margin.hori();
        int db_h = db_size.height() + db_margin.vert();

        auto& tv_size = text_view_->getDeterminedSize();
        auto& tv_margin = text_view_->getLayoutMargin();
        int tv_w = tv_size.width() + tv_margin.hori();
        int tv_h = tv_size.height() + tv_margin.vert();

        switch (info.width().mode) {
        case SizeInfo::DEFINED:
            f_width = info.width().val;
            break;

        case SizeInfo::CONTENT:
            f_width = (std::min)(info.width().val, padding_w + db_w + tv_w);
            break;

        case SizeInfo::FREEDOM:
        default:
            f_width = padding_w + db_w + tv_w;
            break;
        }

        switch (info.height().mode) {
        case SizeInfo::DEFINED:
            f_height = info.height().val;
            break;

        case SizeInfo::CONTENT:
            f_height = (std::min)(info.height().val, padding_h + (std::max)(tv_h, db_h));
            break;

        case SizeInfo::FREEDOM:
        default:
            f_height = padding_h + (std::max)(tv_h, db_h);
            break;
        }

        return Size(f_width, f_height);
    }

    void ComboBox::onLayout(const Rect& new_bounds, const Rect& old_bounds) {
        auto& tv_size = text_view_->getDeterminedSize();
        auto& tv_margin = text_view_->getLayoutMargin();
        int tv_h = tv_size.height() + tv_margin.vert();
        int tv_x = tv_margin.start() + getPadding().start();

        auto& db_size = button_->getDeterminedSize();
        auto& db_margin = button_->getLayoutMargin();
        int db_h = db_size.height() + db_margin.vert();
        int db_x = tv_x + tv_size.width() + db_margin.start();

        if (tv_h >= db_h) {
            int tv_y = tv_margin.top() + getPadding().top();
            text_view_->layout(
                Rect(tv_x, tv_y, tv_size.width(), tv_size.height()));

            int db_y = getPadding().top() + (tv_h - db_h) / 2;
            button_->layout(
                Rect(db_x, db_y,
                    db_size.width(),
                    db_margin.top() + db_size.height()));
        } else {
            int db_y = db_margin.top() + getPadding().top();
            button_->layout(
                Rect(db_x, db_y, db_size.width(), db_size.height()));

            int tv_y = getPadding().top() + (db_h - tv_h) / 2;
            text_view_->layout(
                Rect(tv_x, tv_y, tv_size.width(), tv_margin.top() + tv_size.height()));
        }
    }

    void ComboBox::show(int width) {
        auto w = getWindow();
        if (!is_finished_ || !w) {
            return;
        }

        is_finished_ = false;

        using namespace std::chrono_literals;

        levitator_->dismiss();
        levitator_->setLayoutWidth(width);
        levitator_->show(this, GV_MID_HORI | GV_BOTTOM);
        levitator_->getFrameView()->animate()
            .rectReveal(
                tval::ofReal(0), tval::ofReal(0),
                tval::ofAuto(), tval::ofAuto(),
                tval::ofReal(0), tval::ofAuto(), 150ms).start();
        list_view_->setEnabled(true);
    }

    void ComboBox::close() {
        if (is_finished_) {
            return;
        }

        is_finished_ = true;

        list_view_->setEnabled(false);
        levitator_->dismissing();

        std::weak_ptr<Levitator> ptr = levitator_;
        auto frame_view = levitator_->getFrameView();
        if (frame_view) {
            using namespace std::chrono_literals;
            frame_view->animate()
                .alpha(0.f, 100ms).setFinishedHandler(
                    [ptr](AnimationDirector* director)
            {
                auto window = ptr.lock();
                if (window) {
                    window->dismiss();
                }
            }).start();
        }
    }

    void ComboBox::onClick(View* v) {
        if (v == button_) {
            if (is_finished_) {
                if (!items_.empty()) {
                    auto bounds = getBoundsInRoot();
                    auto width = (std::max)(bounds.width(), min_dropdown_width_);
                    show(width);
                }
            } else {
                close();
            }
        }
    }

    void ComboBox::onItemClicked(ListView* list_view, ListItem* item, View* v) {
        if (item->data_pos < items_.size()) {
            text_view_->setText(items_[item->data_pos].title);
            selected_index_ = utl::num_cast<int>(item->data_pos);
            close();

            if (listener_) {
                listener_->onComboItemSelected(this);
            }
        }
    }

    void ComboBox::onRequestDismissByTouchOutside(Levitator* lev) {
        close();
    }

    ListItem* ComboBox::onCreateListItem(
        LayoutView* parent, ListItemEventRouter* router, size_t position)
    {
        auto c = parent->getContext();

        auto title_tv = new TextView(c);
        title_tv->setPadding(c.dp2pxi(16), c.dp2pxi(8), c.dp2pxi(16), c.dp2pxi(8));
        title_tv->setClickable(true);
        title_tv->setOnClickListener(router);
        title_tv->setBackground(new RippleElement());
        title_tv->setLayoutSize(LS_FILL, LS_AUTO);

        return new TextViewListItem(title_tv);
    }

    void ComboBox::onSetListItemData(
        LayoutView* parent, ListItemEventRouter* router, ListItem* item)
    {
        auto& data = items_.at(item->data_pos);
        auto combo_item = static_cast<TextViewListItem*>(item);
        combo_item->title_label->setText(data.title);
    }

    size_t ComboBox::onGetListDataCount(LayoutView* parent) const {
        return items_.size();
    }

}
