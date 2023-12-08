#ifndef RING_BUF_H
#define RING_BUF_H


#define TRUE	1
#define FALSE	0
#define BUF_SIZE	256
struct ringbuf
{
	unsigned char buf[BUF_SIZE];
	unsigned char write;
	unsigned char read;
};
typedef struct ringbuf *ptr_ringbuf_t;
	
void ringbuf_init(ptr_ringbuf_t ringbuf);
//int ringbuf_flush(ringbuf_t* ringbuf);
//uint8_t ringbuf_empty(pt_ringbuf_t ringbuf);
//uint8_t ringbuf_full(pt_ringbuf_t ringbuf);
void ringbuf_write(ptr_ringbuf_t ringbuf, unsigned char data);
unsigned char ringbuf_read(unsigned char* buf, ptr_ringbuf_t ringbuf);

#endif

