#include <string.h>
#include <assert.h>
#include "mp4_writer.h"

#include <android/log.h>

#define LOG_TAG "MP4_WRITER"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

Mp4_Writer::Mp4_Writer(int width, int height):
			mp4_fps(25),
			isFirstSPS(false),
			isFirstPPS(false),
			video_frame_number(1),
			extractor(NULL),
			mp4_file(NULL),
			video_track_id(MP4_INVALID_TRACK_ID),
			video_time_scale(90000),
			video_start_time_stamp(0),
			video_payload_data(NULL),
			video_payload_data_buffer_size(65535),
			video_temp_buffer(NULL),
			video_temp_buffer_size(65535*10),
			video_h264_sps(NULL),
			video_h264_sps_size(32),
			video_h264_pps(NULL),
			video_h264_pps_size(32)
{
	video_width = width;
	video_height = height;
	memset(mp4_filename, '\0', 256);
	//video_h264_sps = (unsigned char *)malloc(video_h264_sps_size*sizeof(unsigned char));
	//video_h264_pps = (unsigned char *)malloc(video_h264_pps_size*sizeof(unsigned char));
	//video_payload_data = (unsigned char *)malloc(video_payload_data_buffer_size*sizeof(unsigned char));
}

Mp4_Writer::~Mp4_Writer() {

}

void Mp4_Writer::SetMp4Fps(int fps) {
	mp4_fps = fps;
}

void Mp4_Writer::SetMp4FileName(const char *file_name) {
	assert(file_name != NULL);
	strcpy(mp4_filename, file_name);
}

int Mp4_Writer::DoStartRecord() {
	mp4_file = MP4CreateEx(mp4_filename, 0, 1, 1, 0, 0, 0, 0);
	if(mp4_file == MP4_INVALID_FILE_HANDLE) {
		return -1;
	}

	if(MP4SetTimeScale(mp4_file, video_time_scale) != 1) {
		return -1;
	}
	return 0;
}

int Mp4_Writer::DoStopRecord() {
	if(video_payload_data) {
		free(video_payload_data);
		video_payload_data_buffer_size = 0;
	}	

	MP4Close(mp4_file);
	mp4_file = NULL;
}

void Mp4_Writer::WriteEncodedVideoFrame(const unsigned char *payload_data, unsigned int payload_size, unsigned int time_stamp) {
	bool foundIDR = false;
	//Write 1st frame
	if(video_frame_number == 1) {
		unsigned char *data_header = const_cast<unsigned char *>(payload_data);
		unsigned int header_len = payload_size;

		unsigned char nalu_type = 0;
		if( (payload_data[0] == 0x0) && (payload_data[1] == 0x0) && (payload_data[2] == 0x0) && (payload_data[3] == 0x01) ) {
			nalu_type = payload_data[4] & 0x1F;
			data_header = const_cast <unsigned char *>(payload_data) + 4;
			header_len = payload_size - 4;
		} else {
			nalu_type = (*(payload_data)) & 0x1F;
		}
		
		if(nalu_type != 0x07) {
			return;
		}
		

		unsigned char avc_profile = *(data_header + 1);
		unsigned char profile_compat = *(data_header + 2);
		unsigned char avc_level = *(data_header + 3);

		video_track_id = MP4AddH264VideoTrack(mp4_file, video_time_scale, video_time_scale/mp4_fps, video_width, video_height, avc_profile, profile_compat, avc_level, 3);

		if(video_track_id == MP4_INVALID_TRACK_ID) {
			return;
		}

		MP4SetVideoProfileLevel(mp4_file, 0x7F);

	}

	video_payload_data = (unsigned char *)malloc(payload_size*sizeof(unsigned char) + 2048);
	video_payload_data_buffer_size = payload_size*sizeof(unsigned char) + 2048;

	const unsigned char *data = payload_data;
	const unsigned char *head = NULL;
	unsigned int nalu_len = 0;
	unsigned int write_len = 0, copy_len = 0;
	unsigned char *write_pos = video_payload_data;

	for(int i=0; i<payload_size; i++) {
		if( (i>2) && (data[i-2] == 0x0) && (data[i-1] == 0x0) && (data[i] == 0x01) ) {
			if( ((i-2)==0x0) || ((i-2) == 1) ) {
				head = data + i + 1;
				nalu_len = 0;
				continue;
			}

			if(data[i-3] == 0) {
				copy_len = nalu_len -3;
			} else {
				copy_len = nalu_len -2;
			}

			write_pos[0] = (copy_len >> 24) & 0xff;
			write_pos[1] = (copy_len >> 16) & 0xff;
			write_pos[2] = (copy_len >>  8) & 0xff;
			write_pos[3] = (copy_len ) & 0xff;

			write_pos += 4;
			write_len += 4;

			memcpy(write_pos, head, copy_len);
			write_pos += copy_len;
			write_len += copy_len;

			if( ((*head & 0x1f) == 0x07) || ((*head & 0x1f) == 0x08) || ((*head & 0x1f) == 0x06) ) {
				WriteH264Frame(video_payload_data, write_len, time_stamp);//time_stamp/100);
				write_len = 0;
				write_pos = video_payload_data;
			}

			nalu_len = 0;
			head = data + i + 1;
			continue;

		}

		nalu_len++;
	}

	if(nalu_len > 0) {
		write_pos[0] = (nalu_len >> 24) & 0xff;
		write_pos[1] = (nalu_len >> 16) & 0xff;
		write_pos[2] = (nalu_len >>  8) & 0xff;
		write_pos[3] = (nalu_len) & 0xff;

		write_pos += 4;
		write_len += 4;
		memcpy(write_pos, head, nalu_len);
		write_len += nalu_len;
	}

	if(write_len>0) {
		WriteH264Frame(video_payload_data, write_len, 3600);
	}
	
	if(video_payload_data) {
		free(video_payload_data);
		video_payload_data = NULL;
	}
	video_frame_number++;
}


