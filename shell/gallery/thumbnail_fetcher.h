// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef GALLERY_THUMBNAIL_FETCHER_H_
#define GALLERY_THUMBNAIL_FETCHER_H_

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>

#include "utils/message/cycler.h"

#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/images/lc_image_frame.h"


namespace shell {

    struct Thumbnail {
        int id;
        std::string data;
        int width;
        int height;
        ukive::ImageOptions options;
        std::shared_ptr<ukive::LcImageFrame> ph_bmp;
        std::weak_ptr<int> w_token;
    };


    class ThumbnailFetchingListener {
    public:
        virtual ~ThumbnailFetchingListener() = default;

        virtual void onThumbnail(const Thumbnail& thumb) = 0;
    };


    class ThumbnailFetcher : public utl::CyclerListener {
    public:
        ThumbnailFetcher();
        ~ThumbnailFetcher();

        void launch();
        void shutdown();

        void add(
            const std::u16string& path, bool is_file, int id,
            const std::shared_ptr<int>& token);
        void remove(int id);
        void clear();

        void setListener(ThumbnailFetchingListener* l);

    protected:
        // ukive::CyclerListener
        void onHandleMessage(const utl::Message& msg) override;

    private:
        struct Data {
            std::u16string path;
            int id;
            bool is_file;
            std::weak_ptr<int> w_token;
        };

        void onWork();
        bool getFolderThumbnail(const std::u16string& path, Thumbnail* out);
        bool getThumbnail(const std::u16string& file_name, Thumbnail* out);

        std::mutex pending_sync_;
        std::thread thread_;
        utl::Cycler cycler_;
        std::atomic_bool shutdown_;
        std::list<Data> pending_;

        std::mutex cv_sync_;
        std::condition_variable cv_;
        bool cv_pred_ = false;

        ThumbnailFetchingListener* listener_ = nullptr;
    };

}

#endif  // GALLERY_THUMBNAIL_FETCHER_H_