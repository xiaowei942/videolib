#include "transfer.h"
#include <netinet/in.h>

//#define TRANSFER_DEBUG

//线程退出标志，不可用Transfer的成员来控制

int offset2 = 0;
bool isExit = false;
nalu_package *nal_pkg = NULL;

void *start_receive_func(void *arg) {
	Transfer *cls = (Transfer *)arg;
	cls->receiveThread();
}

void *start_process_func(void *arg) {
	Transfer *cls = (Transfer *)arg;
	//cls->processThread();
}

void Transfer::Exit() {
	isExit = true;
}

Transfer::Transfer(int width, int height) : isReceive(false), isProcess(false), video_width(width), video_height(height), gotSpsPps(false), local_data_socket_fd(-1) {
	LOGI("Enter Transfer");
	package_queue = new CircleQueue<data_package *>(QUEUE_SIZE, "DATA_QUEUE", true);
	if(!package_queue) {
		LOGE("Cannot Create CircleQueue");
	}

	frame_queue = new CircleQueue<nalu_package *>(QUEUE_SIZE, "NALU_QUEUE", true);
	if(!frame_queue) {
		LOGE("Cannot Create CircleQueue");
	}

#if 0
	for(int i=0; i<QUEUE_SIZE; i++) {
		nalu_package *nal_pkg = (nalu_package *)malloc(sizeof(nalu_package));
		if(!nal_pkg) {
			LOGE("Malloc Frame Queue Element Error");
			exit(-1);
		}

		nal_pkg->size = QUEUE_SIZE;
		nal_pkg->pos = 0;
		nal_pkg->nalu = (uint8_t *)malloc(FRAME_SIZE);
		if(!nal_pkg->nalu) {
			LOGE("Malloc Frame Queue Buffer Error");
			exit(-1);
		}
		frame_queue->enQueue(nal_pkg);
	}
#endif

	int error = pthread_create(&receive_handle, NULL, &start_receive_func, (void *)this);
	if(error) {
		LOGE("Cannot Create Thread");
	}

	error = pthread_create(&process_handle, NULL, &start_process_func, (void *)this);
	if(error) {
		LOGE("Cannot Create Thread");
	}
	LOGI("Leave Transfer");
}

Transfer::~Transfer() {
	LOGI("Enter ~Transfer");
	Exit();
	if(package_queue) {
		//遍历列表，以此释放内部指针
		delete package_queue;
		package_queue = NULL;
	}

	if(frame_queue) {
		//遍历列表，以此释放内部指针
		delete frame_queue;
		frame_queue = NULL;
	}

	unInitSockets();
	LOGI("Leave ~Transfer");
}

