// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shell/gallery/picture/picture_window.h"

#include "utils/convert.h"
#include "utils/log.h"
#include "utils/files/file_utils.h"

#include "ukive/app/application.h"
#include "ukive/views/button.h"
#include "ukive/views/image_view.h"
#include "ukive/views/layout/sequence_layout.h"
#include "ukive/views/layout_info/layout_info.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/system/win/reg_manager.h"
#include "ukive/elements/color_element.h"
#include "ukive/views/check_box.h"
#include "ukive/graphics/canvas.h"

#include "shell/gallery/picture/slide_image_view.h"


namespace {

    const wchar_t kProgID[] = L"XPicture.Viewer.1";

}

namespace shell {

    void PictureWindow::onCreated() {
        Window::onCreated();

        showTitleBar();

        allowed_exts_.push_back(u".png");
        allowed_exts_.push_back(u".jpg");
        allowed_exts_.push_back(u".jpeg");
        allowed_exts_.push_back(u".bmp");
        allowed_exts_.push_back(u".gif");

        using Rlp = ukive::RestraintLayoutInfo;
        auto layout = new ukive::RestraintLayout(getContext());

        asso_btn_ = new ukive::Button(getContext());
        asso_btn_->setText(u"Associate");
        asso_btn_->setOnClickListener(this);
        asso_btn_->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);
        asso_btn_->setLayoutMargin(getContext().dp2pxi(8), getContext().dp2pxi(8), 0, 0);
        asso_btn_->setExtraLayoutInfo(
            Rlp::Builder()
            .start(layout->getId(), Rlp::START)
            .top(layout->getId(), Rlp::TOP).build());

        unasso_btn_ = new ukive::Button(getContext());
        unasso_btn_->setText(u"Unassociate");
        unasso_btn_->setOnClickListener(this);
        unasso_btn_->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);
        unasso_btn_->setLayoutMargin(getContext().dp2pxi(8), getContext().dp2pxi(8), 0, 0);
        unasso_btn_->setExtraLayoutInfo(
            Rlp::Builder()
            .start(asso_btn_->getId(), Rlp::END)
            .top(layout->getId(), Rlp::TOP).build());

        gallery_view_ = new SlideImageView(getContext());
        gallery_view_->setImageFileExts(allowed_exts_);
        gallery_view_->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_FILL);
        gallery_view_->setLayoutMargin(
            getContext().dp2pxi(8), getContext().dp2pxi(8), getContext().dp2pxi(8), getContext().dp2pxi(8));
        gallery_view_->setExtraLayoutInfo(
            Rlp::Builder()
            .start(layout->getId(), Rlp::START)
            .end(layout->getId(), Rlp::END)
            .top(asso_btn_->getId(), Rlp::BOTTOM)
            .bottom(layout->getId(), Rlp::BOTTOM).build());

        layout->addView(asso_btn_);
        layout->addView(unasso_btn_);
        layout->addView(gallery_view_);

        setContentView(layout);

        initData();
    }

    void PictureWindow::initData() {
        auto& params = getPurpose().params;
        auto it = params.find("target");
        if (it != params.end()) {
            gallery_view_->setFirstImageFile(it->second);
        }

        if (checkReg()) {
            asso_btn_->setEnabled(false);
        } else {
            unasso_btn_->setEnabled(false);
        }
    }

    bool PictureWindow::checkReg() {
        if (!ukive::RegManager::hasProgId(kProgID)) {
            return false;
        }

        for (const auto& ext : allowed_exts_) {
            auto dot_ext = utl::toASCIILower(ext);
            if (!ukive::RegManager::hasProgIdInExt(std::wstring(dot_ext.begin(), dot_ext.end()), kProgID)) {
                return false;
            }
        }
        return true;
    }

    bool PictureWindow::associateExts() {
        std::u16string app_path;
        app_path.append(u"\"");
        app_path.append(utl::getExecFileName(false).u16string());
        app_path.append(u"\" \"%1\"");

        if (!ukive::RegManager::registerProgId(kProgID, std::wstring(app_path.begin(), app_path.end()))) {
            return false;
        }

        for (const auto& ext : allowed_exts_) {
            auto dot_ext = utl::toASCIILower(ext);
            if (!ukive::RegManager::hasProgIdInExt(std::wstring(dot_ext.begin(), dot_ext.end()), kProgID)) {
                if (!ukive::RegManager::associateExtName(std::wstring(dot_ext.begin(), dot_ext.end()), kProgID)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool PictureWindow::unassociateExts() {
        for (const auto& ext : allowed_exts_) {
            auto dot_ext = utl::toASCIILower(ext);
            if (ukive::RegManager::hasProgIdInExt(std::wstring(dot_ext.begin(), dot_ext.end()), kProgID)) {
                if (!ukive::RegManager::unassociateExtName(std::wstring(dot_ext.begin(), dot_ext.end()), kProgID)) {
                    return false;
                }
            }
        }

        if (!ukive::RegManager::unregisterProgId(kProgID)) {
            return false;
        }

        return true;
    }

    void PictureWindow::onClick(ukive::View* v) {
        if (v == asso_btn_) {
            if (associateExts()) {
                asso_btn_->setEnabled(false);
                unasso_btn_->setEnabled(true);
            }
        } else if (v == unasso_btn_) {
            if (unassociateExts()) {
                asso_btn_->setEnabled(true);
                unasso_btn_->setEnabled(false);
            }
        }
    }

}
