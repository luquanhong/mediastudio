/***************************************************************************
 * FILE NAME: um_audio_dec.h
 *
 * Version: 1.0,  Date: 2012-08-06
 *
 * Description: 
 *
 * Platform: 
 *
 ***************************************************************************/
 
#ifndef _UM_H_
#define _UM_H_

#include "um_types.h"
typedef enum
{
	CODEC_DEVICE_NONE,
	CODEC_DEVICE_FFMPEG,
	CODEC_DEVICE_OMX,
	CODEC_DEVICE_DSP,
}CodecDevice;

typedef enum
{
	CODEC_UID_NONE = 0,
	CODEC_UID_AAC,
	CODEC_UID_H264,
	CODEC_UID_MPEG4,
	CODEC_UID_MPEG2VIDEO,
	
	CODEC_UID_H263,
}CodecUID;

typedef enum
{
	FRAME_TYPE_NONE ,
	FRAME_TYPE_YUV,
	FRAME_TYPE_RGB,
}FrameType;


typedef struct _CodecParams
{
	CodecDevice device;
	CodecUID codec;
	FrameType frameType;
	union
	{
		struct
		{
			UMSint width, height;
		}v;
		struct 
		{
			UMSint channels, sampleRate;
		}a;
	};
}CodecParams;

#endif
