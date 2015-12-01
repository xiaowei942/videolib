#ifndef __TRANSFER_PROTOCOL_H__
#define __TRANSFER_PROTOCOL_H__
#include <stdint.h>

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
#endif
