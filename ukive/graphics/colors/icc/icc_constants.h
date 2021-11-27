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

    constexpr uint32_t kFileSignature = 0x61637370; // 'acsp'

    constexpr uint32_t kClassInputDevice   = 0x73636E72; // 'scnr'
    constexpr uint32_t kClassDisplayDevice = 0x6D6E7472; // 'mntr'
    constexpr uint32_t kClassOutputDevice  = 0x70727472; // 'prtr'
    constexpr uint32_t kClassDeviceLink    = 0x6C696E6B; // 'link'
    constexpr uint32_t kClassColorSpace    = 0x73706163; // 'spac'
    constexpr uint32_t kClassAbstract      = 0x61627374; // 'abst'
    constexpr uint32_t kClassNamedColor    = 0x6E6D636C; // 'nmcl'

    constexpr uint32_t kColorSpaceXYZ   = 0x58595A20; // 'XYZ '
    constexpr uint32_t kColorSpaceLAB   = 0x4C616220; // 'Lab '
    constexpr uint32_t kColorSpaceLUV   = 0x4C757620; // 'Luv '
    constexpr uint32_t kColorSpaceYCbCr = 0x59436272; // 'YCbr'
    constexpr uint32_t kColorSpaceYxy   = 0x59787920; // 'Yxy '
    constexpr uint32_t kColorSpaceRGB   = 0x52474220; // 'RGB '
    constexpr uint32_t kColorSpaceGray  = 0x47524159; // 'GRAY'
    constexpr uint32_t kColorSpaceHSV   = 0x48535620; // 'HSV '
    constexpr uint32_t kColorSpaceHLS   = 0x484C5320; // 'HLS '
    constexpr uint32_t kColorSpaceCMYK  = 0x434D594B; // 'CMYK'
    constexpr uint32_t kColorSpaceCMY   = 0x434D5920; // 'CMY '
    constexpr uint32_t kColorSpace2Clr  = 0x32434C52; // '2CLR'
    constexpr uint32_t kColorSpace3Clr  = 0x33434C52; // '3CLR'
    constexpr uint32_t kColorSpace4Clr  = 0x34434C52; // '4CLR'
    constexpr uint32_t kColorSpace5Clr  = 0x35434C52; // '5CLR'
    constexpr uint32_t kColorSpace6Clr  = 0x36434C52; // '6CLR'
    constexpr uint32_t kColorSpace7Clr  = 0x37434C52; // '7CLR'
    constexpr uint32_t kColorSpace8Clr  = 0x38434C52; // '8CLR'
    constexpr uint32_t kColorSpace9Clr  = 0x39434C52; // '9CLR'
    constexpr uint32_t kColorSpace10Clr = 0x41434C52; // 'ACLR'
    constexpr uint32_t kColorSpace11Clr = 0x42434C52; // 'BCLR'
    constexpr uint32_t kColorSpace12Clr = 0x43434C52; // 'CCLR'
    constexpr uint32_t kColorSpace13Clr = 0x44434C52; // 'DCLR'
    constexpr uint32_t kColorSpace14Clr = 0x45434C52; // 'ECLR'
    constexpr uint32_t kColorSpace15Clr = 0x46434C52; // 'FCLR'

    constexpr uint32_t kPrimaryPlatformApple = 0x4150504C; // 'APPL'
    constexpr uint32_t kPrimaryPlatformMS    = 0x4D534654; // 'MSFT'
    constexpr uint32_t kPrimaryPlatformSGI   = 0x53474920; // 'SGI '
    constexpr uint32_t kPrimaryPlatformSMI   = 0x53554E57; // 'SUNW'

    constexpr uint32_t kTagAToB0      = 0x41324230; // 'A2B0'
    constexpr uint32_t kTagAToB1      = 0x41324231; // 'A2B1'
    constexpr uint32_t kTagAToB2      = 0x41324232; // 'A2B2'
    constexpr uint32_t kTagBlueMatCol = 0x6258595A; // 'bXYZ'
    constexpr uint32_t kTagBlueTRC    = 0x62545243; // 'bTRC'
    constexpr uint32_t kTagBToA0      = 0x42324130; // 'B2A0'
    constexpr uint32_t kTagBToA1      = 0x42324131; // 'B2A1'
    constexpr uint32_t kTagBToA2      = 0x42324132; // 'B2A2'
    constexpr uint32_t kTagBToD0      = 0x42324430; // 'B2D0'
    constexpr uint32_t kTagBToD1      = 0x42324431; // 'B2D1'
    constexpr uint32_t kTagBToD2      = 0x42324432; // 'B2D2'
    constexpr uint32_t kTagBToD3      = 0x42324433; // 'B2D3'
    constexpr uint32_t kTagCalibDateTime  = 0x63616C74; // 'calt'
    constexpr uint32_t kTagCharTarget     = 0x74617267; // 'targ'
    constexpr uint32_t kTagChromAdapt     = 0x63686164; // 'chad'
    constexpr uint32_t kTagChromaticity   = 0x6368726D; // 'chrm'
    constexpr uint32_t kTagColorOrder     = 0x636C726F; // 'clro'
    constexpr uint32_t kTagColorTable     = 0x636C7274; // 'clrt'
    constexpr uint32_t kTagColorTableOut  = 0x636C6F74; // 'clot'
    constexpr uint32_t kTagClrIntImgState = 0x63696973; // 'ciis'
    constexpr uint32_t kTagCopyright      = 0x63707274; // 'cprt'
    constexpr uint32_t kTagCrdInfo        = 0x63726469; // 'crdi'  ICC.1:2001-04
    constexpr uint32_t kTagDevMfgDesc     = 0x646D6E64; // 'dmnd'
    constexpr uint32_t kTagDevModelDesc   = 0x646D6464; // 'dmdd'
    constexpr uint32_t kTagDevSettings    = 0x64657673; // 'devs'  ICC.1:2001-04
    constexpr uint32_t kTagDToB0          = 0x44324230; // 'D2B0'
    constexpr uint32_t kTagDToB1          = 0x44324231; // 'D2B1'
    constexpr uint32_t kTagDToB2          = 0x44324232; // 'D2B2'
    constexpr uint32_t kTagDToB3          = 0x44324233; // 'D2B3'
    constexpr uint32_t kTagGamut          = 0x67616D74; // 'gamt'
    constexpr uint32_t kTagGrayTRC        = 0x6B545243; // 'kTRC'
    constexpr uint32_t kTagGreenMatCol    = 0x6758595A; // 'gXYZ'
    constexpr uint32_t kTagGreenTRC       = 0x67545243; // 'gTRC'
    constexpr uint32_t kTagICC5           = 0x49434335; // 'ICC5'  ???
    constexpr uint32_t kTagLuminance      = 0x6C756D69; // 'lumi'
    constexpr uint32_t kTagMeasure        = 0x6D656173; // 'meas'
    constexpr uint32_t kTagMediaBlackPoint   = 0x626B7074; // 'bkpt'  ICC.1:2001-04
    constexpr uint32_t kTagMediaWhitePoint   = 0x77747074; // 'wtpt'
    constexpr uint32_t kTagNamedColor        = 0x6E636F6C; // 'ncol'  ICC.1:2001-04
    constexpr uint32_t kTagNamedColor2       = 0x6E636C32; // 'ncl2'
    constexpr uint32_t kTagOutputResp        = 0x72657370; // 'resp'
    constexpr uint32_t kTagPerRenderIntGamut = 0x72696730; // 'rig0'
    constexpr uint32_t kTagPreview0          = 0x70726530; // 'pre0'
    constexpr uint32_t kTagPreview1          = 0x70726531; // 'pre1'
    constexpr uint32_t kTagPreview2          = 0x70726532; // 'pre2'
    constexpr uint32_t kTagProfileDesc       = 0x64657363; // 'desc'
    constexpr uint32_t kTagProfileSeqDesc    = 0x70736571; // 'pseq'
    constexpr uint32_t kTagProfileSeqId      = 0x70736964; // 'psid'
    constexpr uint32_t kTagPS2CRD0           = 0x70736430; // 'psd0'  ICC.1:2001-04
    constexpr uint32_t kTagPS2CRD1           = 0x70736431; // 'psd1'  ICC.1:2001-04
    constexpr uint32_t kTagPS2CRD2           = 0x70736432; // 'psd2'  ICC.1:2001-04
    constexpr uint32_t kTagPS2CRD3           = 0x70736433; // 'psd3'  ICC.1:2001-04
    constexpr uint32_t kTagPS2CSA            = 0x70733273; // 'ps2s'  ICC.1:2001-04
    constexpr uint32_t kTagPS2RenderIntent   = 0x70733269; // 'ps2i'  ICC.1:2001-04
    constexpr uint32_t kTagRedMatCol         = 0x7258595A; // 'rXYZ'
    constexpr uint32_t kTagRedTRC            = 0x72545243; // 'rTRC'
    constexpr uint32_t kTagScreeningDesc     = 0x73637264; // 'scrd'  ICC.1:2001-04
    constexpr uint32_t kTagScreening         = 0x7363726E; // 'scrn'  ICC.1:2001-04
    constexpr uint32_t kTagSatRenderIntGamut = 0x72696732; // 'rig2'
    constexpr uint32_t kTagTechnology        = 0x74656368; // 'tech'
    constexpr uint32_t kTagUcrbg             = 0x62666420; // 'bfd '  ICC.1:2001-04
    constexpr uint32_t kTagViewCondDesc      = 0x76756564; // 'vued'
    constexpr uint32_t kTagViewConds         = 0x76696577; // 'view'

    constexpr uint32_t kTypeChromaticity   = 0x6368726D; // 'chrm'
    constexpr uint32_t kTypeColorOrder     = 0X636C726F; // 'clro'
    constexpr uint32_t kTypeColorTable     = 0x636c7274; // 'clrt'
    constexpr uint32_t kTypeCRDInfo        = 0x63726469; // 'crdi'  ICC.1:2001-04
    constexpr uint32_t kTypeCurve          = 0x63757276; // 'curv'
    constexpr uint32_t kTypeData           = 0x64617461; // 'data'
    constexpr uint32_t kTypeDateTime       = 0x6474696D; // 'dtim'
    constexpr uint32_t kTypeDevSettings    = 0x64657673; // 'devs'  ICC.1:2001-04
    constexpr uint32_t kTypeLut16          = 0x6D667432; // 'mft2'
    constexpr uint32_t kTypeLut8           = 0x6D667431; // 'mft1'
    constexpr uint32_t kTypeLutAToB        = 0x6D414220; // 'mAB '
    constexpr uint32_t kTypeLutBToA        = 0x6D424120; // 'mBA '
    constexpr uint32_t kTypeMeasure        = 0x6D656173; // 'meas'
    constexpr uint32_t kTypeMultLocUnicode = 0x6D6C7563; // 'mluc'
    constexpr uint32_t kTypeMultProcElem   = 0x6D706574; // 'mpet'
    constexpr uint32_t kTypeNamedClr       = 0x6E636F6C; // 'ncol'  ICC.1:2001-04
    constexpr uint32_t kTypeNamedClr2      = 0x6E636C32; // 'ncl2'
    constexpr uint32_t kTypeParamCurve     = 0x70617261; // 'para'
    constexpr uint32_t kTypeProfileSeqDesc = 0x70736571; // 'pseq'
    constexpr uint32_t kTypeProfileSeqId   = 0x70736964; // 'psid'
    constexpr uint32_t kTypeRespCurveSet16 = 0x72637332; // 'rcs2'
    constexpr uint32_t kTypeS15F16Array    = 0x73663332; // 'sf32'
    constexpr uint32_t kTypeScreening      = 0x7363726E; // 'scrn'  ICC.1:2001-04
    constexpr uint32_t kTypeSignature      = 0x73696720; // 'sig '
    constexpr uint32_t kTypeTextDesc       = 0x64657363; // 'desc'  ICC.1:2001-04
    constexpr uint32_t kTypeText           = 0x74657874; // 'text'
    constexpr uint32_t kTypeU16F16Array    = 0x75663332; // 'uf32'
    constexpr uint32_t kTypeUCRBG          = 0x62666420; // 'bfd '  ICC.1:2001-04
    constexpr uint32_t kTypeUInt16Array    = 0x75693136; // 'ui16'
    constexpr uint32_t kTypeUInt32Array    = 0x75693332; // 'ui32'
    constexpr uint32_t kTypeUInt64Array    = 0x75693634; // 'ui64'
    constexpr uint32_t kTypeUInt8Array     = 0x75693038; // 'ui08'
    constexpr uint32_t kTypeViewConds      = 0x76696577; // 'view'
    constexpr uint32_t kTypeXYZ            = 0x58595A20; // 'XYZ '

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_ICC_CONSTANTS_H_