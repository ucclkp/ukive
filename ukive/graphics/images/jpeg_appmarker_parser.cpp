// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "jpeg_appmarker_parser.h"

#include <cstdint>
#include <cstring>
#include <fstream>

#include <utils/stream_utils.h>

#define READ_STREAM_E(var, size, big_endian)  \
    READ_STREAM(var, size)  \
    static_assert(size == sizeof(var), "The size of 'var' must be equal to 'size'");  \
    if (big_endian) var = utl::fromToBE(var); else var = utl::fromToLE(var);


bool read_c_ascii(std::istream & s, char* buf, size_t max_len) {
    for (size_t i = 0;; ++i) {
        char c; GET_STREAM(c);
        if (i < max_len) {
            buf[i] = c;
        }
        if (!c) break;
    }
    return true;
}

namespace ukive {

    bool parse_jpeg_ifd_de(std::istream& s, bool big_endian, jpeg_ifd_de* out) {
        READ_STREAM_E(out->tag, 2, big_endian);
        READ_STREAM_E(out->type, 2, big_endian);
        READ_STREAM_E(out->count, 4, big_endian);
        READ_STREAM_E(out->offset, 4, big_endian);
        return true;
    }

    bool parse_mp_entry(std::istream& s, bool big_endian, jpeg_mp_entry* out) {
        READ_STREAM_E(out->indiv_img_attr, 4, big_endian);
        READ_STREAM_E(out->indiv_img_size, 4, big_endian);
        READ_STREAM_E(out->indiv_img_data_offset, 4, big_endian);
        READ_STREAM_E(out->dep_img1_entry_num, 2, big_endian);
        READ_STREAM_E(out->dep_img2_entry_num, 2, big_endian);
        return true;
    }

    bool parse_jpeg_xmp(std::istream& s, size_t len, std::string* out) {
        out->resize(len);
        s.read(&(*out)[0], len);
        return true;
    }

    bool parse_21496_1(std::istream& s, size_t len, iso_21496_1* out) {
        READ_STREAM_BE(out->min_ver, 2);
        READ_STREAM_BE(out->wrt_ver, 2);
        if (out->min_ver == 0 && len > 4) {
            uint8_t tmp1;
            READ_STREAM(tmp1, 1);
            out->is_mul_channel = (tmp1 & 0x80u) != 0;
            out->use_base_color_space = (tmp1 & 0x40u) != 0;

            READ_STREAM_BE(out->base_hdr_headroom_num, 4);
            READ_STREAM_BE(out->base_hdr_headroom_den, 4);
            READ_STREAM_BE(out->alter_hdr_headroom_num, 4);
            READ_STREAM_BE(out->alter_hdr_headroom_den, 4);

            out->channel_count = out->is_mul_channel ? 3 : 1;
            for (size_t i = 0; i < out->channel_count; ++i) {
                iso_21496_1_channel& channel = out->channels[i];
                READ_STREAM_BE(channel.gain_map_min_num, 4);
                READ_STREAM_BE(channel.gain_map_min_den, 4);
                READ_STREAM_BE(channel.gain_map_max_num, 4);
                READ_STREAM_BE(channel.gain_map_max_den, 4);
                READ_STREAM_BE(channel.gamma_num, 4);
                READ_STREAM_BE(channel.gamma_den, 4);
                READ_STREAM_BE(channel.base_offset_num, 4);
                READ_STREAM_BE(channel.base_offset_den, 4);
                READ_STREAM_BE(channel.alter_offset_num, 4);
                READ_STREAM_BE(channel.alter_offset_den, 4);
            }
            out->valid = true;
        } else {
            out->valid = false;
        }
        return true;
    }

    bool parse_jpeg_mpf(std::istream& s, jpeg_mp_index_ifd* out) {
        auto offset_start = (size_t)s.tellg();

        char endian_id[4];
        READ_STREAM(endian_id, 4);

        bool big_endian;
        if (endian_id[0] == 0x4Du && endian_id[1] == 0x4Du &&
            endian_id[2] == 0x00u && endian_id[3] == 0x2Au)
        {
            big_endian = true;
        } else if (
            endian_id[0] == 0x49u && endian_id[1] == 0x49u &&
            endian_id[2] == 0x2Au && endian_id[3] == 0x00u)
        {
            big_endian = false;
        } else {
            return false;
        }

        uint32_t offset_to_first_ifd;
        READ_STREAM_E(offset_to_first_ifd, 4, big_endian);

        // to first IFD
        SEEKG_STREAM(offset_start + offset_to_first_ifd);

        uint16_t de_count;
        READ_STREAM_E(de_count, 2, big_endian);

        jpeg_mp_index_ifd idx_ifd;
        memset(&idx_ifd, 0, sizeof(idx_ifd));

        for (uint16_t i = 0; i < de_count; ++i) {
            jpeg_ifd_de de;
            if (!parse_jpeg_ifd_de(s, big_endian, &de)) {
                return false;
            }

            switch (de.tag) {
            case 0xB000u: idx_ifd.version = de; break;
            case 0xB001u: idx_ifd.img_num = de; break;
            case 0xB002u: idx_ifd.mp_ent  = de; break;
            case 0xB003u: idx_ifd.indiv_img_uid_list  = de; break;
            case 0xB004u: idx_ifd.total_cap_frame_num = de; break;
            default: break;
            }
        }

        READ_STREAM_E(idx_ifd.next_ifd_offset, 4, big_endian);

        // to MP Entry #1
        SEEKG_STREAM(offset_start + idx_ifd.mp_ent.offset);

        for (size_t i = 0; i < idx_ifd.img_num.offset; ++i) {
            jpeg_mp_entry me;
            if (!parse_mp_entry(s, big_endian, &me)) {
                return false;
            }
            idx_ifd.mp_entries.push_back(me);
        }

        *out = idx_ifd;
        return true;
    }

