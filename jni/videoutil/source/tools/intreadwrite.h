#ifndef __INTREADWRITE_H__
#define __INTREADWRITE_H__
#define AV_RB32(x)  ((((const uint8_t*)(x))[0] << 24) | /
 (((const uint8_t*)(x))[1] << 16) | /
 (((const uint8_t*)(x))[2] <<  8) | /
 ((const uint8_t*)(x))[3])
#endif
