// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "mf_sample_recycler.h"


namespace ukive {
namespace win {

    MFSampleRecycler::MFSampleRecycler() {}

    MFSampleRecycler::~MFSampleRecycler() {}

    void MFSampleRecycler::recycleSample(const ComPtr<IMFSample>& sample) {
        CritSecGuard guard(crit_sec_);

        samples_.push_back(sample);
        --pending_;
    }

    void MFSampleRecycler::recycleSamples(SampleList& samples) {
        CritSecGuard guard(crit_sec_);

        samples_ = samples;
        samples.clear();
    }

    ComPtr<IMFSample> MFSampleRecycler::reuseSample() {
        CritSecGuard guard(crit_sec_);

        if (samples_.empty()) {
            return {};
        }

        auto sample = samples_.front();
        samples_.pop_front();

        ++pending_;

        return sample;
    }

    void MFSampleRecycler::clear() {
        CritSecGuard guard(crit_sec_);

        samples_.clear();
        pending_ = 0;
    }

    bool MFSampleRecycler::isPending() {
        CritSecGuard guard(crit_sec_);
        return pending_ > 0;
    }

}
}