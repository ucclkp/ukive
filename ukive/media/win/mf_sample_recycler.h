// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_WIN_MF_SAMPLE_RECYCLER_H_
#define UKIVE_MEDIA_WIN_MF_SAMPLE_RECYCLER_H_

#include <list>

#include "ukive/media/win/mf_common.h"
#include "ukive/system/win/com_ptr.hpp"
#include "ukive/system/win/critical_section.h"

#include <mfidl.h>


namespace ukive {

    class MFSampleRecycler {
    public:
        MFSampleRecycler();
        ~MFSampleRecycler();

        void recycleSample(const ComPtr<IMFSample>& sample);
        void recycleSamples(SampleList& samples);
        ComPtr<IMFSample> reuseSample();
        void clear();

        bool isPending();

    private:
        win::CritSec crit_sec_;
        SampleList samples_;
        uint32_t pending_ = 0;
    };

}

#endif  // UKIVE_MEDIA_WIN_MF_SAMPLE_RECYCLER_H_