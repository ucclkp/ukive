#include "message_dialog.h"

#include "ukive/views/text_view.h"
#include "ukive/views/button.h"
#include "ukive/views/title_bar/circle_color_button.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/window/window.h"


namespace ukive {

    // static
    void MessageDialog::showDialog(
        Window* parent,
        const std::u16string_view& title,
        const std::u16string_view& text,
        const std::u16string_view& btn_text)
    {
        auto d = new MessageDialog(parent->getContext());
        d->setOwnership(true);
        d->setTitle(title);
        d->setText(text);
        d->setButtonText(btn_text);
        d->show(parent, GV_MID_HORI | GV_MID_VERT);
    }

    MessageDialog::MessageDialog(Context c)
        : Dialog(c) {}

    void MessageDialog::setTitle(const std::u16string_view& title) {
        title_ = title;
        if (title_tv_) {
            title_tv_->setText(title);
        }
    }

    void MessageDialog::setText(const std::u16string_view& text) {
        text_ = text;
        if (text_tv_) {
            text_tv_->setText(text);
        }
    }

    void MessageDialog::setButtonText(const std::u16string_view& text) {
        btn_text_ = text;
        if (btn_) {
            btn_->setText(text);
        }
    }

    View* MessageDialog::onCreate(Context c) {
        auto rv = new RestraintLayout(c);

        using Rli = RestraintLayoutInfo;

        // 左上角标题
        title_tv_ = new TextView(c);
        title_tv_->setText(title_);
        title_tv_->setLayoutSize(View::LS_AUTO, View::LS_AUTO);
        title_tv_->setLayoutMargin(c.dp2pxi(8), c.dp2pxi(8), 0, 0);
        title_tv_->setExtraLayoutInfo(
            Rli::Builder()
            .start(rv->getId()).top(rv->getId()).build());
        rv->addView(title_tv_);

        // 右上角关闭按钮
        close_btn_ = new CircleColorButton(c);
        close_btn_->setColor(Color::Red500);
        close_btn_->setOnClickListener(this);
        close_btn_->setLayoutSize(View::LS_AUTO, View::LS_AUTO);
        close_btn_->setLayoutMargin(0, c.dp2pxi(8), c.dp2pxi(8), 0);
        close_btn_->setExtraLayoutInfo(
            Rli::Builder()
            .top(rv->getId()).end(rv->getId()).build());
        rv->addView(close_btn_);

        // 正文
        text_tv_ = new TextView(c);
        text_tv_->setText(text_);
        text_tv_->setLayoutSize(View::LS_AUTO, View::LS_AUTO);
        text_tv_->setLayoutMargin(c.dp2pxi(16), c.dp2pxi(12), c.dp2pxi(16), 0);
        text_tv_->setExtraLayoutInfo(
            Rli::Builder()
            .start(rv->getId()).top(title_tv_->getId(), Rli::BOTTOM).end(rv->getId()).build());
        rv->addView(text_tv_);

        // 按钮
        btn_ = new Button(c);
        btn_->setText(btn_text_);
        btn_->setOnClickListener(this);
        btn_->setLayoutSize(View::LS_AUTO, View::LS_AUTO);
        btn_->setLayoutMargin(0, c.dp2pxi(24), c.dp2pxi(8), c.dp2pxi(8));
        btn_->setExtraLayoutInfo(
            Rli::Builder()
            .top(text_tv_->getId(), Rli::BOTTOM)
            .end(rv->getId())
            .bottom(rv->getId()).build());
        rv->addView(btn_);

        return rv;
    }

    void MessageDialog::onDestroy() {

    }

    void MessageDialog::onClick(View* v) {
        if (v == close_btn_ || v == btn_) {
            close();
        }
    }

}
