// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "media_player.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/media/win/media_player_win.h"
#endif


namespace ukive {

    // static
    MediaPlayer* MediaPlayer::create() {
#ifdef OS_WINDOWS
        return new MediaPlayerWin();
#endif
    }

}
