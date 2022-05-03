// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_MEDIA_VIEW_H_
#define UKIVE_VIEWS_MEDIA_VIEW_H_

#include "ukive/media/media_player.h"
#include "ukive/views/view.h"


namespace ukive {

    class GPUTexture;
    class GPURenderTarget;
    class ImageFrame;

    class MediaView : public View, public MediaPlayerCallback {
    public:
        explicit MediaView(Context c);
        MediaView(Context c, AttrsRef attrs);
        ~MediaView();

        void setMediaFile(const std::u16string_view& file_path);

    protected:
        Size onDetermineSize(const SizeInfo& info) override;
        void onBoundsChanged(const Rect& new_bounds, const Rect& old_bounds) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

        void onContextChanged(Context::Type type, const Context& context) override;
        void onAttachedToWindow(Window* w) override;
        void onDetachFromWindow() override;

        // MediaPlayerCallback
        void onMediaOpenComplete(bool succeeded) override;
        void onMediaStarted() override;
        void onMediaPaused() override;
        void onMediaStopped() override;
        void onMediaEnded() override;
        void onMediaClosed() override;
        void onRenderVideoFrame(const GPtr<ImageFrame>& frame) override;

    private:
        std::u16string file_path_;
        std::unique_ptr<MediaPlayer> media_player_;
        GPtr<ImageFrame> video_frame_;
    };

}

#endif  // UKIVE_VIEWS_MEDIA_VIEW_H_