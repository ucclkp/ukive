// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_LOD_LOD_WINDOW_H_
#define SHELL_LOD_LOD_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/seek_value_changed_listener.h"


namespace ukive {
    class TextView;
    class SeekBar;
    class RestraintLayout;
    class Space3DView;
}

namespace shell {

    class TerrainScene;

    class LodWindow :
        public ukive::Window,
        public ukive::OnSeekValueChangedListener,
        public ukive::OnClickListener
    {
    public:
        LodWindow();
        ~LodWindow();

        void onCreated() override;

        void onClick(ukive::View* v) override;

        void onSeekValueChanged(ukive::SeekBar* seekBar, float value) override;
        void onSeekIntegerValueChanged(ukive::SeekBar* seekBar, int value) override;

        void onRender();

    private:
        void inflateCtlLayout(ukive::RestraintLayout* rightLayout);

        int frame_counter_ = 0;
        int frame_pre_second_ = 0;
        uint64_t prev_time_ = 0;

        ukive::SeekBar* c1_seekbar_ = nullptr;
        ukive::SeekBar* c2_seekbar_ = nullptr;
        ukive::SeekBar* split_seekbar_ = nullptr;
        ukive::TextView* c1_value_tv_ = nullptr;
        ukive::TextView* c2_value_tv_ = nullptr;
        ukive::TextView* split_value_tv_ = nullptr;
        ukive::TextView* render_info_ = nullptr;
        ukive::Space3DView* lod_view_ = nullptr;

        TerrainScene* terrain_scene_ = nullptr;
    };

}

#endif  // SHELL_LOD_LOD_WINDOW_H_