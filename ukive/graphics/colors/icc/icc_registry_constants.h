// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_ICC_REGISTRY_CONSTANTS_H_
#define UKIVE_GRAPHICS_COLORS_ICC_ICC_REGISTRY_CONSTANTS_H_

#include <cstdint>


namespace ukive {
namespace icc {

    /**
     * Vendor: Adobe Systems, Inc.
     * Date:   2004-01-13
     * Desc:   Adobe CMM
     */
    constexpr uint32_t kCMMAdobe = 0x41444245; // 'ADBE'

    /**
     * Vendor: Agfa Graphics N.V.
     * Date:   2009-03-16
     * Desc:   Agfa CMM
     */
    constexpr uint32_t kCMMAgfa = 0x41434D53; // 'ACMS';

    /**
     * Vendor: Apple Computer
     * Date:   2004-01-23
     * Desc:   Apple CMM
     */
    constexpr uint32_t kCMMApple = 0x6170706C; // 'appl';

    /**
     * Vendor: Canon
     * Date:   2004-01-13
     * Desc:   ColorGear CMM
     */
    constexpr uint32_t kCMMCanon = 0x43434D53; // 'CCMS';

    /**
     * Vendor: Canon
     * Date:   2004-01-13
     * Desc:   ColorGear CMM Lite
     */
    constexpr uint32_t kCMMCanonLite = 0x5543434D; // 'UCCM';

    /**
     * Vendor: Canon
     * Date:   2011-03-09
     * Desc:   ColorGear CMM C
     */
    constexpr uint32_t kCMMCanonC = 0x55434D53; // 'UCMS';

    /**
     * Vendor: EFI
     * Date:   2009-01-07
     * Desc:   EFI CMM
     */
    constexpr uint32_t kCMMEFI = 0x45464920; // 'EFI ';

    /**
     * Vendor: Fujifilm Corporation
     * Date:   2004-02-25
     * Desc:   Fujifilm CMM
     */
    constexpr uint32_t kCMMFujiFilm = 0x46462020; // 'FF  ';

    /**
     * Vendor: ExactCODE GmbH
     * Date:   2013-03-23
     * Desc:   ExactScan CMM
     */
    constexpr uint32_t kCMMExactCODE = 0x45584143; // 'EXAC';

    /**
     * Vendor: Global Graphics Software Inc
     * Date:   2004-02-11
     * Desc:   Harlequin RIP CMM
     */
    constexpr uint32_t kCMMGGS = 0x48434D4D; // 'HCMM';

    /**
     * Vendor: Graeme Gill
     * Date:   2004-11-20
     * Desc:   Argyll CMS CMM
     */
    constexpr uint32_t kCMMGraeme = 0x6172676C; // 'argl';

    /**
     * Vendor: GretagMacbeth
     * Date:   2004-01-20
     * Desc:   LogoSync CMM
     */
    constexpr uint32_t kCMMGretagMacbeth = 0x44676F53; // 'LgoS';

    /**
     * Vendor: Heidelberger Druckmaschinen AG
     * Date:   2004-01-13
     * Desc:   Heidelberg CMM
     */
    constexpr uint32_t kCMMHeidelberger = 0x48444D20; // 'HDM ';

    /**
     * Vendor: Hewlett Packard
     * Date:   2004-01-08
     * Desc:   Little CMS CMM
     */
    constexpr uint32_t kCMMHewlett = 0x6C636d73; // 'lcms';

    /**
     * Vendor: ICC
     * Date:   2018-03-07
     * Desc:   RefIccMAX CMM
     */
    constexpr uint32_t kCMMICCRef = 0x52494D58; // 'RIMX';

    /**
     * Vendor: ICC
     * Date:   2021-02-25
     * Desc:   DemoIccMAX CMM
     */
    constexpr uint32_t kCMMICCDem = 0x44494D58; // 'DIMX';

    /**
     * Vendor: Kodak
     * Date:   2004-01-09
     * Desc:   Kodak CMM
     */
    constexpr uint32_t kCMMKodak = 0x4B434D53; // 'KCMS';

    /**
     * Vendor: Konica Minolta
     * Date:   2003-11-25
     * Desc:   Konica Minolta CMM
     */
    constexpr uint32_t kCMMKonica = 0x4D434D44; // 'MCML';

    /**
     * Vendor: Microsoft
     * Date:   2013-09-05
     * Desc:   Windows Color System CMM
     */
    constexpr uint32_t kCMMMicrosoft = 0x57435320; // 'WCS ';

    /**
     * Vendor: Mutoh
     * Date:   2005-08-31
     * Desc:   Mutoh CMM
     */
    constexpr uint32_t kCMMMutoh = 0x5349474E; // 'SIGN';

    /**
     * Vendor: Onyx Graphics
     * Date:   2018-03-07
     * Desc:   Onyx Graphics CMM
     */
    constexpr uint32_t kCMMOnyx = 0x4F4E5958; // 'ONYX';

    /**
     * Vendor: Rolf Gierling Multitools
     * Date:   2007-09-30
     * Desc:   DeviceLink CMM
     */
    constexpr uint32_t kCMMRolf = 0x52474D53; // 'RGMS';

    /**
     * Vendor: SampleICC
     * Date:   2005-03-16
     * Desc:   SampleICC CMM
     */
    constexpr uint32_t kCMMSampleICC = 0x53494343; // 'SICC';

    /**
     * Vendor: Toshiba TEC Corporation
     * Date:   2013-08-02
     * Desc:   Toshiba CMM
     */
    constexpr uint32_t kCMMToshiba = 0x54434D4D; // 'TCMM';

    /**
     * Vendor: the imaging factory
     * Date:   2004-10-14
     * Desc:   the imaging factory CMM
     */
    constexpr uint32_t kCMMTheImgFac = 0x33324254; // '32BT';

    /**
     * Vendor: Vivo Mobile Communication
     * Date:   2017-10-27
     * Desc:   Vivo CMM
     */
    constexpr uint32_t kCMMVivo = 0x7669766F; // 'vivo';

    /**
     * Vendor: Ware To Go
     * Date:   2009-11-03
     * Desc:   Ware to Go CMM
     */
    constexpr uint32_t kCMMWare = 0x57544720; // 'WTG ';

    /**
     * Vendor: Zoran Corporation
     * Date:   2008-2-23
     * Desc:   Zoran CMM
     */
    constexpr uint32_t kCMMZoran = 0x7A633030; // 'zc00';

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_ICC_REGISTRY_CONSTANTS_H_