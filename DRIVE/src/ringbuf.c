#include "include.h"

void ringbuf_init(pt_ringbuf_t ringbuf)
{
	ringbuf->read = 0;
	ringbuf->write = 0;
	memset(ringbuf->buf, '\0', sizeof(ringbuf->buf));
}

uint8_t ringbuf_empty(pt_ringbuf_t ringbuf)
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

uint8_t ringbuf_full(pt_ringbuf_t ringbuf)
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

void ringbuf_write(pt_ringbuf_t ringbuf, unsigned char data)
{
	if(ringbuf != NULL)
	{
		if(ringbuf_full(ringbuf) != TRUE)
		{
			ringbuf->write = (ringbuf->write + 1) % BUF_SIZE;
			ringbuf->buf[ringbuf->write] = data;
		}
	}
}

uint8_t ringbuf_read(pt_ringbuf_t ringbuf)
{
	unsigned char buf;

	if(ringbuf != NULL)
	{
		(ringbuf->read) = ((ringbuf->read) + 1) % BUF_SIZE;
		buf = ringbuf->buf[ringbuf->read];
		return buf;
	}
	else
		return FALSE;	
}
