// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/combo_box/combo_box.h"

#include <algorithm>

#include "utils/weak_bind.hpp"

#include "ukive/animation/animation_director.h"
#include "ukive/animation/view_animator.h"
#include "ukive/elements/ripple_element.h"
#include "ukive/elements/shape_element.h"
#include "ukive/event/input_event.h"
#include "ukive/views/button.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/list/list_item.h"
#include "ukive/views/combo_box/combo_list_source.h"
#include "ukive/views/text_view.h"
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
        // TextView
        text_view_ = new TextView(getContext());
        text_view_->setText(u"ComboBox");
        text_view_->setLayoutSize(LS_AUTO, LS_AUTO);
        addView(text_view_);

        // DropdownButton
        button_ = new DropdownButton(getContext());
        button_->setOnClickListener(this);
        button_->setLayoutSize(LS_AUTO, LS_FILL);
        addView(button_);

        source_ = new ComboListSource();

        list_view_ = new ListView(getContext());
        list_view_->setLayouter(new LinearListLayouter());
        list_view_->setSource(source_);
        list_view_->setItemSelectedListener(this);

        auto shape_element = new ShapeElement(ShapeElement::RECT);
        shape_element->setRadius(2.f);
        shape_element->setSolidEnable(true);
        shape_element->setSolidColor(Color::White);

        inner_window_ = std::make_shared<InnerWindow>();
        inner_window_->setShadowRadius(getContext().dp2px(2.f));
        inner_window_->setContentView(list_view_);
        inner_window_->setOutsideTouchable(false);
        inner_window_->setDismissByTouchOutside(true);
        inner_window_->setBackground(shape_element);
        inner_window_->setHeight(getContext().dp2pxi(100));
        inner_window_->setEventListener(this);

        min_dropdown_width_ = getContext().dp2pxi(100);
    }

    void ComboBox::addItem(const std::u16string& title) {
        source_->addItem(title);
    }

    void ComboBox::determineViewsSize(const SizeInfo& info) {
        int padding_w = getPadding().hori();
        int padding_h = getPadding().vert();

        // DropdownButton
        auto& db_margin = button_->getLayoutMargin();

        int db_margin_w = db_margin.hori();
        auto db_width = SizeInfo::getChildSizeInfo(info.width, padding_w + db_margin_w, LS_AUTO);

        int db_margin_h = db_margin.top + db_margin.bottom;
        auto db_height = SizeInfo::getChildSizeInfo(info.height, padding_h + db_margin_h, LS_AUTO);

        button_->measure(SizeInfo(db_width, db_height));

        // TextView
        auto& tv_margin = text_view_->getLayoutMargin();

        int tv_margin_w = tv_margin.hori();
        auto tv_width = SizeInfo::getChildSizeInfo(
            SizeInfo::Value(
                info.width.val - button_->getDeterminedSize().width - db_margin_w, info.width.mode),
            padding_w + tv_margin_w, LS_FILL);

        int tv_margin_h = tv_margin.vert();
        auto tv_height = SizeInfo::getChildSizeInfo(info.height, padding_h + tv_margin_h, LS_AUTO);

        text_view_->measure(SizeInfo(tv_width, tv_height));
    }

    Size ComboBox::onDetermineSize(const SizeInfo& info) {
        determineViewsSize(info);

        int f_width, f_height;
        int padding_w = getPadding().hori();
        int padding_h = getPadding().vert();

        auto& db_size = button_->getDeterminedSize();
        auto& db_margin = button_->getLayoutMargin();
        int db_w = db_size.width + db_margin.hori();
        int db_h = db_size.height + db_margin.vert();

        auto& tv_size = text_view_->getDeterminedSize();
        auto& tv_margin = text_view_->getLayoutMargin();
        int tv_w = tv_size.width + tv_margin.hori();
        int tv_h = tv_size.height + tv_margin.vert();

        switch (info.width.mode) {
        case SizeInfo::DEFINED:
            f_width = info.width.val;
            break;

        case SizeInfo::CONTENT:
            f_width = std::min(info.width.val, padding_w + db_w + tv_w);
            break;

        case SizeInfo::FREEDOM:
        default:
            f_width = padding_w + db_w + tv_w;
            break;
        }

        switch (info.height.mode) {
        case SizeInfo::DEFINED:
            f_height = info.height.val;
            break;

        case SizeInfo::CONTENT:
            f_height = std::min(info.height.val, padding_h + std::max(tv_h, db_h));
            break;

        case SizeInfo::FREEDOM:
        default:
            f_height = padding_h + std::max(tv_h, db_h);
            break;
        }

        return Size(f_width, f_height);
    }

    void ComboBox::onLayout(const Rect& new_bounds, const Rect& old_bounds) {
        auto& tv_size = text_view_->getDeterminedSize();
        auto& tv_margin = text_view_->getLayoutMargin();
        int tv_h = tv_size.height + tv_margin.vert();
        int tv_x = tv_margin.start + getPadding().start;

        auto& db_size = button_->getDeterminedSize();
        auto& db_margin = button_->getLayoutMargin();
        int db_h = db_size.height + db_margin.vert();
        int db_x = tv_x + tv_size.width + db_margin.start;

        if (tv_h >= db_h) {
            int tv_y = tv_margin.top + getPadding().top;
            text_view_->layout(
                Rect(tv_x, tv_y, tv_size.width, tv_size.height));

            int db_y = getPadding().top + (tv_h - db_h) / 2;
            button_->layout(
                Rect(db_x, db_y,
                    db_size.width,
                    db_margin.top + db_size.height));
        } else {
            int db_y = db_margin.top + getPadding().top;
            button_->layout(
                Rect(db_x, db_y, db_size.width, db_size.height));

            int tv_y = getPadding().top + (db_h - tv_h) / 2;
            text_view_->layout(
                Rect(tv_x, tv_y, tv_size.width, tv_margin.top + tv_size.height));
        }
    }

    void ComboBox::show(int x, int y, int width) {
        auto w = getWindow();
        if (!is_finished_ || !w) {
            return;
        }

        is_finished_ = false;

        int center_x = 0, center_y = 0;
        auto bounds = w->getContentBounds();
        if (x + width > bounds.width()) {
            center_x = width;
            center_y = 0;
            x -= width;
        }

        int height = inner_window_->getHeight();

        inner_window_->dismiss();
        inner_window_->setWidth(width);
        inner_window_->show(w, x, y);
        inner_window_->getDecorView()->animate()->setDuration(150)->
            rectReveal(center_x, center_y, width, width, 0, height)->start();
        list_view_->setEnabled(true);

        auto last_input_view = w->getLastInputView();
        if (last_input_view) {
            InputEvent e;
            e.setEvent(InputEvent::EV_LEAVE_VIEW);
            e.setIsNoDispatch(true);
            last_input_view->dispatchInputEvent(&e);
        }
    }

    void ComboBox::close() {
        if (is_finished_) {
            return;
        }

        is_finished_ = true;

        list_view_->setEnabled(false);
        inner_window_->markDismissing();

        std::weak_ptr<InnerWindow> ptr = inner_window_;
        auto dec_view = inner_window_->getDecorView();
        if (dec_view) {
            dec_view->animate()->
                setDuration(100)->alpha(0.f)->setFinishedHandler(
                    [ptr](AnimationDirector* director)
            {
                auto window = ptr.lock();
                if (window) {
                    window->dismiss();
                }
            })->start();
        }
    }

    void ComboBox::onClick(View* v) {
        if (v == button_) {
            if (is_finished_) {
                auto bounds = getBoundsInRoot();
                auto width = std::max(bounds.width(), min_dropdown_width_);
                show(bounds.left, bounds.bottom, width);
            } else {
                close();
            }
        }
    }

    void ComboBox::onRequestDismissByTouchOutside(InnerWindow* iw) {
        close();
    }

    void ComboBox::onItemClicked(ListView* lv, ListItem* item) {
        int pos = item->data_pos;
        if (pos >= 0 && pos < source_->onListGetDataCount()) {
            text_view_->setText(source_->getItemData(pos));
            close();
        }
    }

}
