#include "transfer.h"

void *start_receive_func(void *arg) {
	Transfer *cls = (Transfer *)arg;
	cls->receiveThread();
}

void *start_process_func(void *arg) {
	Transfer *cls = (Transfer *)arg;
	cls->processThread();
}

Transfer::Transfer() : isRecieve(false), isProcess(false) {
	package_queue = new CircleQueue<data_package *>(QUEUE_SIZE);
	if(!package_queue) {
		perror("Cannot Create CircleQueue");
	}

	frame_queue = new CircleQueue<uint8_t *>(QUEUE_SIZE);
	if(!frame_queue) {
		perror("Cannot Create CircleQueue");
	}

	int error = pthread_create(handle, NULL, start_receive_func, (void *)this);
	if(error) {
		perror("Cannot Create Thread");
	}

	error = pthread_create(handle, NULL, start_receive_func, (void *)this);
	if(error) {
		perror("Cannot Create Thread");
	}
}

Transfer::~Transfer() {
	if(package_queue) {
		delete package_queue;
		package_queue = NULL;
	}

	if(frame_queue) {
		delete frame_queue;
		frame_queue = NULL;
	}
}

int Transfer::initSocket(const char *server_ip, const char *local_ip, int port) {
	/* 创建socket */
	local_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(local_socket_fd < 0)
	{
		perror("Create Socket Failed:");
		return -1;	
	}

	if(local_ip) {
		bzero(&local_addr, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(local_ip);
		local_addr.sin_port = htons(port);

		/* 绑定套接口 */
		if(-1 == (bind(local_socket_fd,(struct sockaddr*)&local_addr,sizeof(local_addr))))
		{
			perror("Client Bind Failed:");
			return -1;
		}
	}
}

void* Transfer::receiveThread() {
	int data_len;
	/* 定义一个地址，用于捕获客户端地址 */
	struct sockaddr_in server_addr;
	socklen_t server_addr_length = sizeof(server_addr);

	while(!isRecieve) {
		usleep(100000);
	}

	/* 数据传输 */
	while(isRecieve)
	{
		data_len = 0;

		/* 接收数据 */
		char buffer[BUFFER_SIZE];
		memset(buffer, 0x0, BUFFER_SIZE);

		int rcv_len = recvfrom(local_socket_fd, buffer, BUFFER_SIZE, 0,(struct sockaddr*)&server_addr, &server_addr_length);
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
}

data_package* Transfer::getDataPackage() {
	data_package *pkg;
	if(package_queue->deQueue(pkg)) {
		return pkg;
	}
	return NULL;
}

void* Transfer::processThread() {
	while(!isProcess) {
		usleep(100000);
	}

	while(isProcess) {
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
}
