#include "include.h"


static uint8_t ringbuf_empty(ringbuf_t* ringbuf);
static uint8_t ringbuf_full(ringbuf_t* ringbuf);

void ringbuf_init(ringbuf_t* ringbuf)
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

static uint8_t ringbuf_empty(ringbuf_t* ringbuf)
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

static uint8_t ringbuf_full(ringbuf_t* ringbuf)
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

void ringbuf_write(ringbuf_t* ringbuf, unsigned char data)
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

uint8_t ringbuf_read(ringbuf_t* ringbuf)
{
	unsigned char buf;

	if(ringbuf != NULL)
	{
		if(ringbuf_empty(ringbuf) != TRUE)
		{
			(ringbuf->read) = ((ringbuf->read) + 1) % BUF_SIZE;
			buf = ringbuf->buf[ringbuf->read];
			return buf;
		}
		else
			return FALSE; 
	}
	else
		return FALSE;	
}