    bool parse_jpeg_app1(std::istream& s, size_t len, jpeg_app1* out) {
        char id[64];
        if (!read_c_ascii(s, id, sizeof(id))) {
            return false;
        }

        if (std::memcmp(id, "http://ns.adobe.com/xap/1.0/", 29) == 0) {
            if (len < 29) {
                return false;
            }

            std::string xmp;
            if (!parse_jpeg_xmp(s, len - 29, &xmp)) {
                return false;
            }

            out->type = 1;  // XMP
            out->xmp = std::move(xmp);
        } else {
            return false;
        }

        return true;
    }

    bool parse_jpeg_app2(std::istream& s, size_t len, jpeg_app2* out) {
        char id[64];
        if (!read_c_ascii(s, id, sizeof(id))) {
            return false;
        }

        if (std::memcmp(id, "MPF", 4) == 0) {
            if (len < 4) {
                return false;
            }
            auto offset_start = (size_t)s.tellg();

            jpeg_mp_index_ifd index_ifd;
            if (!parse_jpeg_mpf(s, &index_ifd)) {
                return false;
            }

            out->type = 1;
            out->mpf.index_ifd = index_ifd;
            out->mpf.offset_start = (size_t)offset_start;
        } else if (std::memcmp(id, "urn:iso:std:iso:ts:21496:-1", 28) == 0) {
            if (len < 28) {
                return false;
            }
            iso_21496_1 iso_21496;
            if (!parse_21496_1(s, len - 28, &iso_21496)) {
                return false;
            }

            out->type = 2;
            out->iso_21496_1 = iso_21496;
        }

        return true;
    }

    bool parse_jpeg_appmarker_wo_marker(std::istream& s, uint8_t marker[4], jpeg_metadata* out) {
        auto start = (size_t)s.tellg();
        size_t marker_length = (marker[2] << 8) | marker[3];
        if (marker_length < 2) {
            return false;
        }

        unsigned int app_type = marker[1] & 0x0Fu;
        switch (app_type) {
        case 0x0:
        {
            // APP0
            SKIP_BYTES(marker_length - 2);
            return true;
        }
        case 0x1:
        {
            // APP1
            jpeg_app1 app1;
            if (parse_jpeg_app1(s, marker_length - 2, &app1)) {
                out->app1s.push_back(std::move(app1));
            }
            SEEKG_STREAM(start + marker_length - 2);
            return true;
        }
        case 0x2:
        {
            // APP2
            jpeg_app2 app2;
            if (parse_jpeg_app2(s, marker_length - 2, &app2)) {
                out->app2s.push_back(std::move(app2));
            }
            SEEKG_STREAM(start + marker_length - 2);
            return true;
        }
        case 0xD:
        {
            // APP13
            jpeg_app13 app13;
            SKIP_BYTES(marker_length - 2);
            return true;
        }
        case 0xE:
        {
            // APP14
            jpeg_app14 app14;
            SKIP_BYTES(marker_length - 2);
            return true;
        }
        default:
            SKIP_BYTES(marker_length - 2);
            return true;
        }
    }

    bool parse_jpeg_appmarker(std::istream& s, jpeg_metadata* out) {
        uint8_t marker[4];
        READ_STREAM(marker, 4);
        if (marker[0] != 0xFFu || (marker[1] & 0xE0u) != 0xE0u) {
            return false;
        }
        return parse_jpeg_appmarker_wo_marker(s, marker, out);
    }

    bool parse_jpeg_metadata(std::istream& s, jpeg_metadata* out) {
        uint8_t soi[2];
        READ_STREAM(soi, 2);
        if (soi[0] != 0xFFu || soi[1] != 0xD8u) {
            return false;  // Not a valid JPEG SOI marker
        }

        for (;;) {
            uint8_t marker[4];
            READ_STREAM(marker, 4);
            if (marker[0] != 0xFFu || (marker[1] & 0xE0u) != 0xE0u) {
                break;
            }
            if (!parse_jpeg_appmarker_wo_marker(s, marker, out)) {
                return false;
            }
        }

        return true;
    }

}