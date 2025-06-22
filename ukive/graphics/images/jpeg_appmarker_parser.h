// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGE_JPEG_APPMARKER_PARSER_H_
#define UKIVE_GRAPHICS_IMAGE_JPEG_APPMARKER_PARSER_H_

#include <istream>
#include <string>
#include <vector>


namespace ukive {

    enum mpf_mp_type {
        MPF_MP_TYPE_UNDEFINED    = 0x00000u,
        MPF_MP_TYPE_L_THUMB_CLS1 = 0x10001u,
        MPF_MP_TYPE_L_THUMB_CLS2 = 0x10002u,
        MPF_MP_TYPE_MF_PANORAMA  = 0x20001u,
        MPF_MP_TYPE_MF_DISPARITY = 0x20002u,
        MPF_MP_TYPE_MF_MUL_ANGLE = 0x20003u,
        MPF_MP_TYPE_MP_PRIMARY   = 0x30000u,
        MPF_MP_TYPE_MP_ULTRAHDR  = 0x50000u, // ???
    };

    enum mpf_mp_attr_flags {
        MPF_MP_AF_DEP_PARENT = 0x80000000u,
        MPF_MP_AF_DEP_CHILD  = 0x40000000u,
        MPF_MP_AF_REP_IMG    = 0x20000000u,
        MPF_MP_AF_IMG_FORMAT = 0x07000000u,
        MPF_MP_AF_TYPE_CODE  = 0x00FFFFFFu,
    };

    struct jpeg_ifd_de {
        uint16_t tag;
        uint16_t type;
        uint32_t count;
        uint32_t offset;
    };

    struct jpeg_mp_entry {
        uint32_t indiv_img_attr;
        uint32_t indiv_img_size;
        uint32_t indiv_img_data_offset;
        uint16_t dep_img1_entry_num;
        uint16_t dep_img2_entry_num;
    };

    struct jpeg_mp_index_ifd {
        jpeg_ifd_de version;
        jpeg_ifd_de img_num;
        jpeg_ifd_de mp_ent;
        jpeg_ifd_de indiv_img_uid_list;
        jpeg_ifd_de total_cap_frame_num;
        uint32_t next_ifd_offset;

        // data pointed by mp_ent
        std::vector<jpeg_mp_entry> mp_entries;
    };

    struct jpeg_mpf {
        size_t offset_start; // start offset in entire JPEG file
        jpeg_mp_index_ifd index_ifd;
    };

    struct iso_21496_1_channel {
        int32_t gain_map_min_num;
        uint32_t gain_map_min_den;
        int32_t gain_map_max_num;
        uint32_t gain_map_max_den;
        uint32_t gamma_num;
        uint32_t gamma_den;
        int32_t base_offset_num;
        uint32_t base_offset_den;
        int32_t alter_offset_num;
        uint32_t alter_offset_den;
    };

    struct iso_21496_1 {
        uint16_t min_ver;
        uint16_t wrt_ver;

        bool valid;
        bool is_mul_channel;
        bool use_base_color_space;
        uint32_t base_hdr_headroom_num;
        uint32_t base_hdr_headroom_den;
        uint32_t alter_hdr_headroom_num;
        uint32_t alter_hdr_headroom_den;

        size_t channel_count;
        iso_21496_1_channel channels[3];
    };

    // JFIF
    struct jpeg_app0 {
    };

    // Exif or XMP
    struct jpeg_app1 {
        uint8_t type; // 0:Exif, 1:XMP
        std::string xmp;
    };

    // MPF, ICC or ISO 21496-1
    struct jpeg_app2 {
        uint8_t type; // 0:ICC, 1:MPF, 2:ISO 21496-1
        jpeg_mpf mpf;
        iso_21496_1 iso_21496_1;
    };

    // Photoshop
    struct jpeg_app13 {
    };

    // Adobe
    struct jpeg_app14 {
    };

    struct jpeg_metadata {
        std::vector<jpeg_app0> app0s;
        std::vector<jpeg_app1> app1s;
        std::vector<jpeg_app2> app2s;
        std::vector<jpeg_app13> app13s;
        std::vector<jpeg_app14> app14s;
    };


    bool parse_jpeg_metadata(std::istream& s, jpeg_metadata* out);

    bool parse_jpeg_appmarker(std::istream& s, jpeg_metadata* out);

}

#endif  // UKIVE_GRAPHICS_IMAGE_JPEG_APPMARKER_PARSER_H_