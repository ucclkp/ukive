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

    void MFSampleRecycler::recycleSample(const utl::win::ComPtr<IMFSample>& sample) {
        utl::win::CritSecGuard guard(crit_sec_);

        samples_.push_back(sample);
        --pending_;
    }

    void MFSampleRecycler::recycleSamples(SampleList& samples) {
        utl::win::CritSecGuard guard(crit_sec_);

        samples_ = samples;
        samples.clear();
    }

    utl::win::ComPtr<IMFSample> MFSampleRecycler::reuseSample() {
        utl::win::CritSecGuard guard(crit_sec_);

        if (samples_.empty()) {
            return {};
        }

        auto sample = samples_.front();
        samples_.pop_front();

        ++pending_;

        return sample;
    }

    void MFSampleRecycler::clear() {
        utl::win::CritSecGuard guard(crit_sec_);

        samples_.clear();
        pending_ = 0;
    }

    bool MFSampleRecycler::isPending() {
        utl::win::CritSecGuard guard(crit_sec_);
        return pending_ > 0;
    }

}
}