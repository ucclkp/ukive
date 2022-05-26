// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shell/gallery/thumbnail_fetcher.h"

#include <algorithm>
#include <filesystem>

#include <objbase.h>

#include "utils/message/message.h"

#include "ukive/app/application.h"
#include "ukive/graphics/images/lc_image_factory.h"
#include "ukive/graphics/images/lc_image.h"
#include "ukive/resources/resource_manager.h"

#include "gallery_utils.h"


namespace shell {

    namespace fs = std::filesystem;

    ThumbnailFetcher::ThumbnailFetcher()
        : shutdown_(false)
    {
        cycler_.setListener(this);
    }

    ThumbnailFetcher::~ThumbnailFetcher() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    void ThumbnailFetcher::launch() {
        shutdown_ = false;
        thread_ = std::thread(&ThumbnailFetcher::onWork, this);
    }

    void ThumbnailFetcher::shutdown() {
        shutdown_ = true;

        {
            std::lock_guard<std::mutex> lg(cv_sync_);
            cv_pred_ = true;
        }
        cv_.notify_one();
    }

    void ThumbnailFetcher::add(
        const std::u16string& path, bool is_file, int id,
        const std::shared_ptr<int>& token)
    {
        Data data{ path, id, is_file, token };
        {
            std::lock_guard<std::mutex> lg(pending_sync_);
            pending_.push_back(std::move(data));
        }

        {
            std::lock_guard<std::mutex> lg(cv_sync_);
            cv_pred_ = true;
        }
        cv_.notify_one();
    }

    void ThumbnailFetcher::remove(int id) {
        std::lock_guard<std::mutex> lg(pending_sync_);
        for (auto it = pending_.begin(); it != pending_.end(); ++it) {
            if (it->id == id) {
                pending_.erase(it);
                return;
            }
        }
    }

    void ThumbnailFetcher::clear() {
        std::lock_guard<std::mutex> lg(pending_sync_);
        pending_.clear();
    }

    void ThumbnailFetcher::setListener(ThumbnailFetchingListener* l) {
        listener_ = l;
    }

    void ThumbnailFetcher::onHandleMessage(const utl::Message& msg) {
        switch (msg.id) {
        case 10089:
            if (listener_) {
                auto info = static_cast<Thumbnail*>(msg.shared_data.get());
                listener_->onThumbnail(*info);
            }
            break;
        default:
            break;
        }
    }

    void ThumbnailFetcher::onWork() {
        ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        for (; !shutdown_;) {
            Data data {};
            bool is_empty;
            {
                std::lock_guard<std::mutex> lg(pending_sync_);
                for (;;) {
                    is_empty = pending_.empty();
                    if (!is_empty) {
                        data = std::move(pending_.front());
                        pending_.pop_front();
                        if (data.w_token.lock()) {
                            break;
                        }
                    } else {
                        break;
                    }
                }
            }

            if (!is_empty) {
                auto thumbnail = std::make_shared<Thumbnail>();
                thumbnail->id = data.id;
                thumbnail->w_token = data.w_token;

                if (data.is_file) {
                    if (!getThumbnail(data.path, thumbnail.get())) {
                    }
                } else {
                    if (!getFolderThumbnail(data.path, thumbnail.get())) {
                    }
                }

                utl::Message* msg = utl::Message::get();
                msg->id = 10089;
                msg->shared_data = thumbnail;
                cycler_.post(msg);
            } else {
                std::unique_lock<std::mutex> lk(cv_sync_);
                cv_.wait(lk, [this] { return cv_pred_; });
                cv_pred_ = false;
            }
        }

        ::CoUninitialize();
    }

    bool ThumbnailFetcher::getFolderThumbnail(const std::u16string& path, Thumbnail* out) {
        struct Info {
            std::u16string path;
            std::u16string name;
        };
        std::error_code ec;
        std::vector<Info> img_files;
        for (auto& f : fs::directory_iterator(path, ec)) {
            if (!f.is_directory(ec) && isImageExtension(f.path().extension().u16string())) {
                img_files.push_back(
                    { f.path().u16string(), f.path().filename().u16string() });
            }
        }
        if (ec || img_files.empty()) {
            auto ph_path = ukive::Application::getResourceManager()->getResRootPath()
                / u"ic_folder.png";

            auto ic = ukive::Application::getImageLocFactory();
            auto img = ic->decodeFile(ph_path.u16string(), ukive::ImageOptions());
            if (!img.isValid()) {
                return false;
            }
            auto frame = img.getFrames()[0];
            img.removeFrame(frame);
            out->ph_bmp = frame;
            return true;
        }

        std::sort(
            img_files.begin(), img_files.end(),
            [](const Info& f1, const Info& f2)->bool
        {
            int ret = compareLingString(f1.name, f2.name);
            return ret == 1;
        });

        return getThumbnail(img_files.front().path, out);
    }

    bool ThumbnailFetcher::getThumbnail(const std::u16string& file_name, Thumbnail* out) {
        auto ic = ukive::Application::getImageLocFactory();
        out->ph_bmp = ic->createThumbnail(file_name, 400, 400, &out->options);
        if (!out->ph_bmp) {
            return false;
        }
        return true;
    }

}
