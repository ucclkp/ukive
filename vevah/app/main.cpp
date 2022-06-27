// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <memory>

#include "utils/log.h"
#include "utils/platform_utils.h"

#include "ukive/app/application.h"
#include "ukive/resources/layout_parser.h"

#include "vevah/main_window.h"
#include "vevah/container_layout.h"


GUI_MAIN() {
    utl::Log::Params log_params;
    log_params.file_name = u"Debug.log";
    log_params.short_file_name = true;
    log_params.target = utl::Log::OutputTarget::DEBUGGER | utl::Log::OutputTarget::FILE;
    utl::InitLogging(log_params);

    LOG(Log::INFO) << "========== Application start. ==========";

    ukive::Application::Options options;
    options.is_auto_dpi_scale = false;
    options.app_name = u"vevah";
    auto app = std::make_shared<ukive::Application>(options);

    UKIVE_ADD_VIEW_NAME(vevah::ContainerLayout);

    ukive::Window::InitParams native_params;
    native_params.frame_type = ukive::WINDOW_FRAME_NATIVE;

    auto main_window = std::make_shared<vevah::MainWindow>();
    main_window->init(ukive::Window::InitParams());
    main_window->setTitle(u"Examples");
    main_window->setWidth(ukive::Application::dp2pxi(1000));
    main_window->setHeight(ukive::Application::dp2pxi(700));
    main_window->setTranslucentType(ukive::TRANS_BLURBEHIND_SYSTEM);
    main_window->center();
    main_window->show();

    app->run();

    LOG(Log::INFO) << "========== Application exit. ==========\n";

    utl::UninitLogging();

    return 0;
}
