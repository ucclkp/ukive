// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_WIN_MEDIA_PLAYER_MAC_H_
#define UKIVE_MEDIA_WIN_MEDIA_PLAYER_MAC_H_

#include <string>

#include "utils/message/cycler.h"

#include "ukive/media/media_player.h"


namespace ukive {

    class Window;
    class ImageFrame;

namespace mac {

    class MediaPlayerMac :
        public MediaPlayer,
        public utl::CyclerListener
    {
    public:
        MediaPlayerMac();
        ~MediaPlayerMac();

        void setCallback(MediaPlayerCallback* cb) override;
        void setDisplaySize(const Size& size) override;

        bool openUrl(const std::u16string_view& url, Window* w) override;
        bool openFile(const std::u16string_view& file_name, Window* w) override;

        bool start(bool current, int64_t position) override;
        bool pause() override;
        bool stop() override;
        void close() override;

    private:
        enum class State {
            Idle,
            Opened,
            Started,
            Paused,
            Stopped,
            Ended,
            Closed,
        };

        static bool startup();
        static void shutdown();
        static unsigned int startup_counter_;

        void destroy();

        // utl::CyclerListener
        void onHandleMessage(const utl::Message& msg) override;

        Window* window_ = nullptr;
        MediaPlayerCallback* callback_ = nullptr;

        utl::Cycler cycler_;
        State state_ = State::Idle;
    };

}
}

#endif  // UKIVE_MEDIA_WIN_MEDIA_PLAYER_MAC_H_
