/*
 * avcodec.h
 *
 *  Created on: 2013-4-7
 *      Author: Administrator
 */

#ifndef AVCODEC_H_
#define AVCODEC_H_

#include <stdint.h>
#include "../libavutil/avutil.h"

enum CodecID {

	CODEC_ID_TRUESPEECH,
	CODEC_ID_MSRLE,
	CODEC_ID_NONE
};


enum CodecType{

	CODEC_TYPE_UNKNOWN = -1,
	CODEC_TYPE_VIDEO,
	CODEC_TYPE_AUDIO,
	CODEC_TYPE_DATA
};


#define	AVCODEC_MAX_AUDIO_FRAME_SIZE	192000 //1 second of 48khz 32 bit audio


typedef	struct AVPicture{

	uint8_t *data[4];
	int	linesize[4];
}AVPicture;

typedef	struct AVFrame{

	uint8_t *data[4];
	int	linesize[4];
	uint8_t *base[4];
}AVFrame;


typedef	struct 	AVCodecContext{

	int	bit_rate;
	int	frame_number;

	unsigned char* extradata; 	// Codec的私有数据，对Audio是WAVEFORMATEX结构扩展字节。
	int	extradata_size; 		// 对Video是BITMAPINFOHEADER后的扩展字节

	int width;
	int height;

	enum PixelFormat pix_fmt;

	int sample_rate;
	int	channels;
	int bits_per_sample;
	int block_align;

	struct	AVCodec* codec;
	void*	priv_data;

	enum CodecType	codec_type;
	enum CodecID	codec_id;

	int  (*get_buffer)(struct AVCodecContext* c, AVFrame* pic);
	void (*release_buffer)(struct AVCodecContext* c, AVFrame* pic);
	int	 (*reget_buffer)(struct AVCodecContext* c, AVFrame* pic);

	int internal_buffer_count;
	void* internal_buffer;
}AVCodecContext;


void* av_malloc(unsigned int size);
void* av_mallocz(unsigned int size);
void* av_realloc(void* ptr, unsigned int size);
void  av_free(void* ptr);
void  av_freep(void* arg);

#endif /* AVCODEC_H_ */
