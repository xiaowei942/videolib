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

#define BUFFER_SIZE 1024
#define QUEUE_SIZE 32

class Transfer {
public:
	Transfer(int width, int height);
	~Transfer();
	int initSocket(const char *server_ip, const char *local_ip, int local_port);
	int initDataSocket(const char *server_ip, const char *local_ip, int local_port);
	void unInitSocket();
	int startReceive() { isReceive = true; }
	int stopReceive() { isReceive = false; }
	void* receiveThread();

	int startProcess() { isProcess = true; }
	int stopProcess() { isProcess = false; }
	void* processThread();

private:
	data_package* getDataPackage();

	CircleQueue<data_package *> *package_queue;
	CircleQueue<uint8_t *> *frame_queue;
	/* 服务端地址 */
	struct sockaddr_in server_addr;
	/* 本机地址 */
	struct sockaddr_in local_addr;
	/* socket */
	int local_data_socket_fd; //data
	int local_control_socket_fd; //control
	bool isReceive;
	bool isProcess;
	pthread_t *handle;

	int video_width;
	int video_height;
};
#endif
