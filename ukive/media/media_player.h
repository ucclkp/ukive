// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_MEDIA_PLAYER_H_
#define UKIVE_MEDIA_MEDIA_PLAYER_H_

#include <memory>
#include <string>

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/size.hpp"


namespace ukive {

    class Window;
    class ImageFrame;

    class MediaPlayerCallback {
    public:
        virtual ~MediaPlayerCallback() = default;

        virtual void onMediaOpenComplete(bool succeeded) = 0;
        virtual void onMediaStarted() = 0;
        virtual void onMediaPaused() = 0;
        virtual void onMediaStopped() = 0;
        virtual void onMediaEnded() = 0;
        virtual void onMediaClosed() = 0;

        virtual void onRenderVideoFrame(const GPtr<ImageFrame>& frame) = 0;
    };

    class MediaPlayer {
    public:
        static MediaPlayer* create();

        virtual ~MediaPlayer() = default;

        virtual void setCallback(MediaPlayerCallback* cb) = 0;
        virtual void setDisplaySize(const Size& size) = 0;

        virtual bool openUrl(const std::u16string_view& url, Window* w) = 0;
        virtual bool openFile(const std::u16string_view& file_name, Window* w) = 0;

        virtual bool start(bool current, int64_t position) = 0;
        virtual bool pause() = 0;
        virtual bool stop() = 0;
        virtual void close() = 0;
    };

}

#endif  // UKIVE_MEDIA_MEDIA_PLAYER_H_