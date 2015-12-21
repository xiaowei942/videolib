#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "golomb.h"
#include "GetBitContext.h"
#include "sps_resolution.h"
// fixme: copied from h264data.h
static const uint8_t zigzag_scan[16]={
    0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0 + 2 * 4,
    1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4,
    1 + 2 * 4, 0 + 3 * 4, 1 + 3 * 4, 2 + 2 * 4,
    3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3 + 3 * 4,
};

const uint8_t ff_zigzag_direct[64] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

static const uint8_t default_scaling4[2][16] = {
    {  6, 13, 20, 28, 13, 20, 28, 32,
      20, 28, 32, 37, 28, 32, 37, 42 },
    { 10, 14, 20, 24, 14, 20, 24, 27,
      20, 24, 27, 30, 24, 27, 30, 34 }
};

static const uint8_t default_scaling8[2][64] = {
    {  6, 10, 13, 16, 18, 23, 25, 27,
      10, 11, 16, 18, 23, 25, 27, 29,
      13, 16, 18, 23, 25, 27, 29, 31,
      16, 18, 23, 25, 27, 29, 31, 33,
      18, 23, 25, 27, 29, 31, 33, 36,
      23, 25, 27, 29, 31, 33, 36, 38,
      25, 27, 29, 31, 33, 36, 38, 40,
      27, 29, 31, 33, 36, 38, 40, 42 },
    {  9, 13, 15, 17, 19, 21, 22, 24,
      13, 13, 17, 19, 21, 22, 24, 25,
      15, 17, 19, 21, 22, 24, 25, 27,
      17, 19, 21, 22, 24, 25, 27, 28,
      19, 21, 22, 24, 25, 27, 28, 30,
      21, 22, 24, 25, 27, 28, 30, 32,
      22, 24, 25, 27, 28, 30, 32, 33,
      24, 25, 27, 28, 30, 32, 33, 35 }
};

#define MAX_LOG2_MAX_FRAME_NUM    (12 + 4)
#define MIN_LOG2_MAX_FRAME_NUM    4

#define MAX_SPS_COUNT 32
#define H264_MAX_PICTURE_COUNT 36

/* Minimum and maximum values a `signed int' can hold.  */
#undef INT_MIN
#define INT_MIN (-INT_MAX - 1)
#undef INT_MAX
#define INT_MAX __INT_MAX__
#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

static void decode_scaling_list(GetBitContext *gb, uint8_t *factors, int size,
                                const uint8_t *jvt_list,
                                const uint8_t *fallback_list)
{
    int i, last = 8, next = 8;
    const uint8_t *scan = size == 16 ? zigzag_scan : ff_zigzag_direct;
    if (!get_bits1(gb)) /* matrix not written, we use the predicted one */
        memcpy(factors, fallback_list, size * sizeof(uint8_t));
    else
        for (i = 0; i < size; i++) {
            if (next)
                next = (last + get_se_golomb(gb)) & 0xff;
            if (!i && !next) { /* matrix not written, we use the preset one */
                memcpy(factors, jvt_list, size * sizeof(uint8_t));
                break;
            }
            last = factors[scan[i]] = next ? next : last;
        }
}

static void decode_scaling_matrices(GetBitContext *gb, SPS *sps,
                                    PPS *pps, int is_sps,
                                    uint8_t(*scaling_matrix4)[16],
                                    uint8_t(*scaling_matrix8)[64])
{
    int fallback_sps = !is_sps && sps->scaling_matrix_present;
    const uint8_t *fallback[4] = {
        fallback_sps ? sps->scaling_matrix4[0] : default_scaling4[0],
        fallback_sps ? sps->scaling_matrix4[3] : default_scaling4[1],
        fallback_sps ? sps->scaling_matrix8[0] : default_scaling8[0],
        fallback_sps ? sps->scaling_matrix8[3] : default_scaling8[1]
    };
    if (get_bits1(gb)) {
        sps->scaling_matrix_present |= is_sps;
        decode_scaling_list(gb, scaling_matrix4[0], 16, default_scaling4[0], fallback[0]);        // Intra, Y
        decode_scaling_list(gb, scaling_matrix4[1], 16, default_scaling4[0], scaling_matrix4[0]); // Intra, Cr
        decode_scaling_list(gb, scaling_matrix4[2], 16, default_scaling4[0], scaling_matrix4[1]); // Intra, Cb
        decode_scaling_list(gb, scaling_matrix4[3], 16, default_scaling4[1], fallback[1]);        // Inter, Y
        decode_scaling_list(gb, scaling_matrix4[4], 16, default_scaling4[1], scaling_matrix4[3]); // Inter, Cr
        decode_scaling_list(gb, scaling_matrix4[5], 16, default_scaling4[1], scaling_matrix4[4]); // Inter, Cb
        if (is_sps || pps->transform_8x8_mode) {
            decode_scaling_list(gb, scaling_matrix8[0], 64, default_scaling8[0], fallback[2]); // Intra, Y
            decode_scaling_list(gb, scaling_matrix8[3], 64, default_scaling8[1], fallback[3]); // Inter, Y
            if (sps->chroma_format_idc == 3) {
                decode_scaling_list(gb, scaling_matrix8[1], 64, default_scaling8[0], scaling_matrix8[0]); // Intra, Cr
                decode_scaling_list(gb, scaling_matrix8[4], 64, default_scaling8[1], scaling_matrix8[3]); // Inter, Cr
                decode_scaling_list(gb, scaling_matrix8[2], 64, default_scaling8[0], scaling_matrix8[1]); // Intra, Cb
                decode_scaling_list(gb, scaling_matrix8[5], 64, default_scaling8[1], scaling_matrix8[4]); // Inter, Cb
            }
        }
    }
}

