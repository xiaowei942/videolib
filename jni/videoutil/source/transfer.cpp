#include "transfer.h"

#define DEBUG

//线程退出标志，不可用Transfer的成员来控制
bool isExit = false;
void *start_receive_func(void *arg) {
	Transfer *cls = (Transfer *)arg;
	cls->receiveThread();
}

void *start_process_func(void *arg) {
	Transfer *cls = (Transfer *)arg;
	cls->processThread();
}

void Transfer::Exit() {
	isExit = true;
}

Transfer::Transfer(int width, int height) : isReceive(false), isProcess(false), video_width(width), video_height(height) {
	LOGI("Enter Transfer");
	package_queue = new CircleQueue<data_package *>(QUEUE_SIZE);
	if(!package_queue) {
		perror("Cannot Create CircleQueue");
	}

	frame_queue = new CircleQueue<uint8_t *>(QUEUE_SIZE);
	if(!frame_queue) {
		perror("Cannot Create CircleQueue");
	}

	int error = pthread_create(&receive_handle, NULL, &start_receive_func, (void *)this);
	if(error) {
		perror("Cannot Create Thread");
	}

	error = pthread_create(&process_handle, NULL, &start_process_func, (void *)this);
	if(error) {
		perror("Cannot Create Thread");
	}
	LOGI("Leave Transfer");
}

Transfer::~Transfer() {
	LOGI("Enter ~Transfer");
	Exit();
	if(package_queue) {
		delete package_queue;
		package_queue = NULL;
	}

	if(frame_queue) {
		delete frame_queue;
		frame_queue = NULL;
	}
	LOGI("Leave ~Transfer");
}

int Transfer::initDataSocket(const char *server_ip, const char *local_ip, int local_port) {
	if(local_data_socket_fd) {
		return -1;
	}

	/* 创建data socket */
	local_data_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(local_data_socket_fd < 0)
	{
		perror("Create Socket Failed:");
		return -1;	
	}

	if(local_ip) {
		bzero(&local_addr, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(local_ip);
		local_addr.sin_port = htons(local_port);

		/* 绑定套接口 */
		if(-1 == (bind(local_data_socket_fd,(struct sockaddr*)&local_addr,sizeof(local_addr))))
		{
			perror("Client Bind Failed:");
			return -1;
		}
	}
	return 0;
}

int Transfer::initSocket(const char *server_ip, const char *local_ip, const int local_port) {
	int ret;
	LOGI("Now initDataSocket\n");
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

void Transfer::unInitSocket(int port) {
	switch (port) {
		case CONTROL_PORT:
			close(local_control_socket_fd);
			break;
		case DATA_PORT:
			close(local_data_socket_fd);
			break;
		case MANAGE_PORT:
			break;
		case TRANSFER_PORT:
			break;
		default:
			break;
	}
}

void* Transfer::receiveThread() {
	int data_len;
	char buffer[BUFFER_SIZE];
	/* 定义一个地址，用于捕获客户端地址 */
	struct sockaddr_in server_addr;
	socklen_t server_addr_length = sizeof(server_addr);

	while(!isReceive && !isExit) {
#ifdef DEBUG
		LOGI("Now receive sleep\n");
		usleep(1000000);
#else
		usleep(100000);
#endif
	}

	/* 数据传输 */
	while(isReceive && !isExit)
	{
		data_len = 0;
		memset(buffer, 0x0, BUFFER_SIZE);

		/* 接收数据 */
		int rcv_len = recvfrom(local_data_socket_fd, buffer, BUFFER_SIZE, 0,(struct sockaddr*)&server_addr, &server_addr_length);
		if(rcv_len == -1)
		{
			perror("Receive Data Failed:");
			exit(1);
		}

		data_len = get_valid_data_length(buffer);
		if(data_len) {
#ifdef VERIFY
			if(!verify(buffer, data_len)) {
#endif
				data_package *pkg = (data_package *)malloc(sizeof(data_package));			
				assert(pkg);
				if(!pkg) {
					perror("Cannot malloc data_package");
				}

				int result = parse_data_package(buffer, pkg);
				if(result) {
					perror("Parse data_package failed");
				}

				package_queue->enQueue(pkg);
#ifdef VERIFY
			}
#endif
		}
	}

	LOGI("Exit Receive Thread");
}

data_package* Transfer::getDataPackage() {
	data_package *pkg;
	if(package_queue->deQueue(pkg)) {
		return pkg;
	}
	return NULL;
}

void* Transfer::processThread() {
	while(!isProcess && !isExit) {
#ifdef DEBUG
		LOGI("Now process sleep\n");
		usleep(1000000);
#else
		usleep(100000);
#endif
	}

	while(isProcess && !isExit) {
		data_package *pkg = getDataPackage();	
		if(pkg) {
			perror("Cannot Get DataPackage");
		}


//make frame
		int slice_type = get_data_package_slice_ident_type(pkg);
		switch(slice_type) {
			case SLICE_TYPE_INTER: //中间分片
				break;

			case SLICE_TYPE_FIRST: //分片开始

				break;
			case SLICE_TYPE_LAST: //分片结束

				break;
			case SLICE_TYPE_NONE:
				frame_queue->enQueue(pkg->nal_data);
				free(pkg);
				pkg = NULL;
			break;
		}

//release package
		if(pkg) {
			if(pkg->nal_data) {
				free(pkg->nal_data);
				pkg->nal_data = NULL;
			}
			free(pkg);
			pkg = NULL;
		}
	}
	LOGI("Exit Process Thread");
}
