#include "fifo.h"

/**
 * @brief create a fifo
 * 
 * @param pfifo fifo point
 * @param buf source buffer addr
 * @param size buffer size
 * @param lock fifo lock func
 * @param unlock fifo unlock func
 */
void fifo_create(fifo_t *pfifo, uint8_t *buf, uint32_t size,
                plock_status_cb lock, plock_status_cb unlock)
{
    if(pfifo == NULL)
    {
        return;
    }
    pfifo->buf = buf;
    pfifo->buf_size = size;
    pfifo->buf_cnt = 0;
    pfifo->pread = pfifo->buf;
    pfifo->pwrite = pfifo->buf;
    pfifo->lock = lock;
    pfifo->unlock = unlock;
}

/**
 * @brief destory fifo
 * 
 * @param pfifo 
 */
void fifo_destory(fifo_t *pfifo)
{
    pfifo->buf = NULL;
    pfifo->buf_size = 0;
    pfifo->buf_cnt = 0;
    pfifo->pread = pfifo->buf;
    pfifo->pwrite = pfifo->buf;
    pfifo->lock = NULL;
    pfifo->unlock = NULL;

    memset(pfifo, 0x00, sizeof(fifo_t));
}

static uint32_t get_fifo_total_size(fifo_t *pfifo)
{
    if(pfifo == NULL)
    {
        return NULL;
    }
    return pfifo->buf_size;
}

static uint32_t get_fifo_occupy_size(fifo_t *pfifo)
{
    if(pfifo != NULL)
    {
        return pfifo->buf_cnt;
    }
    return NULL;
}

static uint32_t get_fifo_free_size(fifo_t *pfifo)
{
    uint32_t free_size;
    if(pfifo == NULL)
    {
        return 0;
    }
    free_size = (pfifo->buf_size) - (pfifo->buf_cnt);

    return free_size;
}

/**
 * @brief write fifo
 * 
 * @param pfifo 
 * @param pbuf write to fifo buffer
 * @param size buffer size
 * @return uint32_t write to fifo buffer size
 */
uint32_t fifo_write(fifo_t *pfifo, const uint8_t *pbuf, uint32_t size)
{
    if(pfifo == NULL || pbuf == NULL || size == 0)
    {
        return 0;
    }
    
    uint32_t w_size = 0;
    uint32_t free_size = 0;

    free_size = get_fifo_free_size(pfifo);
    if(free_size == 0)
    {
        return 0;
    }

    if(free_size < size)
    {
        size = free_size;
    }

    w_size = size;

    pfifo->lock;
    while(w_size > 0)
    {
        *(pfifo->pwrite) = *pbuf;
        *(pfifo->pwrite)++;
        *pbuf++;

        if(pfifo->pwrite >= (pfifo->buf + pfifo->buf_size))
        {
            /* circulate */
            pfifo->pwrite = pfifo->buf;
        }
        pfifo->buf_cnt++;
        w_size--;
    }
    pfifo->unlock;
    return size;
}

uint32_t fifo_read(fifo_t *pfifo, uint8_t *pbuf, uint32_t size)
{
    if(pfifo == NULL || pbuf == NULL || size == 0)
    {
        return 0;
    }
    
    uint32_t r_size = 0;
    uint32_t occupy_size = 0;

    occupy_size = get_fifo_occupy_size(pfifo);
    if(occupy_size == 0)
    {
        return 0;
    }

    if(occupy_size < size)
    {
        size = occupy_size;
    }

    r_size = size;

    pfifo->lock;
    while(r_size > 0)
    {
        *pbuf = *(pfifo->pread);
        *(pfifo->pread)++;
        *pbuf++;

        if(pfifo->pread >= (pfifo->buf + pfifo->buf_size))
        {
            /* circulate */
            pfifo->pread = pfifo->buf;
        }
        pfifo->buf_cnt--;
        r_size--;
    }
    pfifo->unlock;
    return size;
}
