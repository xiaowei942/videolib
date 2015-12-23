#ifndef __TRANSFER_H__
#define __TRANSFER_H__

/* Must include in android ndk env */
#include <sys/endian.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <CircleQueue.h>
#include "transfer_protocol.h"
#include "tools/sps_resolution.h"
#define FRAME_SIZE 4*1024*1024
#define BUFFER_SIZE 1024
#define QUEUE_SIZE 32

#include <android/log.h>
#define TRANSFER_LOG_TAG "TRANSFER"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TRANSFER_LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TRANSFER_LOG_TAG,__VA_ARGS__)

class Transfer {
public:
	Transfer(int width, int height);
	~Transfer();
	int initSocket(const char *server_ip, const char *local_ip, int local_port);
	int unInitSocket(int port);
	int initDataSocket(const char *server_ip, const char *local_ip, int local_port);
	void unInitSocket();
	int startReceive() { isReceive = true; }
	int stopReceive() { isReceive = false; }
	void* receiveThread();

	void Exit();
	int startProcess() { isProcess = true; }
	int stopProcess() { isProcess = false; }
	void* processThread();

	int getSps(uint8_t *buf);
	int getPps(uint8_t *buf);

	uint8_t **get_frame(uint32_t &payload_size);
	nalu_package *getFrame();
	nalu_package *makeFrame(data_package *pkg);
	bool isPrepared();
private:
	data_package* getDataPackage();
	void unInitSockets();

	CircleQueue<data_package *> *package_queue;
	CircleQueue<nalu_package *> *frame_queue;
	/* 服务端地址 */
	struct sockaddr_in server_addr;
	/* 本机地址 */
	struct sockaddr_in local_addr;
	/* socket */
	int local_data_socket_fd; //data
	int local_control_socket_fd; //control

	bool isReceive;
	bool isProcess;
	pthread_t receive_handle;
	pthread_t process_handle;

	int video_width;
	int video_height;

	uint8_t sps[512];
	uint8_t Pps[128];
	uint8_t spsPps[1024];

	int sps_size;
	int pps_size;
	int sps_pps_size;

	bool gotSpsPps;
	bool gotWidthHeight;
};
#endif
