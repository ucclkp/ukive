// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "example_misc_page.h"

#include "utils/math/algebra/matrix.hpp"
#include "utils/math/algebra/optimization.hpp"
#include "utils/math/algebra/dynamic_optimization.hpp"
#include "utils/strings/string_utils.hpp"

#include "ukive/animation/interpolator.h"
#include "ukive/app/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/views/button.h"
#include "ukive/views/text_view.h"
#include "ukive/views/check_box.h"
#include "ukive/views/combo_box.h"
#include "ukive/views/image_view.h"
#include "ukive/views/chart_view.h"
#include "ukive/views/scroll_view.h"
#include "ukive/views/progress_bar.h"
#include "ukive/text/span/effect_span.h"
#include "ukive/text/span/interactable_span.h"
#include "ukive/text/span/text_attributes_span.h"
#include "ukive/text/span/inline_object_span.h"
#include "ukive/resources/layout_instantiator.h"
#include "ukive/resources/resource_manager.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/display.h"
#include "ukive/elements/color_element.h"
#include "ukive/elements/texteditor_element.h"
#include "ukive/graphics/colors/color.h"
#include "ukive/window/window.h"
#include "ukive/graphics/colors/color_manager.h"
#include "ukive/graphics/colors/ucmm.h"
#include "ukive/system/dialogs/sys_message_dialog.h"

#include "shell/resources/necro_resources_id.h"

#define BEZIER_BASE_VELOCITY  20000
#define BEZIER_BASE_TIME      6


namespace {

    void fillChartView1(ukive::ChartView* v) {
        /*chart_view->addData(50, 32);
        chart_view->addData(60, 2);
        chart_view->addData(70, -897);
        chart_view->addData(80, 0);*/
    }

    void fillChartView2(ukive::ChartView* v) {
        double V = BEZIER_BASE_VELOCITY;
        double T = BEZIER_BASE_TIME;
        ukive::PointF p1{ 3, BEZIER_BASE_VELOCITY };
        ukive::PointF p2{ 3, 0 };
        for (int i = 0; i <= 100; ++i) {
            double s = i / double(100);
            auto tn = 3 * p1.x() *s*(1 - s)*(1 - s) + 3 * p2.x() *s*s*(1 - s) + T * s*s*s;
            auto vn = V * std::pow(1 - s, 3) + 3 * p1.y() *s*(1 - s)*(1 - s) + 3 * p2.y() *s*s*(1 - s);
            v->addData(tn, vn, false);
        }
        v->addData(0, V, true, true);
        v->addData(p1.x(), p1.y(), true, true);
        v->addData(p2.x(), p2.y(), true, true);
        v->addData(T, 0, true, true);
    }

    void fillChartView3(ukive::ChartView* v) {
        using namespace utl;

        const size_t M = 18;
        const size_t N = 10;

        math::VectorT<double, N> x;
        for (size_t i = 0; i < N; ++i) {
            x(i) = double(i + 3);
        }

        math::VectorT<double, N> T{
            23, 12, 8, 56, 96, 12, 99, 8, 31, 3
        };

        math::VectorT<double, M> W;
        math::Optimization::conjugateGradient(x, T, &W);

        for (size_t i = 0; i < N; ++i) {
            v->addData(x(i), T(i), true, true);
        }

        for (double i = x(0); i < x(N - 1); i += 0.1) {
            double result = 0;
            for (int m = 0; m < M; ++m) {
                result += W(m)*std::pow(i, m);
            }
            v->addData(i, result, false, false);
        }
    }

    void fillChartView4(ukive::ChartView* v) {
        using namespace utl;

        const size_t M = 5;
        const size_t N = 10;

        math::DMatrixT<double> x(N, 1);
        for (size_t i = 0; i < N; ++i) {
            x(i) = double(i + 3);
        }

        math::DMatrixT<double> T(N, 1);
        T = { 23, 12, 8, 56, 96, 12, 99, 8, 31, 3 };

        math::DMatrixT<double> W(M, 1);
        math::DOptimization::conjugateGradient(x, T, &W);

        for (size_t i = 0; i < N; ++i) {
            v->addData(x(i), T(i), true, true);
        }

        for (double i = x(0); i < x(N - 1); i += 0.1) {
            double result = 0;
            for (int m = 0; m < M; ++m) {
                result += W(m)*std::pow(i, m);
            }
            v->addData(i, result, false, false);
        }
    }


    class TestInlineObjectSpan : public ukive::InlineObjectSpan {
    public:
        TestInlineObjectSpan(int start, int end)
            : InlineObjectSpan(start, end) {}

        void onDrawInlineObject(ukive::Canvas* c, float x, float y) override {
            c->fillCircle({ x + 15, y }, 10.f, ukive::Color::Blue300);
        }

        void onGetMetrics(float* width, float* height, float* baseline) override {
            *width = 30.f;
            *height = 30.f;
            *baseline = 0;
        }
    };

}

namespace shell {

    ExampleMiscPage::ExampleMiscPage(ukive::Window* w)
        : Page(w) {}

