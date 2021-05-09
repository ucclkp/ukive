// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "media_view.h"

#include "ukive/window/window.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/images/image_frame.h"


namespace ukive {

    MediaView::MediaView(Context c)
        : MediaView(c, {})
    {}

    MediaView::MediaView(Context c, AttrsRef attrs)
        : View(c, attrs)
    {
        setFocusable(true);
        setMinimumWidth(1);
        setMinimumHeight(1);

        media_player_.reset(MediaPlayer::create());
        media_player_->setCallback(this);
    }

    MediaView::~MediaView() {
        media_player_->close();
    }

    void MediaView::setMediaFile(const std::u16string& file_path) {
        file_path_ = file_path;
    }

    Size MediaView::onDetermineSize(const SizeInfo& info) {
        return getPreferredSize(info, 0, 0);
    }

    void MediaView::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        if (video_frame_) {
            canvas->drawImage(video_frame_);
        }
    }

    bool MediaView::onInputEvent(InputEvent* e) {
        bool result = View::onInputEvent(e);
        return result;
    }

    void MediaView::onBoundsChanged(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        View::onBoundsChanged(new_bounds, old_bounds);

        if (new_bounds.size() == old_bounds.size()) {
            return;
        }

        int content_width = new_bounds.width() - getPadding().hori();
        int content_height = new_bounds.height() - getPadding().vert();

        media_player_->setDisplaySize(Size(content_width, content_height));
    }

    void MediaView::onContextChanged(const Context& context) {
        View::onContextChanged(context);

        switch (context.getChanged()) {
        case Context::DEV_LOST:
        {
            // 通知场景，要把设备资源释放掉
            if (video_frame_) {
                delete video_frame_;
                video_frame_ = nullptr;
            }
            break;
        }

        case Context::DEV_RESTORE:
        {
            auto bounds(getContentBounds());

            // 重新创建设备资源
            break;
        }

        default:
            break;
        }
    }

    void MediaView::onAttachedToWindow(Window* w) {
        media_player_->openFile(uR"(D:\test.mp4)", w);
    }

    void MediaView::onDetachFromWindow() {
        media_player_->close();
    }

    void MediaView::onMediaOpenComplete(bool succeeded) {
        if (succeeded) {
            media_player_->start(true, 0);
        }
    }

    void MediaView::onMediaStarted() {

    }

    void MediaView::onMediaPaused() {

    }

    void MediaView::onMediaStopped() {

    }

    void MediaView::onMediaEnded() {

    }

    void MediaView::onMediaClosed() {

    }

    void MediaView::onRenderVideoFrame(ImageFrame* frame) {
        // TODO:
        if (video_frame_) {
            delete video_frame_;
        }
        video_frame_ = frame;
        requestDraw();
    }

}