#include "transfer.h"
#include <netinet/in.h>

//#define TRANSFER_DEBUG
//#define transfer_free
//线程退出标志，不可用Transfer的成员来控制

int offset2 = 0;
bool isExit = false;
nalu_package *nal_pkg = NULL;

void *start_receive_func(void *arg) {
	Transfer *cls = (Transfer *)arg;
	//开启接收线程
	cls->receiveThread();
}

void Transfer::Exit() {
	isExit = true;
}

Transfer::Transfer(int width, int height) : isReceive(false), isProcess(false), video_width(width), video_height(height), gotSpsPps(false), gotWidthHeight(false), local_data_socket_fd(-1) {
	LOGI("Enter Transfer");
//	package_queue = new CircleQueue<data_package *>(QUEUE_SIZE, "DATA_QUEUE", true);
//	if(!package_queue) {
//		LOGE("Cannot Create CircleQueue");
//	}

	frame_queue = new CircleQueue<nalu_package *>(QUEUE_SIZE, "NALU_QUEUE", false);
	if(!frame_queue) {
		LOGE("Cannot Create CircleQueue");
	}

	int error = pthread_create(&receive_handle, NULL, &start_receive_func, (void *)this);
	if(error) {
		LOGE("Cannot Create Thread");
	}
	LOGI("Leave Transfer");
}

Transfer::~Transfer() {
	LOGI("Enter ~Transfer");
	Exit();
//	if(package_queue) {
//		//遍历列表，以此释放内部指针
//		delete package_queue;
//		package_queue = NULL;
//	}

	gotWidthHeight = false;

	LOGI("Now free circle queue elements");
	if(frame_queue) {
		for(int i=0; i<frame_queue->getCapacity(); i++){
			nalu_package *mal_pkg = NULL;
			frame_queue->deQueuebyNum(mal_pkg,i);
			if(mal_pkg) {
				if(!mal_pkg->size){
					if(mal_pkg->nalu) {
						free(mal_pkg->nalu);
						mal_pkg->nalu = NULL;
					}
				}
				free(mal_pkg);
				mal_pkg=NULL;
			}
		}
		//遍历列表，以此释放内部指针
		delete frame_queue;
		frame_queue = NULL;
	}

	LOGI("Now freee circle queue end");
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
	LOGI("Leave unInitSocket: %d", ret);
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
		usleep(100000);
#else
		usleep(100000);
#endif
	}
#endif
	isReceive = true;
	isExit = false;
	/* 数据传输 */
	while(isReceive && !isExit)
	{
		data_len = 0;
		socklen_t server_addr_length = sizeof(server_addr);
		memset(buffer, 0x0, BUFFER_SIZE);

		if(isExit) {
			exit(0);
		}

		/* 接收数据 */
		int rcv_len = recvfrom(local_data_socket_fd, buffer, BUFFER_SIZE, MSG_DONTWAIT, (struct sockaddr*)&server_addr, &server_addr_length);
		if(rcv_len == -1)
		{
#if 0
			if(isExit) {
				LOGE("Receive Data Failed - true!");
			} else {
				LOGE("Receive Data Failed - false!");
			}
#endif
			continue;
			//exit(1);
		} else {

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
#ifdef TRANSFER_DEBUG
					LOGI("raw package: size: %d(%d)  seq: %02x", pkg->package_length, pkg->nal_size, pkg->seq);
#endif
					if(!gotWidthHeight) {
						if(!gotSpsPps) {
#ifdef TRANSFER_DEBUG
							LOGI("Now parse package -> has_sps_pps");
#endif
							int ret = has_sps_pps(pkg, sps, sps_size, Pps, pps_size, spsPps);
							if(ret == STATUS_PARSE_SUCCESS) {
								LOGI("Now Got Sps Pps !");
								gotSpsPps = true;
							} else {
#ifdef TRANSFER_DEBUG
								LOGI("Cannot get Sps Pps");
#endif

								goto pkgfree;
								continue;
							}
						}

						Screen_Info info;
						LOGI("Now Get Width and Height");
						int spssize = sizeof(sps)/sizeof(sps[0]);
						if(ff_h264_decode_sps(&sps[5],spssize-5, &info)){
							video_width=info.width;
							video_height=info.height;
							LOGI("Width: %d, Height: %d\n", info.width, info.height);
							gotWidthHeight = true;
						} else {
							goto pkgfree;
							continue;
						}
					}
					if(makeFrame(pkg) != NULL) {
#ifdef TRANSFER_DEBUG
						LOGI("makeFrame size: %d frame_num:%d", nal_pkg->size,nal_pkg->frame_num);
#endif
						//					nal_pkg->frame_num++;

						nalu_package *current_nal_pkg = NULL;
						//					frame_queue->deQueueCurrent(current_nal_pkg);
						//					if(current_nal_pkg){
						//					if(current_nal_pkg->nalu) {
						//									free(current_nal_pkg->nalu);
						//									current_nal_pkg->nalu = NULL;
						//								}
						//
						//						free(current_nal_pkg);
						//					}

						frame_queue->enQueue(nal_pkg);
						nal_pkg = NULL;
					}

					goto pkgfree;
#ifdef VERIFY
				}
#endif
pkgfree:
				if(pkg) {
					if(pkg->nal_data) {
						free(pkg->nal_data);
						pkg->nal_data = NULL;
					}
#ifdef transfer_free
					LOGI("transfer_free 111");
#endif
					free(pkg);
				}
			}
		}
		//usleep(1000);
	}

	LOGI("Exit Receive Thread");
}

