// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_APP_APPLICATION_H_
#define UKIVE_APP_APPLICATION_H_

#include <memory>


namespace ukive {

    class ColorManager;
    class DisplayManager;
    class GraphicDeviceManager;
    class InputMethodManager;
    class LcImageFactory;
    class ResourceManager;
    class VSyncProvider;

    class Application {
    public:
        struct Options {
            bool is_auto_dpi_scale;
        };

        explicit Application(const Options& options);
        Application(const Options& options, int argc, char16_t* argv[]);
        ~Application();

        void run();

        static void setVSync(bool enable);
        static bool isVSyncEnabled();

        static LcImageFactory* getImageLocFactory();
        static InputMethodManager* getInputMethodManager();
        static ResourceManager* getResourceManager();
        static GraphicDeviceManager* getGraphicDeviceManager();
        static VSyncProvider* getVSyncProvider();
        static DisplayManager* getDisplayManager();
        static ColorManager* getColorManager();

        static int getViewID();
        static const Options& getOptions();

        static float dp2px(int dp);
        static float dp2px(float dp);
        static int dp2pxi(int dp);
        static int dp2pxi(float dp);

    private:
        void initApplication();
        void cleanApplication();

        void initPlatform();
        void cleanPlatform();

        static int view_uid_;
        static bool vsync_enabled_;
        static Application* instance_;

        Options options_;
        std::unique_ptr<LcImageFactory> ilf_;
        std::unique_ptr<InputMethodManager> imm_;
        std::unique_ptr<ResourceManager> res_mgr_;
        std::unique_ptr<GraphicDeviceManager> gdm_;
        std::unique_ptr<VSyncProvider> vsp_;
        std::unique_ptr<ColorManager> cm_;
        std::unique_ptr<DisplayManager> dm_;
    };

}

#endif  // UKIVE_APP_APPLICATION_H_
