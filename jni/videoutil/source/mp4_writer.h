#ifndef __MP4_WRITER_H__
#define __MP4_WRITER_H__
#include <iostream>
#include <malloc.h>
#include <mp4v2/mp4v2.h>
#include "mp4_extractor.h"
using namespace std;

class Mp4_Writer {
	int mp4_fps;
	bool isFirstSPS;
	bool isFirstPPS;
	int video_width;
	int video_height;
	unsigned int video_frame_number;
	H264_Extractor *extractor;
	MP4FileHandle mp4_file;
	MP4TrackId video_track_id;
	int video_time_scale;
	unsigned int video_start_time_stamp; 
	unsigned char *video_payload_data;
	unsigned int video_payload_data_buffer_size;
	unsigned char *video_temp_buffer;
	unsigned int video_temp_buffer_size;
	unsigned char *video_h264_sps;
	unsigned int video_h264_sps_size;
	unsigned char *video_h264_pps;
	unsigned int video_h264_pps_size;
	char mp4_filename[256];
public:
	Mp4_Writer(int width, int height);
	~Mp4_Writer();

	void SetMp4Fps(int fps);
	void SetMp4FileName(const char *file_name);
	int DoStartRecord();
	int DoStopRecord();

	void WriteEncodedVideoFrame(const unsigned char *payload_data, unsigned int payload_size, unsigned int time_stamp);
	void WriteH264Frame(unsigned char *nalus, unsigned int nalus_len, unsigned int duration);
};
#endif
