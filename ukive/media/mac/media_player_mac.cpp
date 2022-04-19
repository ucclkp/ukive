// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "media_player_mac.h"

#include "utils/log.h"
#include "utils/message/message.h"
#include "utils/scope_utils.hpp"

#include "ukive/app/application.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/window/window.h"

#define MSG_CREATE_SURFACE   1
#define MSG_DESTROY_SURFACE  2
#define MSG_RENDER_SURFACE   3
#define MSG_OPEN_SUCCEEDED   4
#define MSG_OPEN_FAILED      5
#define MSG_MEDIA_STARTED    6
#define MSG_MEDIA_PAUSED     7
#define MSG_MEDIA_STOPPED    8
#define MSG_MEDIA_ENDED      9
#define MSG_MEDIA_CLOSED     10


namespace ukive {
namespace mac {

    // static
    unsigned int MediaPlayerMac::startup_counter_ = 0;

    // static
    bool MediaPlayerMac::startup() {
        if (startup_counter_ == 0) {
        }
        ++startup_counter_;
        return true;
    }

    // static
    void MediaPlayerMac::shutdown() {
        if (startup_counter_ > 0) {
            --startup_counter_;
            if (startup_counter_ == 0) {
            }
        }
    }

    MediaPlayerMac::MediaPlayerMac() {
        cycler_.setListener(this);
    }

    MediaPlayerMac::~MediaPlayerMac() {
        destroy();
    }

    void MediaPlayerMac::setCallback(MediaPlayerCallback* cb) {
        callback_ = cb;
    }

    void MediaPlayerMac::setDisplaySize(const Size& size) {
    }

    bool MediaPlayerMac::openUrl(const std::u16string_view& url, Window* w) {
        if (state_ != State::Idle) {
            return false;
        }

        window_ = w;

        if (!startup()) {
            return false;
        }

        return true;
    }

    bool MediaPlayerMac::openFile(const std::u16string_view& file_name, Window* w) {
        if (state_ != State::Idle) {
            return false;
        }

        window_ = w;

        if (!startup()) {
            return false;
        }

        return true;
    }

    bool MediaPlayerMac::start(bool current, int64_t position) {
        return false;
    }

    bool MediaPlayerMac::pause() {
        return false;
    }

    bool MediaPlayerMac::stop() {
        return false;
    }

    void MediaPlayerMac::close() {
        destroy();
    }

    void MediaPlayerMac::destroy() {
        shutdown();
        state_ = State::Idle;
    }

    void MediaPlayerMac::onHandleMessage(const utl::Message& msg) {
        switch (msg.id) {
        case MSG_OPEN_SUCCEEDED:
            state_ = State::Opened;
            if (callback_) {
                callback_->onMediaOpenComplete(true);
            }
            break;
        case MSG_OPEN_FAILED:
            if (callback_) {
                callback_->onMediaOpenComplete(false);
            }
            break;
        case MSG_MEDIA_STARTED:
            state_ = State::Started;
            if (callback_) {
                callback_->onMediaStarted();
            }
            break;
        case MSG_MEDIA_PAUSED:
            state_ = State::Paused;
            if (callback_) {
                callback_->onMediaPaused();
            }
            break;
        case MSG_MEDIA_STOPPED:
            state_ = State::Stopped;
            if (callback_) {
                callback_->onMediaStopped();
            }
            break;
        case MSG_MEDIA_ENDED:
            state_ = State::Ended;
            if (callback_) {
                callback_->onMediaEnded();
            }
            break;
        case MSG_MEDIA_CLOSED:
            state_ = State::Closed;
            if (callback_) {
                callback_->onMediaClosed();
            }
            break;
        case MSG_RENDER_SURFACE:
            if (callback_) {
                callback_->onRenderVideoFrame(
                    std::static_pointer_cast<MessageData>(msg.shared_data)->frame);
            } else {
                delete static_cast<ImageFrame*>(msg.data);
            }
            break;
        default:
            break;
        }
    }

}
}
