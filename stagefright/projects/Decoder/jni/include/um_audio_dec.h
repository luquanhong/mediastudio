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
#ifndef _UM_AUDIO_DEC_H_
#define _UM_AUDIO_DEC_H_

#include "um.h"
#include "um_types.h"


typedef struct AudioDecoderContext
{
	CodecParams params;
	UMSint8* outData;
	UMSint outDataLen, outSample_rate, outSample_fmt, outChannels;
	UMSint8 priv[0];
}ADCtx;

#ifdef	__cplusplus
extern "C"{
#endif

UMSint 	um_adec_init();
UMSint 	um_adec_fini();
ADCtx* 	um_adec_create(CodecParams* params);
UMSint 	um_adec_destroy(ADCtx* thiz);
UMSint 	um_adec_decode(ADCtx* thiz, UMSint8* buf, UMSint bufLen);
UMSint 	um_adec_reset(ADCtx* thiz);

#ifdef	__cplusplus
}
#endif
		
#endif //_UM_AUDIO_DEC_H_