void Mp4_Writer::WriteH264Frame(unsigned char *nalus, unsigned int nalus_len, unsigned int duration) {
	bool isIFrame = false;
	unsigned int rend_offset = 0;

	unsigned char *dst = video_temp_buffer;
	unsigned int nalu_len = nalus_len;
	unsigned char nalu_type;

	bool write_it = false;
	unsigned int len_written = 0;
	nalu_type = nalus[4] & 0x1f;

	switch (nalu_type)
	{
		case 6:
			break;
		case 7:
			if( (nalu_len != video_h264_sps_size) || ((video_h264_sps != NULL) && 
				(memcmp(video_h264_sps, nalus, video_h264_sps_size) != 0)) ) {
				video_h264_sps_size = nalu_len - 4;

				video_h264_sps = (unsigned char *)malloc(video_h264_sps_size);
				memcpy(video_h264_sps, nalus+4, video_h264_sps_size);
				if(!isFirstSPS) {
					LOGI("WEI--> SET SPS PARAM");
					MP4AddH264SequenceParameterSet(mp4_file, 
										video_track_id,
									       	video_h264_sps, 
										video_h264_sps_size);
					isFirstSPS = true;
					LOGI("WEI--> SET SPS PARAM OK");
				}
				if(video_h264_sps) {
					free(video_h264_sps);
					video_h264_sps = NULL;
				}
			}
			isIFrame = true;
			write_it = true;
			break;
		case 8:
			if( (nalu_len != video_h264_pps_size) || ((video_h264_pps != NULL) && 
				(memcmp(video_h264_pps, nalus, video_h264_pps_size) != 0)) ) {
				video_h264_pps_size = nalu_len - 4;

				video_h264_pps = (unsigned char *)malloc(video_h264_pps_size);
				memcpy(video_h264_pps, nalus+4, video_h264_pps_size);
				if(!isFirstPPS) {
					LOGI("WEI--> SET SPS PARAM");
					MP4AddH264PictureParameterSet(mp4_file, 
										video_track_id,
									       	video_h264_pps,
									       	video_h264_pps_size);
					isFirstPPS = true;
					LOGI("WEI--> SET PPS PARAM OK");
				}
				if(video_h264_pps) {
					free(video_h264_pps);
					video_h264_pps = NULL;
				}
			}
			write_it = true;
		break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			write_it = true;
			break;
		case 12:
			write_it = true;
		default:
			write_it = true;
			break;
	}

	if(write_it) {
		bool res = MP4WriteSample(mp4_file, video_track_id, nalus, nalus_len, MP4_INVALID_DURATION, rend_offset, isIFrame);
	}
}
