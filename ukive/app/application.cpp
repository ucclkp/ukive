// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/app/application.h"

#include "utils/log.h"
#include "utils/command_line.h"
#include "utils/message/message.h"
#include "utils/message/message_pump.h"

#include "ukive/graphics/colors/color_manager.h"
#include "ukive/graphics/display.h"
#include "ukive/graphics/display_manager.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/images/lc_image_factory.h"
#include "ukive/graphics/vsync_provider.h"
#include "ukive/text/input_method_manager.h"
#include "ukive/resources/layout_parser.h"
#include "ukive/resources/resource_manager.h"


namespace ukive {

    long long Application::view_uid_ = 100000000ll;
    bool Application::vsync_enabled_ = true;
    Application* Application::instance_ = nullptr;


    Application::Application(const Options& options)
        : options_(options)
    {
        instance_ = this;
        utl::CommandLine::initialize();
        initApplication();
    }

    Application::Application(const Options& options, int argc, char16_t* argv[])
        : options_(options)
    {
        instance_ = this;
        utl::CommandLine::initialize(argc, argv);
        initApplication();
    }

    Application::~Application() {
        cleanApplication();
        instance_ = nullptr;
    }

    void Application::initApplication() {
        initPlatform();

        utl::MessagePump::createForUI();

        gdm_.reset(GraphicDeviceManager::create());
        if (!gdm_->initialize()) {
            LOG(Log::FATAL) << "Failed to initialize GraphicDeviceManager";
            return;
        }

        dm_.reset(DisplayManager::create());
        if (!dm_->initialize()) {
            LOG(Log::FATAL) << "Failed to initialize DisplayManager";
            return;
        }

        cm_.reset(ColorManager::create());

        LayoutParser::initialize();

        ilf_.reset(LcImageFactory::create());
        bool ret = ilf_->initialize();
        if (!ret) {
            LOG(Log::ERR) << "Failed to initialize LcImageFactory";
        }

        imm_.reset(InputMethodManager::create());
        ret = imm_->initialize();
        if (!ret) {
            LOG(Log::ERR) << "Failed to initialize InputMethodManager";
        }

        res_mgr_.reset(new ResourceManager());
        ret = res_mgr_->initialize(options_.app_name);
        if (!ret) {
            LOG(Log::ERR) << "Failed to initialize ResourceManager";
        }

        vsp_.reset(VSyncProvider::create());
    }

    void Application::cleanApplication() {
        vsp_.reset();

        utl::MessagePump::destroy();

        res_mgr_->destroy();

        ilf_->destroy();
        ilf_.reset();

        imm_->destroy();
        imm_.reset();

        cm_.reset();

        dm_->destroy();
        dm_.reset();

        gdm_->destroy();
        gdm_.reset();

        cleanPlatform();
    }

    void Application::run() {
        utl::MessagePump::run();
    }

    // static
    void Application::setVSync(bool enable){
        vsync_enabled_ = enable;
    }

    // static
    bool Application::isVSyncEnabled(){
        return vsync_enabled_;
    }

    // static
    LcImageFactory* Application::getImageLocFactory() {
        return instance_->ilf_.get();
    }

    // static
    InputMethodManager* Application::getInputMethodManager() {
        return instance_->imm_.get();
    }

    // static
    ResourceManager* Application::getResourceManager() {
        return instance_->res_mgr_.get();
    }

    // static
    GraphicDeviceManager* Application::getGraphicDeviceManager() {
        return instance_->gdm_.get();
    }

    // static
    VSyncProvider* Application::getVSyncProvider() {
        return instance_->vsp_.get();
    }

    // static
    DisplayManager* Application::getDisplayManager() {
        return instance_->dm_.get();
    }

    // static
    ColorManager* Application::getColorManager() {
        return instance_->cm_.get();
    }

    // static
    long long Application::getNextViewID() {
        ++view_uid_;
        return view_uid_;
    }

    // static
    const Application::Options& Application::getOptions() {
        return instance_->options_;
    }

    // static
    float Application::dp2px(int dp) {
        if (instance_->options_.is_auto_dpi_scale) {
            return float(dp);
        }

        float sx, sy;
        instance_->dm_->fromPrimary()->getUserScale(&sx, &sy);
        return sx * dp;
    }

    // static
    float Application::dp2px(float dp) {
        if (instance_->options_.is_auto_dpi_scale) {
            return dp;
        }

        float sx, sy;
        instance_->dm_->fromPrimary()->getUserScale(&sx, &sy);
        return sy * dp;
    }

    // static
    int Application::dp2pxi(int dp) {
        if (instance_->options_.is_auto_dpi_scale) {
            return dp;
        }

        float sx, sy;
        instance_->dm_->fromPrimary()->getUserScale(&sx, &sy);
        return static_cast<int>(sx * dp);
    }

    // static
    int Application::dp2pxi(float dp) {
        if (instance_->options_.is_auto_dpi_scale) {
            return int(dp);
        }

        float sx, sy;
        instance_->dm_->fromPrimary()->getUserScale(&sx, &sy);
        return static_cast<int>(sy * dp);
    }

    // static
    int Application::sp2px(int sp) {
        int mid;
        if (instance_->options_.is_auto_dpi_scale) {
            mid = sp;
        } else {
            float sx, sy;
            instance_->dm_->fromPrimary()->getUserScale(&sx, &sy);
            mid = static_cast<int>(sx * sp);
        }
        mid += mid % 2;
        return mid;
    }

}