int Transfer::initDataSocket(const char *server_ip, const char *local_ip, int local_port) {
	LOGI("Now initDataSocket: server_ip: %s, local_ip: %s, local_port: %d", server_ip, local_ip, local_port);
	if(local_data_socket_fd != -1) {
		LOGE("Socket Already Created");
		return -1;
	}

	/* 创建data socket */
	local_data_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(local_data_socket_fd < 0)
	{
		LOGE("Create Socket Failed");
		return -1;
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("192.168.42.202");
	server_addr.sin_port = htons(8000);

	if(local_ip) {
		bzero(&local_addr, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(local_ip);
		local_addr.sin_port = htons(local_port);

		/* 绑定套接口 */
		if(-1 == (bind(local_data_socket_fd,(struct sockaddr*)&local_addr,sizeof(local_addr))))
		{
			LOGE("Client Bind Failed:");
			return -1;
		}
	}

	startReceive();
	LOGI("local_addr bind success");
	return 0;
}

int Transfer::initSocket(const char *server_ip, const char *local_ip, const int local_port) {
	int ret;
	LOGI("Now initSocket: server_ip: %s, local_ip: %s, local_port: %d", server_ip, local_ip, local_port);
	switch(local_port) {
		case CONTROL_PORT:
			break;
		case DATA_PORT:
			ret = initDataSocket(server_ip, local_ip, local_port);
			break;
		case MANAGE_PORT:
			break;
		case TRANSFER_PORT:
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
}

int Transfer::unInitSocket(int port) {
	LOGI("Enter unInitSocket");
	int ret;
	switch (port) {
		case CONTROL_PORT:
			ret = close(local_control_socket_fd);
			break;
		case DATA_PORT:
			ret = close(local_data_socket_fd);
			break;
		case MANAGE_PORT:
			break;
		case TRANSFER_PORT:
			break;
		default:
			ret = -1;
			break;
	}
	LOGI("Leave unInitSocket");
	return ret;
}

void Transfer::unInitSockets() {
	LOGI("Enter unInitSockets");
	if(local_data_socket_fd >= 0) {
		close(local_data_socket_fd);
	}
	if(local_control_socket_fd >= 0) {
		close(local_control_socket_fd);
	}
	LOGI("Leave unInitSockets");
}

void* Transfer::receiveThread() {
	LOGI("Enter receive thread");
	int data_len;
	char buffer[BUFFER_SIZE];

#if 0
	while(!isReceive && !isExit) {
#ifdef TRANSFER_DEBUG
		LOGI("Now receive sleep");
		usleep(3000000);
#else
		usleep(100000);
#endif
	}
#endif
	isReceive = true;
	/* 数据传输 */
	while(isReceive && !isExit)
	{
		data_len = 0;
		socklen_t server_addr_length = sizeof(server_addr);
		memset(buffer, 0x0, BUFFER_SIZE);

//		LOGI("Now receive data");

		/* 接收数据 */
		int rcv_len = recvfrom(local_data_socket_fd, buffer, BUFFER_SIZE, 0,(struct sockaddr*)&server_addr, &server_addr_length);
		if(rcv_len == -1)
		{
			LOGE("Receive Data Failed:");
			continue;
			//exit(1);
		}

#if 0
		LOGI("******************* Receive ******************");
		char *log_buf = (char *)malloc(rcv_len * 8);
		memset(log_buf, 0x0, rcv_len*8);
		for(int i=0; i<rcv_len; i++) {
			sprintf(&log_buf[i*3], "%02x ", buffer[i] & 0xff);
		}
		LOGI("Receive:");
		LOGI("%s", log_buf);
		printf("******************* Receive End******************");
#endif

		data_len = get_valid_data_length(buffer);
		if(data_len) {
#ifdef VERIFY
			if(!verify(buffer, data_len)) {
#endif
				data_package *pkg = (data_package *)malloc(sizeof(data_package));
				assert(pkg);
				if(!pkg) {
					LOGE("Cannot malloc data_package");
				}

				int result = parse_data_package(buffer, pkg);
				if(result) {
					LOGE("Parse data_package failed");
				}
//				LOGI("data_package seq: %02x", pkg->seq);
//				LOGI("Receiver enQueue package %p", pkg);

#if 1
				if(!gotWidthHeight) {
					if(!gotSpsPps) {
						LOGI("Now parse package -> has_sps_pps");
						int ret = has_sps_pps(pkg, sps, sps_size, Pps, pps_size, spsPps);
						if(ret == STATUS_PARSE_SUCCESS) {
							LOGI("Now Got Sps Pps !");
							gotSpsPps = true;
						} else {
							LOGI("Cannot get Sps Pps");
							continue;
						}
					}

					LOGI("ff_h264 : %d ",pkg->nal_size);
					Screen_Info info;
					LOGI("Now Get Width and Height");
					int spssize = sizeof(sps)/sizeof(sps[0]);
					if(ff_h264_decode_sps(&sps[4],spssize-5, &info)){
						LOGI("Width: %d, Height: %d\n", info.width, info.height);
						gotWidthHeight = true;
					} else {
						continue;
					}
				}

#endif

				if(makeFrame(pkg) != NULL) {
					nal_pkg->frame_num++;
					frame_queue->enQueue(nal_pkg);
				}

				if(pkg) {
					if(pkg->nal_data) {
						LOGI("free1");
						free(pkg->nal_data);
						LOGI("free+++++++=");
						pkg->nal_data = NULL;
					} 
		//			LOGI("free %s", __LINE__);
					LOGI("free+++++ ");
					free(pkg);
					pkg = NULL;
				}
#ifdef VERIFY
			}
#endif
		}

		//usleep(1000);
	}

	LOGI("Exit Receive Thread");
}

data_package* Transfer::getDataPackage() {
	data_package *pkg = NULL;
	if(package_queue->deQueue(pkg)) {
		//LOGI("pkg: %p", pkg);
		return pkg;
	}
	return NULL;
}

int Transfer::getSps(uint8_t *buf) {
	memcpy(buf, sps, sps_size);
	return sps_size;
}

int Transfer::getPps(uint8_t *buf) {
	memcpy(buf, Pps, pps_size);
	return pps_size;
}

nalu_package *Transfer::getFrame() {
	nalu_package *pkg = NULL;
	frame_queue->deQueue(pkg);
	if(pkg) {
		return pkg;
	}
	return NULL;
}


bool Transfer::isPrepared() {
	return gotWidthHeight;
}

nalu_package *Transfer::makeFrame(data_package *pkg) {

	LOGI("Now makeFrame");

	char *queue_buffer;
	int slice_type = get_data_package_slice_ident_type(pkg);
	switch(slice_type) {
		case SLICE_TYPE_INTER: //中间分片
			LOGI("FIND A INTERNAL SLICE: %02x", pkg->seq);

			assert(nal_pkg);
			if(!nal_pkg) {
				return NULL;
			}

			offset2 = nal_pkg->size;
			nal_pkg->seq = pkg->seq;
			LOGI("lbg33333 %d",nal_pkg->size);
			//LOGI("Slice seq: %02x", nal_pkg->seq);
			nal_pkg->nalu = (uint8_t *)realloc(nal_pkg->nalu, offset2 + pkg->nal_size);
			LOGI("lbg44444");
			memcpy(nal_pkg->nalu+offset2, pkg->nal_data, pkg->nal_size);
			nal_pkg->size += pkg->nal_size;
			break;
		case SLICE_TYPE_FIRST: //分片开始
			LOGI("FIND A START SLICE: %02x", pkg->seq);

			//如果没有发现分片结束
			if(nal_pkg) {
				LOGI("NO END SLICE, NOW QUEUE, SIZE: %d", nal_pkg->size);
				return nal_pkg;
				nal_pkg = NULL;
			}

			nal_pkg = (nalu_package *)malloc(sizeof(nalu_package));
			if(!nal_pkg) {
				LOGE("No more memory");
				return NULL;
			}

			nal_pkg->seq = pkg->seq;
			nal_pkg->size = pkg->nal_size;
			LOGI("Slice seq: %02x %d", nal_pkg->seq,nal_pkg->size);
			nal_pkg->nalu = (uint8_t *)malloc(nal_pkg->size);
			if(!nal_pkg->nalu) {
				LOGE("No more memory");
				return NULL;
			}

			memset(nal_pkg->nalu, 0x0, nal_pkg->size);
			memcpy(nal_pkg->nalu, pkg->nal_data, pkg->nal_size);
			LOGI("makeFrame size: [pkg] %d [nal_pkg] %d ",pkg->nal_size,nal_pkg->size);
			break;
		case SLICE_TYPE_LAST: //分片结束
			LOGI("FIND A END SLICE: %02x", pkg->seq);

			if(!nal_pkg) {
				return NULL;
			}

			assert(nal_pkg);
			offset2 = nal_pkg->size;
			nal_pkg->seq = pkg->seq;
			//				LOGI("Slice seq: %02x", nal_pkg->seq);
			nal_pkg->nalu = (uint8_t *)realloc(nal_pkg->nalu, offset2 + pkg->nal_size);
			memcpy(nal_pkg->nalu+offset2, pkg->nal_data, pkg->nal_size);
			nal_pkg->size += pkg->nal_size;

			LOGI("FIND END SLICE, NOW QUEUE, SIZE: %d", nal_pkg->size);
#if 0
			LOGI("******************* QUEUE ******************");
			queue_buffer = (char *)malloc(nal_pkg->size * 3);
			memset(queue_buffer, 0x0, nal_pkg->size*3);
			for(int i=0; i<nal_pkg->size; i++) {
				sprintf(&queue_buffer[i*3], "%02x ", nal_pkg->nalu[i] & 0xff);
			}
			LOGI("%s", queue_buffer);
			free(queue_buffer);
			LOGI("******************* QUEUE END******************");
#endif
			return nal_pkg;

			break;
		case SLICE_TYPE_NONE:
#if 1
			LOGI("FIND A WHOLE FRAME: %d", nal_pkg->size);
			//如果没有发现分片结束
			if(nal_pkg) {
				LOGI("NO END SLICE, NOW QUEUE, SIZE: %d", nal_pkg->size);
				return nal_pkg;
			}

			nal_pkg = (nalu_package *)malloc(sizeof(nalu_package));
			if(!nal_pkg) {
				LOGE("No more memory");
				return NULL;
			}

			nal_pkg->seq = pkg->seq;
			LOGI("Slice seq: %02x", nal_pkg->seq);
			nal_pkg->size = pkg->nal_size;
			nal_pkg->nalu = (uint8_t *)malloc(nal_pkg->size);
			if(!nal_pkg->nalu) {
				LOGE("No more memory");
				return NULL;
			}

			memset(nal_pkg->nalu, 0x0, nal_pkg->size);
			memcpy(nal_pkg->nalu, pkg->nal_data, pkg->nal_size);
			return nal_pkg;
#endif
			break;
		default:
			LOGI("Not A Valid Slice");

			if(pkg) {
				if(pkg->nal_data) {
					free(pkg->nal_data);
					pkg->nal_data = NULL;
				}
				free(pkg);
				pkg = NULL;
			}
			break;
	}
}

