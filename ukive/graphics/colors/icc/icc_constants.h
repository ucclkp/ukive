// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_ICC_CONSTANTS_H_
#define UKIVE_GRAPHICS_COLORS_ICC_ICC_CONSTANTS_H_

#include <cstdint>


namespace ukive {
namespace icc {

    constexpr uint32_t kFileSignature = 'acsp';

    constexpr uint32_t kClassInputDevice   = 'scnr';
    constexpr uint32_t kClassDisplayDevice = 'mntr';
    constexpr uint32_t kClassOutputDevice  = 'prtr';
    constexpr uint32_t kClassDeviceLink    = 'link';
    constexpr uint32_t kClassColorSpace    = 'spac';
    constexpr uint32_t kClassAbstract      = 'abst';
    constexpr uint32_t kClassNamedColor    = 'nmcl';

    constexpr uint32_t kColorSpaceXYZ   = 'XYZ ';
    constexpr uint32_t kColorSpaceLAB   = 'Lab ';
    constexpr uint32_t kColorSpaceLUV   = 'Luv ';
    constexpr uint32_t kColorSpaceYCbCr = 'YCbr';
    constexpr uint32_t kColorSpaceYxy   = 'Yxy ';
    constexpr uint32_t kColorSpaceRGB   = 'RGB ';
    constexpr uint32_t kColorSpaceGray  = 'GRAY';
    constexpr uint32_t kColorSpaceHSV   = 'HSV ';
    constexpr uint32_t kColorSpaceHLS   = 'HLS ';
    constexpr uint32_t kColorSpaceCMYK  = 'CMYK';
    constexpr uint32_t kColorSpaceCMY   = 'CMY ';
    constexpr uint32_t kColorSpace2Clr  = '2CLR';
    constexpr uint32_t kColorSpace3Clr  = '3CLR';
    constexpr uint32_t kColorSpace4Clr  = '4CLR';
    constexpr uint32_t kColorSpace5Clr  = '5CLR';
    constexpr uint32_t kColorSpace6Clr  = '6CLR';
    constexpr uint32_t kColorSpace7Clr  = '7CLR';
    constexpr uint32_t kColorSpace8Clr  = '8CLR';
    constexpr uint32_t kColorSpace9Clr  = '9CLR';
    constexpr uint32_t kColorSpace10Clr = 'ACLR';
    constexpr uint32_t kColorSpace11Clr = 'BCLR';
    constexpr uint32_t kColorSpace12Clr = 'CCLR';
    constexpr uint32_t kColorSpace13Clr = 'DCLR';
    constexpr uint32_t kColorSpace14Clr = 'ECLR';
    constexpr uint32_t kColorSpace15Clr = 'FCLR';

    constexpr uint32_t kPrimaryPlatformApple = 'APPL';
    constexpr uint32_t kPrimaryPlatformMS    = 'MSFT';
    constexpr uint32_t kPrimaryPlatformSGI   = 'SGI ';
    constexpr uint32_t kPrimaryPlatformSMI   = 'SUNW';

