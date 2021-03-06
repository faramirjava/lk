/*
 * Copyright (c) 2014 Brian Swetland
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _NET_PKTBUF_H_
#define _NET_PKTBUF_H_

#include <sys/types.h>
#include <list.h>

#define PKTBUF_SIZE 2048
#define PKTBUF_BUF_SIZE 1984
#define PKTBUF_MAX_DATA 1536
#define PKTBUF_MAX_HDR (PKTBUF_BUF_SIZE - PKTBUF_MAX_DATA)

typedef struct pktbuf {
	struct list_node list;
	u8 *data;
	u32 dlen;
	u32 phys_base;
	u32 rsv0;
	u32 rsv1;
	u32 rsv2;
	u8 buffer[PKTBUF_BUF_SIZE];
} pktbuf_t;

static inline u32 pktbuf_data_phys(pktbuf_t *p) {
	return p->phys_base + (p->data - p->buffer);
}

// number of bytes available for _prepend
static inline u32 pktbuf_avail_head(pktbuf_t *p) {
	return p->data - p->buffer;
}

// number of bytes available for _append or _append_data
static inline u32 pktbuf_avail_tail(pktbuf_t *p) {
	return PKTBUF_BUF_SIZE - (p->data - p->buffer) - p->dlen;
}

// allocate packet buffer from buffer pool
pktbuf_t *pktbuf_alloc(void);

// return packet buffer to buffer pool
void pktbuf_free(pktbuf_t *p);

// extend buffer by sz bytes, copied from data
void pktbuf_append_data(pktbuf_t *p, const void *data, size_t sz);

// extend buffer by sz bytes, returning a pointer to the
// start of the newly appended region
void *pktbuf_append(pktbuf_t *p, size_t sz);

// grow the front of the buffer and return a pointer
// to the new start of packet 
void *pktbuf_prepend(pktbuf_t *p, size_t sz);

// shrink the buffer by discarding the first sz bytes
// returning a pointer to the discarded bytes (which
// will remain untouched until the next _prepend),
// or NULL if there were not enough bytes to consume
void *pktbuf_consume(pktbuf_t *p, size_t sz);

// remove sz bytes from the end of the pktbuf
void pktbuf_consume_tail(pktbuf_t *p, size_t sz);

// create a new packet buffer from raw memory and add
// it to the free pool
void pktbuf_create(void *ptr, u32 phys, size_t size);

#endif

// vim: set noexpandtab:
