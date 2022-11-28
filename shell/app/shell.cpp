// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <memory>

#include "utils/log.h"
#include "utils/platform_utils.h"

#include "ukive/app/application.h"
#include "ukive/window/purpose.h"

#include "shell/lod/lod_window.h"
#include "shell/examples/example_window.h"
#include "shell/text/text_window.h"
//#include "shell/effects/effect_window.h"
#include "shell/grid/grid_window.h"
#include "shell/visualize/visualization_window.h"


GUI_MAIN() {
    utl::Log::Params log_params;
    log_params.file_name = u"Debug.log";
    log_params.short_file_name = true;
    log_params.target = utl::Log::OutputTarget::DEBUGGER | utl::Log::OutputTarget::FILE;
    utl::InitLogging(log_params);

    LOG(Log::INFO) << "========== Application start.";

    ukive::Application::Options options;
    options.is_auto_dpi_scale = false;
    options.app_name = u"shell";
    auto app = std::make_shared<ukive::Application>(options);

    ukive::Window::InitParams native_params;
    native_params.frame_type = ukive::WINDOW_FRAME_NATIVE;

    /*auto text_window = std::make_shared<shell::TextWindow>();
    text_window->init(native_params);
    text_window->setTitle(u"Text");
    text_window->setWidth(ukive::Application::dp2pxi(400));
    text_window->setHeight(ukive::Application::dp2pxi(400));
    text_window->center();
    text_window->show();*/

    /*auto lod_window = std::make_shared<shell::LodWindow>();
    lod_window->init(ukive::Window::InitParams());
    lod_window->setTitle(u"Lod");
    lod_window->setWidth(ukive::Application::dp2pxi(600));
    lod_window->setHeight(ukive::Application::dp2pxi(500));
    lod_window->center();
    lod_window->show();*/

    /*auto shadow_window = std::make_shared<shell::EffectWindow>();
    shadow_window->init(native_params);
    shadow_window->setTitle(u"Effects");
    shadow_window->setWidth(ukive::Application::dp2pxi(800));
    shadow_window->setHeight(ukive::Application::dp2pxi(800));
    shadow_window->center();
    shadow_window->show();*/

    auto example_window = std::make_shared<shell::ExampleWindow>();
    example_window->init(ukive::Window::InitParams());
    example_window->setTitle(u"Examples");
    example_window->setWidth(ukive::Application::dp2pxi(600));
    example_window->setHeight(ukive::Application::dp2pxi(600));
    example_window->setTranslucentType(ukive::TRANS_BLURBEHIND_SYSTEM);
    example_window->center();
    example_window->show();
    //example_window->maximize();

    /*auto grid_window = std::make_shared<shell::GridWindow>();
    grid_window->init(ukive::Window::InitParams());
    grid_window->setTitle(u"Graphic Grid");
    grid_window->setWidth(ukive::Application::dp2pxi(600));
    grid_window->setHeight(ukive::Application::dp2pxi(600));
    grid_window->setTranslucentType(ukive::TRANS_BLURBEHIND_SYSTEM);
    grid_window->center();
    grid_window->show();*/

    /*auto vsul_window = std::make_shared<vsul::VisualizationWindow>();
    vsul_window->init(ukive::Window::InitParams());
    vsul_window->setTitle(u"Visualized Layout");
    vsul_window->setWidth(ukive::Application::dp2pxi(600));
    vsul_window->setHeight(ukive::Application::dp2pxi(600));
    vsul_window->setTranslucentType(ukive::TRANS_BLURBEHIND_SYSTEM);
    vsul_window->center();
    vsul_window->show();*/

    app->run();

    LOG(Log::INFO) << "Application exit.\n";

    utl::UninitLogging();

    return 0;
}