#if 1
int ff_h264_decode_seq_parameter_set(GetBitContext *gb, Screen_Info *info)
{
    int profile_idc, level_idc, constraint_set_flags = 0;
    unsigned int sps_id;
    int i, log2_max_frame_num_minus4;
    SPS *sps;
    sps = (SPS*)malloc(sizeof(SPS));
    if (!sps)
        return -1;
    memset(sps, 0x0, sizeof(SPS));
    sps->data_size = gb->buffer_end - gb->buffer;
    if (sps->data_size > sizeof(sps->data)) {
        printf("Truncating likely oversized SPS\n");
        sps->data_size = sizeof(sps->data);
    }
    memcpy(sps->data, gb->buffer, sps->data_size);

    profile_idc           = get_bits(gb, 8);
    constraint_set_flags |= get_bits1(gb) << 0;   // constraint_set0_flag
    constraint_set_flags |= get_bits1(gb) << 1;   // constraint_set1_flag
    constraint_set_flags |= get_bits1(gb) << 2;   // constraint_set2_flag
    constraint_set_flags |= get_bits1(gb) << 3;   // constraint_set3_flag
    constraint_set_flags |= get_bits1(gb) << 4;   // constraint_set4_flag
    constraint_set_flags |= get_bits1(gb) << 5;   // constraint_set5_flag
    skip_bits(gb, 2);                             // reserved_zero_2bits
    level_idc = get_bits(gb, 8);
    sps_id    = get_ue_golomb_31(gb);

    if (sps_id >= MAX_SPS_COUNT) {
        printf("sps_id %u out of range\n", sps_id);
        goto fail;
    }

    sps->sps_id               = sps_id;
    sps->time_offset_length   = 24;
    sps->profile_idc          = profile_idc;
    sps->constraint_set_flags = constraint_set_flags;
    sps->level_idc            = level_idc;
    sps->full_range           = -1;

    memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
    memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
    sps->scaling_matrix_present = 0;
    sps->colorspace = 2; //AVCOL_SPC_UNSPECIFIED

    if (sps->profile_idc == 100 ||  // High profile
        sps->profile_idc == 110 ||  // High10 profile
        sps->profile_idc == 122 ||  // High422 profile
        sps->profile_idc == 244 ||  // High444 Predictive profile
        sps->profile_idc ==  44 ||  // Cavlc444 profile
        sps->profile_idc ==  83 ||  // Scalable Constrained High profile (SVC)
        sps->profile_idc ==  86 ||  // Scalable High Intra profile (SVC)
        sps->profile_idc == 118 ||  // Stereo High profile (MVC)
        sps->profile_idc == 128 ||  // Multiview High profile (MVC)
        sps->profile_idc == 138 ||  // Multiview Depth High profile (MVCD)
        sps->profile_idc == 144) {  // old High444 profile
        sps->chroma_format_idc = get_ue_golomb_31(gb);
        if (sps->chroma_format_idc > 3U) {
            //avpriv_request_sample(avctx, "chroma_format_idc %u",
             //                     sps->chroma_format_idc);
            goto fail;
        } else if (sps->chroma_format_idc == 3) {
            sps->residual_color_transform_flag = get_bits1(gb);
            if (sps->residual_color_transform_flag) {
                printf("separate color planes are not supported\n");
                goto fail;
            }
        }
        sps->bit_depth_luma   = get_ue_golomb(gb) + 8;
        sps->bit_depth_chroma = get_ue_golomb(gb) + 8;
        if (sps->bit_depth_chroma != sps->bit_depth_luma) {
            //avpriv_request_sample(avctx,
             printf("Different chroma and luma bit depth");
            goto fail;
        }
        if (sps->bit_depth_luma   < 8 || sps->bit_depth_luma   > 14 ||
            sps->bit_depth_chroma < 8 || sps->bit_depth_chroma > 14) {
            printf("illegal bit depth value (%d, %d)\n",
                   sps->bit_depth_luma, sps->bit_depth_chroma);
            goto fail;
        }
        sps->transform_bypass = get_bits1(gb);
        decode_scaling_matrices(gb, sps, NULL, 1,
                                sps->scaling_matrix4, sps->scaling_matrix8);
    } else {
        sps->chroma_format_idc = 1;
        sps->bit_depth_luma    = 8;
        sps->bit_depth_chroma  = 8;
    }

    log2_max_frame_num_minus4 = get_ue_golomb(gb);
    if (log2_max_frame_num_minus4 < MIN_LOG2_MAX_FRAME_NUM - 4 ||
        log2_max_frame_num_minus4 > MAX_LOG2_MAX_FRAME_NUM - 4) {
               printf("log2_max_frame_num_minus4 out of range (0-12): %d\n",
               log2_max_frame_num_minus4);
        goto fail;
    }
    sps->log2_max_frame_num = log2_max_frame_num_minus4 + 4;

    sps->poc_type = get_ue_golomb_31(gb);

    if (sps->poc_type == 0) { // FIXME #define
        unsigned t = get_ue_golomb(gb);
        if (t>12) {
            printf("log2_max_poc_lsb (%d) is out of range\n", t);
            goto fail;
        }
        sps->log2_max_poc_lsb = t + 4;
    } else if (sps->poc_type == 1) { // FIXME #define
        sps->delta_pic_order_always_zero_flag = get_bits1(gb);
        sps->offset_for_non_ref_pic           = get_se_golomb(gb);
        sps->offset_for_top_to_bottom_field   = get_se_golomb(gb);
        sps->poc_cycle_length                 = get_ue_golomb(gb);

        if ((unsigned)sps->poc_cycle_length >=
            FF_ARRAY_ELEMS(sps->offset_for_ref_frame)) {
                   printf("poc_cycle_length overflow %d\n", sps->poc_cycle_length);
            goto fail;
        }

        for (i = 0; i < sps->poc_cycle_length; i++)
            sps->offset_for_ref_frame[i] = get_se_golomb(gb);
    } else if (sps->poc_type != 2) {
        printf("illegal POC type %d\n", sps->poc_type);
        goto fail;
    }

    sps->ref_frame_count = get_ue_golomb_31(gb);
//    if (avctx->codec_tag == MKTAG('S', 'M', 'V', '2'))
//        sps->ref_frame_count = FFMAX(2, sps->ref_frame_count);
    if (sps->ref_frame_count > H264_MAX_PICTURE_COUNT - 2 ||
        sps->ref_frame_count > 16U) {
               printf("too many reference frames %d\n", sps->ref_frame_count);
        goto fail;
    }
    sps->gaps_in_frame_num_allowed_flag = get_bits1(gb);
    sps->mb_width                       = get_ue_golomb(gb) + 1;
    sps->mb_height                      = get_ue_golomb(gb) + 1;

    info->width=sps->mb_width*16;
info->height=sps->mb_height*16;
//	printf("Width: %d, Height: %d\n", sps->mb_width, sps->mb_height);

#if 0
    if ((unsigned)sps->mb_width  >= INT_MAX / 16 ||
        (unsigned)sps->mb_height >= INT_MAX / 16 ||
        av_image_check_size(16 * sps->mb_width,
                            16 * sps->mb_height, 0, avctx)) {
        printf("mb_width/height overflow\n");
        goto fail;
    }
#endif

    sps->frame_mbs_only_flag = get_bits1(gb);
    if (!sps->frame_mbs_only_flag)
        sps->mb_aff = get_bits1(gb);
    else
        sps->mb_aff = 0;

    sps->direct_8x8_inference_flag = get_bits1(gb);

#ifndef ALLOW_INTERLACE
    if (sps->mb_aff)
               printf("MBAFF support not included; enable it at compile-time.\n");
#endif
    sps->crop = get_bits1(gb);
    if (sps->crop) {
        unsigned int crop_left   = get_ue_golomb(gb);
        unsigned int crop_right  = get_ue_golomb(gb);
        unsigned int crop_top    = get_ue_golomb(gb);
        unsigned int crop_bottom = get_ue_golomb(gb);
        int width  = 16 * sps->mb_width;
        int height = 16 * sps->mb_height * (2 - sps->frame_mbs_only_flag);

    } else {
    }
    return 1;
fail:
    free(sps);
    return -1;
}
#endif
 int ff_h264_decode_sps( const uint8_t *sps , int size, Screen_Info *info){

	GetBitContext *gb = (GetBitContext *)malloc(sizeof(GetBitContext)*1);
	gb->buffer = (uint8_t *)malloc(size);
	memset(gb->buffer, 0x0, size);
	memcpy(gb->buffer, sps, size);
	gb->buffer_end = gb->buffer+size;
	gb->index = 0;
	gb->size_in_bits = size * 8;
	gb->size_in_bits_plus8 = size * 8 + 8;
	int inreturn= ff_h264_decode_seq_parameter_set(gb, info);
	free(gb);
	gb=NULL;
printf("Width: %d, Height: %d\n", info->width, info->height);
return inreturn;
}
#if 0
int main() {
	//uint8_t sps[] = {0x64, 0x00, 0x29, 0xad, 0x84, 0x05, 0x45, 0x62, 0xb8, 0xac, 0x54, 0x74, 0x20, 0x2a, 0x2b, 0x15, 0xc5, 0x62, 0xa3, 0xa1, 0x01, 0x51, 0x58, 0xae, 0x2b, 0x15, 0x1d, 0x08, 0x0a, 0x8a, 0xc5, 0x71, 0x58, 0xa8, 0xe8, 0x40, 0x54, 0x56, 0x2b, 0x8a, 0xc5, 0x47, 0x42, 0x02, 0xa2, 0xb1, 0x5c, 0x56, 0x2a, 0x3a, 0x10, 0x24, 0x85, 0x21, 0x39, 0x3c, 0x9f, 0x27, 0xe4, 0xfe, 0x4f, 0xc9, 0xf2, 0x79, 0xb9, 0xb3, 0x4d, 0x08, 0x12, 0x42, 0x90, 0x9c, 0x9e, 0x4f, 0x93, 0xf2, 0x7f, 0x27, 0xe4, 0xf9, 0x3c, 0xdc, 0xd9, 0xa6, 0xb4, 0x02, 0x80, 0x2d, 0xd2, 0xa4, 0x00, 0x00, 0x03, 0x01, 0xe0, 0x00, 0x00, 0x70, 0x81, 0x81, 0x00, 0x01, 0xe8, 0x48, 0x00, 0x14, 0x99, 0x6f, 0x7b, 0xe1, 0x78, 0x44, 0x23, 0x50};
        uint8_t sps[] =    {0x64,0x00,0x20,0xac,0xb2,0x00,0xa0,0x0b,0x76,0x02,0x20,0x00,0x00,0x03,0x00,0x20,0x00,0x00,0x0c,0x81,0xe3,0x06,0x49};
Screen_Info info;
	int size = sizeof(sps)/sizeof(sps[0]);
ff_h264_decode_sps(sps,size, &info);
// uint8_t sps[] =   { 0x42, 0x00, 0x28, 0xE9, 0x00, 0xA0, 0x0B, 0x77, 0xFE, 0x00, 0x02, 0x00, 0x03, 0xC4, 0x80, 0x00, 0x00, 0x03, 0x00, 0x80, 0x00, 0x00, 0x1A, 0x4D, 0x88, 0x10, 0x94};
		//	uint8_t sps[] = {0x00 ,0x1f,0xe9,0x02,0xc1,0x2c,0x80};//no
        //     uint8_t sps[] ={0x42, 0x00, 0x0a, 0xf8, 0x41, 0xa2};//128*96
  //     uint8_t sps[] ={0x64,0x00,0x2A,0xAD,0x84,0x01,0x0C,0x20,0x08,0x61,0x00,0x43,0x08,0x02,0x18,0x40,0x10,0xC2,0x00,0x84,0x2B,0x50,0x3C,0x01,0x13,0xF2,0xC2,0x00,0x00,0x03,0x00,0x02,0x00,0x00,0x03,0x00,0x79,0x08};
    //uint8_t sps[] = {0x42,0x00 ,0x1f,0xe9,0x02,0xc1,0x2c,0x80};

//	GetBitContext *gb = (GetBitContext *)malloc(sizeof(GetBitContext));
//
//	gb->buffer = (uint8_t *)malloc(size);
//	memset(gb->buffer, 0x0, size);
//	memcpy(gb->buffer, sps, size);
//	gb->buffer_end = gb->buffer+size;
//	gb->index = 0;
//	gb->size_in_bits = size * 8;
//	gb->size_in_bits_plus8 = size * 8 + 8;
//	ff_h264_decode_seq_parameter_set(gb, &info);
printf("Width: %d, Height: %d\n", info.width, info.height);
	return 0;
}
#endif