    ukive::View* ExampleMiscPage::onCreate(ukive::LayoutView* parent) {
        using namespace std::chrono_literals;
        director_.setListener(this);
        auto animator = director_.add(0);
        director_.setInitValue(0, 0);
        animator->setDuration(5s);
        animator->setFinalValue(400);
        animator->setInterpolator(new ukive::LinearInterpolator());
        director_.addLoop(2s, 1s, 10);
        director_.start();
        startVSync();

        auto v = ukive::LayoutInstantiator::from(
            parent->getContext(), parent, Res::Layout::example_misc_page_layout_xml);

        auto sv = findView<ukive::ScrollView>(v, Res::Id::sv_misc_page);

        std::u16string output_name;
        std::u16string adapter_name;
        ukive::Display::fromWindow(getWindow())->getName(&output_name);
        ukive::Display::fromWindow(getWindow())->getAdapterName(&adapter_name);

        std::u16string deviceDesc;
        deviceDesc.append(u"Device: ").append(adapter_name)
            .append(u"\n").append(u"Monitor: ").append(output_name);

        auto deviceTextView = findView<ukive::TextView>(v, Res::Id::tv_dev_text_params);
        deviceTextView->setText(deviceDesc);

        auto chart_view = findView<ukive::ChartView>(v, Res::Id::cv_data_chart);
        chart_view->setBackground(new ukive::ColorElement(ukive::Color::Grey100));
        chart_view->setVisibility(ukive::View::VANISHED);

        //fillChartView1(chart_view);
        //fillChartView2(chart_view);
        //fillChartView3(chart_view);
        fillChartView4(chart_view);

        check_box_ = findView<ukive::CheckBox>(v, Res::Id::cb_misc_anim_test);
        check_box_->setChecked(true);

        combo_box_ = findView<ukive::ComboBox>(v, Res::Id::sv_misc_combobox);
        combo_box_->addItem(u"Test01");
        combo_box_->addItem(u"Test02");
        combo_box_->addItem(u"Test03");

        auto progress_bar = findView<ukive::ProgressBar>(v, Res::Id::pb_misc_anim_test);
        progress_bar->setProgress(50);

        auto textView = findView<ukive::TextView>(v, Res::Id::tv_misc_txt);
        textView->setText(u"这\n是一个示\n例程序，\n在这里可以显示文本。");
        textView->setBackground(new ukive::TextEditorElement(parent->getContext()));
        textView->setLineSpacing(ukive::TextLayout::LineSpacing::UNIFORM, 10);
        //textView->autoWrap(false);
        //textView->setTextAlignment(ukive::TextLayout::Alignment::CENTER);
        {
            auto span = new ukive::DrawingEffectsSpan(3, 4);
            span->has_underline = true;
            span->text_color = ukive::Color::Blue600;
            textView->getEditable()->addSpan(span);
        }
        {
            auto span = new ukive::DrawingEffectsSpan(4, 5);
            span->has_underline = true;
            span->underline_color = ukive::Color::Pink500;
            textView->getEditable()->addSpan(span);
        }
        {
            auto span = new ukive::InteractableSpan(4, 5);
            textView->getEditable()->addSpan(span);
        }
        {
            auto span = new TestInlineObjectSpan(8, 9);
            textView->getEditable()->addSpan(span);
        }
        textView->requestFocus();

        image_view_ = findView<ukive::ImageView>(v, Res::Id::iv_misc_img);

        auto rm = ukive::Application::getResourceManager();
        auto img_path = rm->getResRootPath() / u"freshpaint.png";
        auto img = ukive::ImageFrame::decodeFile(getWindow()->getCanvas(), img_path.u16string());
        image_view_->setImage(img);
        image_view_->animeParams().setOrder(ukive::ViewAnimatorParams::RST);

        test_button_ = findView<ukive::Button>(v, Res::Id::bt_misc_button);
        test_button_->setOnClickListener(this);

        ukive::Color dst;
        std::unique_ptr<ukive::ColorManager> cm(ukive::ColorManager::create());
        cm->convertColor(ukive::Color(0.25f, 0.89f, 0.47f), &dst);

        std::u16string icc_path;
        if (ukive::ColorManager::getDefaultProfile(&icc_path))
        {
            ukive::icc::ICCProfile pf;
            pf.load(icc_path);

            ukive::Color tc;
            ukive::UCMM cmm;
            cmm.sRGBToTarget(
                ukive::UCMM::Intent::Perceptual,
                ukive::Color(0.25f, 0.89f, 0.47f),
                pf, &tc);

            int sdg = 0;
        }

        return v;
    }

    void ExampleMiscPage::onDestroy() {
        director_.stop();
    }

    void ExampleMiscPage::onDirectorProgress(ukive::AnimationDirector* director) {
        image_view_->requestDraw();
        image_view_->animeParams().setTranslateX(director->getCurValue(0));
        image_view_->animeParams().setScaleX(director->getCurValue(0) / 400);
        image_view_->animeParams().setScaleY(director->getCurValue(0) / 400);
        image_view_->animeParams().setRotate(director->getCurValue(0) / 400 * 360);
        image_view_->animeParams().setRPivotX(image_view_->getWidth() / 2.0);
        image_view_->animeParams().setRPivotY(image_view_->getHeight() / 2.0);
        image_view_->animeParams().setSPivotX(image_view_->getWidth() / 2.0);
        image_view_->animeParams().setSPivotY(image_view_->getHeight() / 2.0);
        image_view_->requestDraw();
    }

    void ExampleMiscPage::onClick(ukive::View* v) {
        if (test_button_ == v) {
            //ukive::SysMessageDialog::show(getWindow(), u"Title", u"Text", 0);
            //test_button_->setEnabled(false);
            if (director_.isRunning()) {
                director_.stop();
            } else {
                director_.start();
                startVSync();
            }
        }
    }

    void ExampleMiscPage::onVSync(
        uint64_t start_time,
        uint32_t display_freq,
        uint32_t real_interval)
    {
        if (!director_.update(start_time, display_freq)) {
            stopVSync();
        }
    }

}