    constexpr uint32_t kTagAToB0      = 'A2B0';
    constexpr uint32_t kTagAToB1      = 'A2B1';
    constexpr uint32_t kTagAToB2      = 'A2B2';
    constexpr uint32_t kTagBlueMatCol = 'bXYZ';
    constexpr uint32_t kTagBlueTRC    = 'bTRC';
    constexpr uint32_t kTagBToA0      = 'B2A0';
    constexpr uint32_t kTagBToA1      = 'B2A1';
    constexpr uint32_t kTagBToA2      = 'B2A2';
    constexpr uint32_t kTagBToD0      = 'B2D0';
    constexpr uint32_t kTagBToD1      = 'B2D1';
    constexpr uint32_t kTagBToD2      = 'B2D2';
    constexpr uint32_t kTagBToD3      = 'B2D3';
    constexpr uint32_t kTagCalibDateTime  = 'calt';
    constexpr uint32_t kTagCharTarget     = 'targ';
    constexpr uint32_t kTagChromAdapt     = 'chad';
    constexpr uint32_t kTagChromaticity   = 'chrm';
    constexpr uint32_t kTagColorOrder     = 'clro';
    constexpr uint32_t kTagColorTable     = 'clrt';
    constexpr uint32_t kTagColorTableOut  = 'clot';
    constexpr uint32_t kTagClrIntImgState = 'ciis';
    constexpr uint32_t kTagCopyright      = 'cprt';
    constexpr uint32_t kTagCrdInfo        = 'crdi';  // ICC.1:2001-04
    constexpr uint32_t kTagDevMfgDesc     = 'dmnd';
    constexpr uint32_t kTagDevModelDesc   = 'dmdd';
    constexpr uint32_t kTagDevSettings    = 'devs';  // ICC.1:2001-04
    constexpr uint32_t kTagDToB0          = 'D2B0';
    constexpr uint32_t kTagDToB1          = 'D2B1';
    constexpr uint32_t kTagDToB2          = 'D2B2';
    constexpr uint32_t kTagDToB3          = 'D2B3';
    constexpr uint32_t kTagGamut          = 'gamt';
    constexpr uint32_t kTagGrayTRC        = 'kTRC';
    constexpr uint32_t kTagGreenMatCol    = 'gXYZ';
    constexpr uint32_t kTagGreenTRC       = 'gTRC';
    constexpr uint32_t kTagICC5           = 'ICC5';  // ???
    constexpr uint32_t kTagLuminance      = 'lumi';
    constexpr uint32_t kTagMeasure        = 'meas';
    constexpr uint32_t kTagMediaBlackPoint   = 'bkpt';  // ICC.1:2001-04
    constexpr uint32_t kTagMediaWhitePoint   = 'wtpt';
    constexpr uint32_t kTagNamedColor        = 'ncol';  // ICC.1:2001-04
    constexpr uint32_t kTagNamedColor2       = 'ncl2';
    constexpr uint32_t kTagOutputResp        = 'resp';
    constexpr uint32_t kTagPerRenderIntGamut = 'rig0';
    constexpr uint32_t kTagPreview0          = 'pre0';
    constexpr uint32_t kTagPreview1          = 'pre1';
    constexpr uint32_t kTagPreview2          = 'pre2';
    constexpr uint32_t kTagProfileDesc       = 'desc';
    constexpr uint32_t kTagProfileSeqDesc    = 'pseq';
    constexpr uint32_t kTagProfileSeqId      = 'psid';
    constexpr uint32_t kTagPS2CRD0           = 'psd0';  // ICC.1:2001-04
    constexpr uint32_t kTagPS2CRD1           = 'psd1';  // ICC.1:2001-04
    constexpr uint32_t kTagPS2CRD2           = 'psd2';  // ICC.1:2001-04
    constexpr uint32_t kTagPS2CRD3           = 'psd3';  // ICC.1:2001-04
    constexpr uint32_t kTagPS2CSA            = 'ps2s';  // ICC.1:2001-04
    constexpr uint32_t kTagPS2RenderIntent   = 'ps2i';  // ICC.1:2001-04
    constexpr uint32_t kTagRedMatCol         = 'rXYZ';
    constexpr uint32_t kTagRedTRC            = 'rTRC';
    constexpr uint32_t kTagScreeningDesc     = 'scrd';  // ICC.1:2001-04
    constexpr uint32_t kTagScreening         = 'scrn';  // ICC.1:2001-04
    constexpr uint32_t kTagSatRenderIntGamut = 'rig2';
    constexpr uint32_t kTagTechnology        = 'tech';
    constexpr uint32_t kTagUcrbg             = 'bfd ';  // ICC.1:2001-04
    constexpr uint32_t kTagViewCondDesc      = 'vued';
    constexpr uint32_t kTagViewConds         = 'view';

    constexpr uint32_t kTypeChromaticity   = 'chrm';
    constexpr uint32_t kTypeColorOrder     = 'clro';
    constexpr uint32_t kTypeColorTable     = 'clrt';
    constexpr uint32_t kTypeCRDInfo        = 'crdi';  // ICC.1:2001-04
    constexpr uint32_t kTypeCurve          = 'curv';
    constexpr uint32_t kTypeData           = 'data';
    constexpr uint32_t kTypeDateTime       = 'dtim';
    constexpr uint32_t kTypeDevSettings    = 'devs';  // ICC.1:2001-04
    constexpr uint32_t kTypeLut16          = 'mft2';
    constexpr uint32_t kTypeLut8           = 'mft1';
    constexpr uint32_t kTypeLutAToB        = 'mAB ';
    constexpr uint32_t kTypeLutBToA        = 'mBA ';
    constexpr uint32_t kTypeMeasure        = 'meas';
    constexpr uint32_t kTypeMultLocUnicode = 'mluc';
    constexpr uint32_t kTypeMultProcElem   = 'mpet';
    constexpr uint32_t kTypeNamedClr       = 'ncol';  // ICC.1:2001-04
    constexpr uint32_t kTypeNamedClr2      = 'ncl2';
    constexpr uint32_t kTypeParamCurve     = 'para';
    constexpr uint32_t kTypeProfileSeqDesc = 'pseq';
    constexpr uint32_t kTypeProfileSeqId   = 'psid';
    constexpr uint32_t kTypeRespCurveSet16 = 'rcs2';
    constexpr uint32_t kTypeS15F16Array    = 'sf32';
    constexpr uint32_t kTypeScreening      = 'scrn';  // ICC.1:2001-04
    constexpr uint32_t kTypeSignature      = 'sig ';
    constexpr uint32_t kTypeTextDesc       = 'desc';  // ICC.1:2001-04
    constexpr uint32_t kTypeText           = 'text';
    constexpr uint32_t kTypeU16F16Array    = 'uf32';
    constexpr uint32_t kTypeUCRBG          = 'bfd ';  // ICC.1:2001-04
    constexpr uint32_t kTypeUInt16Array    = 'ui16';
    constexpr uint32_t kTypeUInt32Array    = 'ui32';
    constexpr uint32_t kTypeUInt64Array    = 'ui64';
    constexpr uint32_t kTypeUInt8Array     = 'ui08';
    constexpr uint32_t kTypeViewConds      = 'view';
    constexpr uint32_t kTypeXYZ            = 'XYZ ';

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_ICC_CONSTANTS_H_