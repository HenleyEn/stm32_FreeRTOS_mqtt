#ifndef RING_BUF
#define RING_BUF


#define TRUE	1
#define FALSE	0
#define BUF_SIZE	20
typedef struct {
	uint8_t buf[BUF_SIZE];
	uint8_t write;
	uint8_t read;
}ringbuf_t, *pt_ringbuf_t;

void ringbuf_init(pt_ringbuf_t ringbuf);
uint8_t ringbuf_empty(pt_ringbuf_t ringbuf);
uint8_t ringbuf_full(pt_ringbuf_t ringbuf);
void ringbuf_write(pt_ringbuf_t ringbuf, unsigned char data);
uint8_t ringbuf_read(pt_ringbuf_t ringbuf);

#endif

