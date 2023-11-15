#include "include.h"
#include "ringbuf.h"


static uint8_t ringbuf_empty(ptr_ringbuf_t ringbuf);
static uint8_t ringbuf_full(ptr_ringbuf_t ringbuf);

void ringbuf_init(ptr_ringbuf_t ringbuf)
{
	ringbuf->read = 0;
	ringbuf->write = 0;
	memset(ringbuf->buf, 0, BUF_SIZE);
}

/*
int ringbuf_flush(ringbuf_t* ringbuf)
{
	if(ringbuf != NULL)
	{
		memset(ringbuf->buf, 0, BUF_SIZE);
		return TRUE;
	}
	
	return FALSE;
}
*/

static uint8_t ringbuf_empty(ptr_ringbuf_t ringbuf)
{
	if(ringbuf != NULL)
	{
		if(ringbuf->read == ringbuf->write)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;

}

static uint8_t ringbuf_full(ptr_ringbuf_t ringbuf)
{
	uint8_t full_flag;
 	if(ringbuf != NULL)
 	{
 		full_flag = (ringbuf->write + 1) % BUF_SIZE; 
 		if(full_flag == ringbuf->read)
			return TRUE;
		else
			return FALSE;
 	}
 	else
 		return FALSE;

}

void ringbuf_write(ptr_ringbuf_t ringbuf, unsigned char data)
{
	if(ringbuf != NULL)
	{
		if(ringbuf_full(ringbuf) != TRUE)
		{
			ringbuf->buf[ringbuf->write] = data;
			ringbuf->write = (ringbuf->write + 1) % BUF_SIZE;
		}
	}
}

uint8_t ringbuf_read(uint8_t* buf, ptr_ringbuf_t ringbuf)
{
	if(ringbuf == NULL || ringbuf_empty(ringbuf) == TRUE)
	{
			return FALSE; 
	}
	
	*buf = ringbuf->buf[ringbuf->read];
	(ringbuf->read) = ((ringbuf->read) + 1) % BUF_SIZE;
	return TRUE;
	
}
