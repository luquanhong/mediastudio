/*
 * avio.h
 *
 *  Created on: 2013-4-7
 *      Author: Administrator
 */

#ifndef AVIO_H_
#define AVIO_H_

#include <stdint.h>

typedef int64_t offset_t;

#define	URL_RDONLY	0
#define	URL_WRONLY	1
#define	URL_RDWR	2


typedef	struct URLContext{

	struct URLProtocol* prot;
	int flags;
	int max_packet_size;
	void* priv_data;
	char filename[1];
}URLContext;


typedef	struct URLProtocol{

	const char* name;

	int (*url_open)(URLContext* h, const char* filename, int flags);
	int (*url_read)(URLContext* h, unsigned char* buf, int size);
	int (*url_write)(URLContext* h, unsigned char* buf, int size);
	offset_t (*url_seek)(URLContext* h, offset_t pos, int whence);
	int (*url_close)(URLContext* h);

	struct URLProtocol* next;
}URLProtocol;




typedef struct ByteIOContext
{
    unsigned char *buffer;
    int buffer_size;
    unsigned char *buf_ptr,  *buf_end;
    void *opaque;
    int (*read_buf)(void *opaque, uint8_t *buf, int buf_size);
    int (*write_buf)(void *opaque, uint8_t *buf, int buf_size);
    offset_t(*seek)(void *opaque, offset_t offset, int whence);
    offset_t pos;    // position in the file of the current buffer
    int must_flush;  // true if the next seek should flush
    int eof_reached; // true if eof reached
    int write_flag;  // true if open for writing
    int max_packet_size;
    int error;       // contains the error code or 0 if no error happened
} ByteIOContext;


int register_protocol(URLProtocol* protocol);



int url_fopen(ByteIOContext* s, const char* filename, int flags);
int url_fclose(ByteIOContext* s);
int url_fread(ByteIOContext*s, unsigned char* buf, int size);
offset_t url_fseek(ByteIOContext* s, offset_t offset, int whence);
offset_t url_ftell(ByteIOContext* s);

#endif /* AVIO_H_ */
