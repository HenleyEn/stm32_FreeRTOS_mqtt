#ifndef _FIFO_H
#define _FIFO_H

#include "include.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*plock_status_cb)(void);
typedef struct __fifo fifo_t;
typedef struct __fifo *pfifo;

struct __fifo
{
    uint8_t *buf;
    uint32_t buf_size;

    uint32_t buf_cnt;

    uint8_t *pwrite;
    uint8_t *pread;

    void (*lock)(void);
    void (*unlock)(void);
};


void fifo_create(fifo_t *pfifo, uint8_t *buf, uint32_t size, plock_status_cb lock, plock_status_cb unlock);
void fifo_destory(fifo_t *pfifo);
uint32_t fifo_write(fifo_t *pfifo, const uint8_t *pbuf, uint32_t size);
uint32_t fifo_read(fifo_t *pfifo, uint8_t *pbuf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
