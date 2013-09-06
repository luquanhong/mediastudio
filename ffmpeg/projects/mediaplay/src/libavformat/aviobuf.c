/*
 * aviobuf.c
 *
 *  Created on: 2013-4-9
 *      Author: Administrator
 */

#include "avformat.h"
#include "avio.h"

#define IO_BUFFER_SIZE 	32768


#define url_write_buf NULL

static int url_read_buf(void* opaque, uint8_t* buf, int buf_size){

	URLContext* h = opaque;

	return url_read(h, buf, buf_size);
}

static offset_t url_seek_buf(void* opaque, offset_t offset, int whence){

	URLContext* h = opaque;

	return url_seek(h, offset, whence);
}

int url_fopen(ByteIOContext* s, const char* filename, int flags){

	int err;
	URLContext* h;
	uint8_t* buffer;
	int buffer_size;
	int max_packet_size;

	err = url_open(&h, filename, flags);

	if(err < 0)
		return err;

	max_packet_size = url_get_max_packet_size(h);

	if(max_packet_size)
		buffer_size = max_packet_size;
	else
		buffer_size = IO_BUFFER_SIZE;

	buffer = av_malloc(buffer_size);

	if(!buffer){
		url_close(h);
		return -ENOMEM;
	}

	if(init_put_byte(s, buffer, buffer_size, (h->flags & URL_WRONLY || h->flags & URL_RDONLY),
			h, url_read_buf, url_write_buf, url_seek_buf) < 0 ){

		url_close(h);
		av_free(buffer);
		return AVERROR_IO;
	}

	s->max_packet_size = max_packet_size;

	return 0;
}


int url_fclose(ByteIOContext* s){


}


int url_fread(ByteIOContext* s, unsigned char* buf, int size){


}


offset_t url_fseek(ByteIOContext* s, offset_t offset, int whence){


}


offset_t url_ftell(ByteIOContext* s){


}
