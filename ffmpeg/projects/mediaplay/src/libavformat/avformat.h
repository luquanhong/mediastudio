/*
 * avformat.h
 *
 *  Created on: 2013-4-7
 *      Author: Administrator
 */

#ifndef AVFORMAT_H_
#define AVFORMAT_H_

#include <stddef.h>
#include <stdio.h>

#include "../libavcodec/avcodec.h"
#include "../libavutil/berrno.h"
#include "avio.h"

#define AVERROR_UNKNOWN			(-1)
#define AVERROR_IO				(-2)
#define	AVERROR_NUMEXPECTED		(-3)
#define	AVERROR_INVALIDDATA		(-4)
#define	AVERROR_NOMEM			(-5)
#define	AVERROR_NOFMT			(-6)
#define	AVERROR_NOTSUPP			(-7)


#define MAX_STREAMS	20


typedef	struct AVProbeData{

	const char* fileName;
	unsigned char* buf;
	int buf_size;
}AVProbeData;

typedef	struct AVFormatParameters{

	int dbg;
}AVFormatParameters;


typedef	struct AVPaket{

	int64_t pts;
	int64_t	dst;
	int64_t pos;
	uint8_t* data;
	int size;
	int stream_index;
	int flags;

	void (*destruct)(struct AVPacket);
}AVPacket;


typedef	struct AVPacketList{

	AVPacket pkt;
	struct 	AVPacketList* next;
}AVPacketList;


typedef	struct AVStream {

	AVCodecContext* actx;

	void* priv_data;

	//AVRational time_base;

	//AVIndexEntry* index_entries;
	int nb_index_entries;
	int index_entries_allocated_size;

	double	frame_lase_delay;
}AVStream;

typedef	struct AVInputFormat{

	const char* name;
	int	priv_data_size;

	int (*read_probe)(AVProbeData* data);
	int (*read_header)(struct AVFormatContext*, AVFormatParameters* ap);
	int (*read_packet)(struct AVFormatContext*, AVPacket* pkt);
	int (*read_close)(struct AVFormatContext*);

	const char* extensions;

	struct AVInputFormat* next;
}AVInputFormat;

typedef	struct AVFormatContex{

	struct AVInputFormat* iformat;
	void* priv_data;
	ByteIOContext pb;
	int nb_stream;

	AVStream* streams[MAX_STREAMS];
}AVFormatContext;


void av_register_all(void);

int strstart(const char *str, const char *val, const char **ptr);
void pstrcpy(char* dst, int buf_size, const char* str);

#endif /* AVFORMAT_H_ */
