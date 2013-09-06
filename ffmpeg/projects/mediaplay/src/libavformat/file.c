/*
 * file.c
 *
 *  Created on: 2013-4-9
 *      Author: Administrator
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "avformat.h"
#include "../libavutil/berrno.h"

static int file_open(URLContext* h, const char* filename, int flags){

	int access;
	int fd;

	strstart(filename, "file:", &filename);

	if(flags & URL_RDWR)
		access = O_CREAT | O_TRUNC | O_RDWR;
	else if(flags & URL_WRONLY)
		access = O_CREAT | O_TRUNC | O_WRONLY;
	else
		access = O_RDONLY;

	fd = open(filename, access, 0666);

	if(fd < 0)
		return -ENOENT;

	h->priv_data = (void*)(size_t)fd;

	return 0;
}


static int file_read(URLContext* h, unsigned char* buf, int size){

	int fd = (size_t)h->priv_data;

	return read(fd, buf, size);
}

static int file_write(URLContext* h, unsigned char* buf, int size){

	int fd = (size_t)h->priv_data;

	return write(fd, buf, size);
}

static offset_t file_seek(URLContext* h, offset_t offset, int whence){

	int fd = (size_t)h->priv_data;

	return lseek(fd, offset, whence);
}

static int file_close(URLContext* h){

	int fd = (size_t)h->priv_data;

	return close(fd);
}

URLProtocol file_protocol = {

		"file",
		file_open,
		file_read,
		file_write,
		file_seek,
		file_close,
};
