// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "lod_window.h"

#include <sstream>

#include "utils/convert.h"
#include "utils/string_utils.h"

#include "ukive/app/application.h"
#include "ukive/graphics/color.h"
#include "ukive/views/layout/sequence_layout.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/views/layout_info/sequence_layout_info.h"
#include "ukive/views/seek_bar.h"
#include "ukive/views/button.h"
#include "ukive/views/text_view.h"
#include "ukive/elements/color_element.h"
#include "ukive/elements/texteditor_element.h"
#include "ukive/views/space3d_view.h"
#include "ukive/system/time_utils.h"

#include "shell/lod/terrain_scene.h"
#include "shell/lod/lod_generator.h"


namespace {
    enum {
        ID_LOD_INFO = 0x010,
        ID_RIGHT_RESTRAIN,

        ID_C1_LABEL,
        ID_C1_SEEKBAR,
        ID_C1_VALUE,

        ID_C2_LABEL,
        ID_C2_SEEKBAR,
        ID_C2_VALUE,

        ID_SPLIT_LABEL,
        ID_SPLIT_SEEKBAR,
        ID_SPLIT_VALUE,

        ID_SUBMIT_BUTTON,
        ID_VSYNC_BUTTON,

        ID_MONITOR,
        ID_HELPER,
    };
}

namespace shell {

    LodWindow::LodWindow()
        : Window() {}

    LodWindow::~LodWindow() {
        terrain_scene_->setRenderListener(nullptr);
    }

    void LodWindow::onCreated() {
        Window::onCreated();

        showTitleBar();

        //root layout.
        ukive::SequenceLayout *rootLayout = new ukive::SequenceLayout(getContext());
        rootLayout->setOrientation(ukive::SequenceLayout::HORIZONTAL);
        rootLayout->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_FILL);

        terrain_scene_ = new TerrainScene();
        terrain_scene_->setRenderListener(std::bind(&LodWindow::onRender, this));

        //3d view.
        ukive::Space3DView* lodView = new ukive::Space3DView(getContext(), terrain_scene_);
        lodView->setLayoutSize(0, ukive::View::LS_FILL);
        lodView->setLayoutMargin(
            getContext().dp2px(8), getContext().dp2px(8), getContext().dp2px(8), getContext().dp2px(8));
        ukive::SequenceLayoutInfo *d3dViewLp = new ukive::SequenceLayoutInfo();
        d3dViewLp->weight = 2;
        lodView->setExtraLayoutInfo(d3dViewLp);
        lodView->setBackground(new ukive::ColorElement(ukive::Color::White));
        lodView->setShadowRadius(getContext().dp2px(2));

        lod_view_ = lodView;

        //right view.
        ukive::RestraintLayout *rightLayout = new ukive::RestraintLayout(getContext());
        rightLayout->setId(ID_RIGHT_RESTRAIN);
        rightLayout->setLayoutSize(0, ukive::View::LS_FILL);
        ukive::SequenceLayoutInfo *rightViewLp = new ukive::SequenceLayoutInfo();
        rightViewLp->weight = 1;
        rightLayout->setExtraLayoutInfo(rightViewLp);

        ////////////////////Right View Panel.

        inflateCtlLayout(rightLayout);

        ////////////////////End Panel////////////////////

        rootLayout->addView(lodView);
        rootLayout->addView(rightLayout);

