// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_WIN_MF_COMMON_H_
#define UKIVE_MEDIA_WIN_MF_COMMON_H_

#include <list>

#include "utils/memory/win/com_ptr.hpp"

#include <guiddef.h>
#include <mfidl.h>


using SampleList = std::list<utl::win::ComPtr<IMFSample>>;

// {0315FF06-7CC7-4930-A0F0-D3678BDA4192}
static const GUID MFSampleSwapChain =
{ 0x315ff06, 0x7cc7, 0x4930, { 0xa0, 0xf0, 0xd3, 0x67, 0x8b, 0xda, 0x41, 0x92 } };


// {147E334E-7B5B-472E-90C1-157A7B5EDEDE}
const GUID MFSampleCounter =
{ 0x147e334e, 0x7b5b, 0x472e, { 0x90, 0xc1, 0x15, 0x7a, 0x7b, 0x5e, 0xde, 0xde } };

#endif  // UKIVE_MEDIA_WIN_MF_COMMON_H_