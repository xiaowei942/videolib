#ifndef __GETBITCONTEXT_H__
#define __GETBITCONTEXT_H__

typedef struct GetBitContext {
    uint8_t *buffer, *buffer_end;
    int index;
    int size_in_bits;
    int size_in_bits_plus8;
} GetBitContext;
#endif