        setContentView(rootLayout);
    }


    void LodWindow::inflateCtlLayout(ukive::RestraintLayout *rightLayout) {
        using Rlp = ukive::RestraintLayoutInfo;

        /////////////////////////////第一行//////////////////////////////////
        //c1 label.
        ukive::TextView *c1Label = new ukive::TextView(getContext());
        c1Label->setId(ID_C1_LABEL);
        c1Label->setIsEditable(false);
        c1Label->setIsSelectable(false);
        c1Label->setText(u"C1值:");
        c1Label->setTextSize(getContext().dp2px(13));
        c1Label->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);
        c1Label->setLayoutMargin(getContext().dp2px(8), getContext().dp2px(12), 0, 0);

        auto c1LabelLp = Rlp::Builder()
            .start(ID_RIGHT_RESTRAIN, Rlp::START)
            .top(ID_RIGHT_RESTRAIN, Rlp::TOP).build();

        c1Label->setExtraLayoutInfo(c1LabelLp);
        rightLayout->addView(c1Label);

        //c1 seekbar.
        c1_seekbar_ = new ukive::SeekBar(getContext());
        c1_seekbar_->setId(ID_C1_SEEKBAR);
        c1_seekbar_->setMaximum(60.f);
        c1_seekbar_->setProgress(2.f - 1.f);
        c1_seekbar_->setOnSeekValueChangedListener(this);
        c1_seekbar_->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        c1_seekbar_->setLayoutMargin(getContext().dp2px(4), 0, getContext().dp2px(4), 0);

        auto c1SeekBarLp = Rlp::Builder()
            .start(ID_C1_LABEL, Rlp::END)
            .top(ID_C1_LABEL)
            .end(ID_C1_VALUE, Rlp::START)
            .bottom(ID_C1_LABEL).build();

        c1_seekbar_->setExtraLayoutInfo(c1SeekBarLp);
        rightLayout->addView(c1_seekbar_);

        //c1 value.
        c1_value_tv_ = new ukive::TextView(getContext());
        c1_value_tv_->setId(ID_C1_VALUE);
        c1_value_tv_->setIsEditable(false);
        c1_value_tv_->setIsSelectable(false);
        c1_value_tv_->setText(u"2.00");
        c1_value_tv_->setTextSize(getContext().dp2px(13));
        c1_value_tv_->setLayoutSize(getContext().dp2px(36), ukive::View::LS_AUTO);
        c1_value_tv_->setLayoutMargin(0, 0, getContext().dp2px(8), 0);

        auto c1ValueLp = Rlp::Builder()
            .top(ID_C1_LABEL)
            .end(ID_RIGHT_RESTRAIN, Rlp::END)
            .bottom(ID_C1_LABEL).build();

        c1_value_tv_->setExtraLayoutInfo(c1ValueLp);
        rightLayout->addView(c1_value_tv_);

        /////////////////////////////第二行//////////////////////////////////
        //c2 label.
        auto c2Label = new ukive::TextView(getContext());
        c2Label->setId(ID_C2_LABEL);
        c2Label->setIsEditable(false);
        c2Label->setIsSelectable(false);
        c2Label->setText(u"C2值:");
        c2Label->setTextSize(getContext().dp2px(13));
        c2Label->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);
        c2Label->setLayoutMargin(getContext().dp2px(8), getContext().dp2px(8), 0, 0);

        auto c2LabelLp = Rlp::Builder()
            .start(ID_RIGHT_RESTRAIN, Rlp::START)
            .top(ID_C1_LABEL, Rlp::BOTTOM).build();

        c2Label->setExtraLayoutInfo(c2LabelLp);
        rightLayout->addView(c2Label);

        //c2 seekbar.
        c2_seekbar_ = new ukive::SeekBar(getContext());
        c2_seekbar_->setId(ID_C2_SEEKBAR);
        c2_seekbar_->setMaximum(60.f);
        c2_seekbar_->setProgress(30.f - 1.f);
        c2_seekbar_->setOnSeekValueChangedListener(this);
        c2_seekbar_->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        c2_seekbar_->setLayoutMargin(getContext().dp2px(4), 0, getContext().dp2px(4), 0);

        auto c2SeekBarLp = Rlp::Builder()
            .start(ID_C2_LABEL, Rlp::END)
            .top(ID_C2_LABEL)
            .end(ID_C2_VALUE, Rlp::START)
            .bottom(ID_C2_LABEL).build();

        c2_seekbar_->setExtraLayoutInfo(c2SeekBarLp);
        rightLayout->addView(c2_seekbar_);

        //c2 value.
        c2_value_tv_ = new ukive::TextView(getContext());
        c2_value_tv_->setId(ID_C2_VALUE);
        c2_value_tv_->setIsEditable(false);
        c2_value_tv_->setIsSelectable(false);
        c2_value_tv_->setText(u"30.00");
        c2_value_tv_->setTextSize(getContext().dp2px(13));
        c2_value_tv_->setLayoutSize(getContext().dp2px(36), ukive::View::LS_AUTO);
        c2_value_tv_->setLayoutMargin(0, 0, getContext().dp2px(8), 0);

        auto c2ValueLp = Rlp::Builder()
            .top(ID_C2_LABEL)
            .end(ID_RIGHT_RESTRAIN, Rlp::END)
            .bottom(ID_C2_LABEL).build();

        c2_value_tv_->setExtraLayoutInfo(c2ValueLp);
        rightLayout->addView(c2_value_tv_);

        /////////////////////////////第三行//////////////////////////////////
        //split label.
        auto splitLabel = new ukive::TextView(getContext());
        splitLabel->setId(ID_SPLIT_LABEL);
        splitLabel->setIsEditable(false);
        splitLabel->setIsSelectable(false);
        splitLabel->setText(u"分割:");
        splitLabel->setTextSize(getContext().dp2px(13));
        splitLabel->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);
        splitLabel->setLayoutMargin(getContext().dp2px(8), getContext().dp2px(16), 0, 0);

        auto splitLabelLp = Rlp::Builder()
            .start(ID_RIGHT_RESTRAIN, Rlp::START)
            .top(ID_C2_LABEL, Rlp::BOTTOM).build();

        splitLabel->setExtraLayoutInfo(splitLabelLp);
        rightLayout->addView(splitLabel);

        //split seekbar.
        split_seekbar_ = new ukive::SeekBar(getContext());
        split_seekbar_->setId(ID_SPLIT_SEEKBAR);
        split_seekbar_->setMaximum(10.f);
        split_seekbar_->setProgress(5.f - 1.f);
        split_seekbar_->setOnSeekValueChangedListener(this);
        split_seekbar_->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        split_seekbar_->setLayoutMargin(0, 0, getContext().dp2px(4), 0);

        auto splitSeekBarLp = Rlp::Builder()
            .start(ID_C2_SEEKBAR)
            .top(ID_SPLIT_LABEL)
            .end(ID_SPLIT_VALUE, Rlp::START)
            .bottom(ID_SPLIT_LABEL).build();

        split_seekbar_->setExtraLayoutInfo(splitSeekBarLp);
        rightLayout->addView(split_seekbar_);

        //split value.
        split_value_tv_ = new ukive::TextView(getContext());
        split_value_tv_->setId(ID_SPLIT_VALUE);
        split_value_tv_->setIsEditable(false);
        split_value_tv_->setIsSelectable(false);
        split_value_tv_->setText(u"5");
        split_value_tv_->setTextSize(getContext().dp2px(13));
        split_value_tv_->setLayoutSize(getContext().dp2px(36), ukive::View::LS_AUTO);
        split_value_tv_->setLayoutMargin(0, 0, getContext().dp2px(8), 0);

        auto splitValueLp = Rlp::Builder()
            .end(ID_RIGHT_RESTRAIN, Rlp::END)
            .top(ID_SPLIT_LABEL)
            .bottom(ID_SPLIT_LABEL).build();

        split_value_tv_->setExtraLayoutInfo(splitValueLp);
        rightLayout->addView(split_value_tv_);


        //submit button.
        auto submitBT = new ukive::Button(getContext());
        submitBT->setId(ID_SUBMIT_BUTTON);
        submitBT->setText(u"提交");
        submitBT->setTextSize(getContext().dp2px(12));
        submitBT->setOnClickListener(this);
        submitBT->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);
        submitBT->setLayoutMargin(0, getContext().dp2px(16), getContext().dp2px(8), 0);

        auto submitBTLp = Rlp::Builder()
            .end(ID_RIGHT_RESTRAIN, Rlp::END)
            .top(ID_SPLIT_LABEL, Rlp::BOTTOM).build();

        submitBT->setExtraLayoutInfo(submitBTLp);
        rightLayout->addView(submitBT);

        //vsync button.
        auto vsyncBT = new ukive::Button(getContext());
        vsyncBT->setId(ID_VSYNC_BUTTON);
        vsyncBT->setText(u"VSYNC ON");
        vsyncBT->setTextSize(getContext().dp2px(12));
        vsyncBT->setTextColor(ukive::Color::White);
        vsyncBT->setTextWeight(ukive::TextLayout::FontWeight::BOLD);
        vsyncBT->setButtonColor(ukive::Color::Blue500);
        vsyncBT->setOnClickListener(this);
        vsyncBT->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);
        vsyncBT->setLayoutMargin(0, getContext().dp2px(8), getContext().dp2px(8), 0);

        auto vsyncBTLp = Rlp::Builder()
            .end(ID_RIGHT_RESTRAIN, Rlp::END)
            .top(ID_SUBMIT_BUTTON, Rlp::BOTTOM).build();

        vsyncBT->setExtraLayoutInfo(vsyncBTLp);
        rightLayout->addView(vsyncBT);


        //渲染参数显示器
        render_info_ = new ukive::TextView(getContext());
        render_info_->setId(ID_LOD_INFO);
        render_info_->setIsEditable(false);
        render_info_->setIsSelectable(false);
        render_info_->setText(u"Render info.");
        render_info_->setTextSize(getContext().dp2px(12));
        render_info_->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        render_info_->setLayoutMargin(
            getContext().dp2px(8), getContext().dp2px(16), getContext().dp2px(8), 0);

        auto renderInfoTVLp = Rlp::Builder()
            .start(ID_RIGHT_RESTRAIN, Rlp::START)
            .top(ID_VSYNC_BUTTON, Rlp::BOTTOM)
            .end(ID_RIGHT_RESTRAIN, Rlp::END).build();

        render_info_->setExtraLayoutInfo(renderInfoTVLp);
        rightLayout->addView(render_info_);


        //帮助说明
        auto helperTV = new ukive::TextView(getContext());
        helperTV->setId(ID_HELPER);
        helperTV->setIsEditable(true);
        helperTV->setIsSelectable(true);
        helperTV->setText(
            u"►拖动滑条，点击提交按钮来更改参数。\
        \n►鼠标滚轮可进行缩放。\
        \n►按住Shift键和鼠标左键拖动可旋转摄像机。\
        \n►按住Ctrl键和鼠标左键拖动可移动摄像机");
        helperTV->setTextSize(getContext().dp2px(14));
        helperTV->setBackground(new ukive::TextEditorElement(getContext()));
        helperTV->setPadding(
            getContext().dp2px(4), getContext().dp2px(4),
            getContext().dp2px(4), getContext().dp2px(4));
        helperTV->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        helperTV->setLayoutMargin(
            getContext().dp2px(8), getContext().dp2px(24), getContext().dp2px(8), 0);

        auto helperTVLp = Rlp::Builder()
            .start(ID_RIGHT_RESTRAIN, Rlp::START)
            .top(ID_LOD_INFO, Rlp::BOTTOM)
            .end(ID_RIGHT_RESTRAIN, Rlp::END).build();

        helperTV->setExtraLayoutInfo(helperTVLp);
        rightLayout->addView(helperTV);
    }


    void LodWindow::onClick(ukive::View *v) {
        switch (v->getId()) {
        case ID_SUBMIT_BUTTON:
        {
            float c1 = c1_seekbar_->getProgress() + 1.f;
            float c2 = c2_seekbar_->getProgress() + 1.f;
            int splitCount = static_cast<int>(split_seekbar_->getProgress()) + 1;

            terrain_scene_->recreate(splitCount);
            terrain_scene_->reevaluate(c1, c2);
            lod_view_->requestDraw();

            break;
        }

        case ID_VSYNC_BUTTON:
        {
            ukive::Button *vsyncButton = static_cast<ukive::Button*>(v);
            if (vsyncButton->getText() == u"VSYNC ON") {
                ukive::Application::setVSync(false);
                vsyncButton->setText(u"VSYNC OFF");
                vsyncButton->setButtonColor(ukive::Color::Yellow800);
            } else if (vsyncButton->getText() == u"VSYNC OFF") {
                ukive::Application::setVSync(true);
                vsyncButton->setText(u"VSYNC ON");
                vsyncButton->setButtonColor(ukive::Color::Blue500);
            }
            break;
        }
        default:
            break;
        }
    }

    void LodWindow::onSeekValueChanged(ukive::SeekBar *seekBar, float value) {
        switch (seekBar->getId()) {
        case ID_C1_SEEKBAR:
            c1_value_tv_->setText(utl::stringPrintf(u"%.2f", 1.f + value));
            break;

        case ID_C2_SEEKBAR:
            c2_value_tv_->setText(utl::stringPrintf(u"%.2f", 1.f + value));
            break;
        default:
            break;
        }
    }

    void LodWindow::onSeekIntegerValueChanged(ukive::SeekBar *seekBar, int value) {
        switch (seekBar->getId()) {
        case ID_SPLIT_SEEKBAR:
            split_value_tv_->setText(utl::to_u16string(1 + value));
            break;
        default:
            break;
        }
    }

    void LodWindow::onRender() {
        auto current_time = ukive::TimeUtils::upTimeMillis();
        if (prev_time_ > 0) {
            ++frame_counter_;
            if (current_time - prev_time_ > 500) {
                frame_pre_second_ = int((double(frame_counter_) / (current_time - prev_time_)) * 1000);
                frame_counter_ = 0;
                prev_time_ = current_time;

                std::stringstream ss;
                ss << "FPS: " << frame_pre_second_
                    << "\nTerrain Size: " << terrain_scene_->getLodGenerator()->getRowVertexCount()
                    << "x" << terrain_scene_->getLodGenerator()->getRowVertexCount()
                    << "\nTriangle Count: " << terrain_scene_->getLodGenerator()->getMaxIndexCount() / 3
                    << "\nRendered Triangle Count: " << terrain_scene_->getLodGenerator()->getIndexCount() / 3;

                render_info_->setText(utl::UTF8ToUTF16(ss.str()));
            }
        } else {
            prev_time_ = current_time;
        }

        //requestDraw();
    }
}