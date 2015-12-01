#ifndef __TRANSFER_PROTOCOL_H__
#define __TRANSFER_PROTOCOL_H__
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

typedef struct _data_package {
	uint16_t package_length;
	uint8_t seq;
	uint8_t package_type;
	uint8_t slice_ident;
	uint8_t *nal_data;
	uint8_t verify;
} data_package;

typedef struct _control_package {
	uint16_t package_length;
	uint8_t seq;
	uint8_t package_type;
	uint8_t command:6;
	uint8_t ack:2;
	uint8_t *oid;
	uint8_t verify;
} control_package;

typedef struct _manage_package {
	uint16_t package_length;
	uint8_t seq;
	uint8_t package_type;
	uint8_t command:6;
	uint8_t ack:2;
	uint8_t *param;
	uint8_t verify;
} manage_package;

typedef struct _transfer_package {
	uint8_t package_length;
	uint8_t seq;
	uint8_t src_addr;
	uint8_t dest_addr;
	uint8_t *content;
	uint8_t verify;
	uint8_t end_flag;
} transfer_package;

#define CONTROL_PORT  	6006
#define DATA_PORT 	6007
#define MANAGE_PORT  	6008
#define TRANSFER_PORT 	6009

#define SWAP16(a) ( (a)<<8 | (a)>>8 )

/*
 * 功能：按位异或方式校验
 * 参数：
 * 	data 	需要校验的数据
 * 返回值:
 * 	0 	校验成功
 * 	-1 	校验失败
 */
int verify(const char *data, int size) {
	assert(data);
	const char *buf = data;
	char result=0;
	for(int i=0; i<size-1; i++) {
		result^=*buf++;
	}
	if(result != data[size-1]) {
		return -1;
	}

	return 0;
}

/*
 * 功能：分析数据并填充到对应数据包
 * 参数：
 *  	data 	需要分析的数据
 *  	package 返回分析后的数据包
 * 返回值：
 * 	0 	成功
 * 	-1 	失败
 */
int parse_data_package(char *data, int size, data_package *package) {
	assert(package);
	uint16_t *p16 = (uint16_t *)data;
	uint8_t *p8 = (uint8_t *)data;
	package->package_length = SWAP16(p16[0]);
	package->seq = p8[2];
	package->package_type = p8[3];
	package->slice_ident = p8[4];
	memcpy(package->nal_data, &p8[5], size-6);
	package->verify=p8[size-1];
}

int parse_control_package(char *data) {

}

int parse_manage_package(char *data) {

}

int parse_transfer_package(char *data) {

}
#endif