//data_package* Transfer::getDataPackage() {
//	data_package *pkg = NULL;
//	if(package_queue->deQueue(pkg)) {
//		//LOGI("pkg: %p", pkg);
//		return pkg;
//	}
//	return NULL;
//}

int Transfer::getSps(uint8_t *buf) {
	memcpy(buf, sps, sps_size);
	return sps_size;
}

int Transfer::getPps(uint8_t *buf) {
	memcpy(buf, Pps, pps_size);
	return pps_size;
}

nalu_package *Transfer::getFrame() {
	if(isExit) {
		return NULL;
	}

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
	nalu_package *ret = NULL;
	char *queue_buffer;
	int slice_type = get_data_package_slice_ident_type(pkg);
	switch(slice_type) {
		case SLICE_TYPE_INTER: //中间分片
#ifdef TRANSFER_DEBUG
			LOGI("FIND A INTERNAL SLICE: %02x", pkg->seq);
#endif
			assert(nal_pkg);
			if(!nal_pkg) {
				return NULL;
			}

			offset2 = nal_pkg->size;
			nal_pkg->seq = pkg->seq;
			nal_pkg->nalu = (uint8_t *)realloc(nal_pkg->nalu, offset2 + pkg->nal_size);
			memcpy(nal_pkg->nalu+offset2, pkg->nal_data, pkg->nal_size);
			nal_pkg->size += pkg->nal_size;
#ifdef TRANSFER_DEBUG
			LOGI("SLICE: size: %d  seq: %02x", nal_pkg->size, nal_pkg->seq);
#endif
			return NULL;
			break;
		case SLICE_TYPE_FIRST: //分片开始
			//如果没有发现分片结束
			if(nal_pkg) {
#ifdef TRANSFER_DEBUG
				LOGI("NO END SLICE, NOW QUEUE, SIZE: %d", nal_pkg->size);
#endif
				return nal_pkg;
			}

			nal_pkg = (nalu_package *)malloc(sizeof(nalu_package));
			if(!nal_pkg) {
				LOGE("No more memory");
				return NULL;
			}

			nal_pkg->seq = pkg->seq;
			nal_pkg->size = pkg->nal_size;
#ifdef TRANSFER_DEBUG
			LOGI("FIND A START SLICE: size: %d  seq: %02x", nal_pkg->size, nal_pkg->seq);
#endif
			nal_pkg->nalu = (uint8_t *)malloc(nal_pkg->size);
			if(!nal_pkg->nalu) {
				LOGE("No more memory");
				return NULL;
			}

			memset(nal_pkg->nalu, 0x0, nal_pkg->size);
			memcpy(nal_pkg->nalu, pkg->nal_data, pkg->nal_size);
			return NULL;
			break;
		case SLICE_TYPE_LAST: //分片结束

			if(!nal_pkg) {
				return NULL;
			}

			assert(nal_pkg);
#ifdef TRANSFER_DEBUG
			LOGI("FIND A END SLICE: %02x SIZE: %d", pkg->seq, nal_pkg->size);
#endif
			offset2 = nal_pkg->size;
			nal_pkg->seq = pkg->seq;
#ifdef TRANSFER_DEBUG
			LOGI("SLICE: size: %d  seq: %02x", nal_pkg->size, nal_pkg->seq);
#endif;
			nal_pkg->nalu = (uint8_t *)realloc(nal_pkg->nalu, offset2 + pkg->nal_size);
			memcpy(nal_pkg->nalu+offset2, pkg->nal_data, pkg->nal_size);
			nal_pkg->size += pkg->nal_size;
#if 0
			LOGI("FIND END SLICE, NOW QUEUE, SIZE: %d", nal_pkg->size);

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
			nal_pkg->size = pkg->nal_size;
#ifdef TRANSFER_DEBUG
			LOGI("SLICE: size: %d  seq: %02x", nal_pkg->size, nal_pkg->seq);
#endif
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
#ifdef transfer_free
				LOGI("transfer_free 222");
#endif
					free(pkg->nal_data);
					pkg->nal_data = NULL;
				}
#ifdef transfer_free
				LOGI("transfer_free 333");
#endif
				free(pkg);
				pkg = NULL;
			}
			return NULL;
			break;
	}
}

