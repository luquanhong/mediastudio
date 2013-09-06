/*!	
*	\file	FILE NAME: um_video_dec.h
*
* Version: 1.0,  Date: 2012-08-06
*
* Description: 
*
* Platform: 
*
*/
#ifndef _UM_VIDEO_DEC_H_
#define _UM_VIDEO_DEC_H_

#include "um.h"
#include "um_types.h"

UMSint is_register_decoder = 0;

/*!
*	\brief 	The main decoder data struct.
*/
typedef struct VideoDecoderContext
{
	CodecParams params;//!<	the input params for setting decode.
	// output frame size
	UMSint outWidth, outHeight;
	UMSint8* outData;
	UMSint outDataLen, outGotpicture;
	UMSint resetFlag;
	UMSint8 priv[0];
}VDCtx;


#ifdef	__cplusplus
extern "C"{
#endif
/*!
*	\brief	initialize the video decoder.it's used for software decoding.
*/
UMSint 	um_vdec_init();

/*!
*	\brief	release the resource of initialization.it's used for software decoding.
*/
UMSint 	um_vdec_fini();

/*!
*	\brief	create the decoder with using the input params.
*	\param	params	the input params for setting decode.
*	\return	it will return VideoDecoderContext, if it create success. otherwise it will return null.
*/
VDCtx* 	um_vdec_create(CodecParams* params);

/*!
*	\brief	release the decoder resource when the decoder  close.
*/
UMSint 	um_vdec_destroy(VDCtx* thiz);

/*!
*	\brief	pass the display surface to decoder,it's only used for hardware decoding.
*	\param	thiz 	the decoder context.
*	\param	surface 	the input pramas, it's passed from java layer. 
*	\return	UMSuccess  	the surface set succesful.
*			UMFailure		have error with setting the surface.
*/
UMSint 	um_vdec_setSurface(VDCtx* thiz,void* surface);

/*!
*	\brief	decode the input data.
*	\param	in	it's input frame data buf.
*	\param	len	it's the length of input frame data.
*	\return	if success, it will return UM_SUCCESS.
*/
UMSint 	um_vdec_decode(VDCtx* thiz, UMSint8* buf, UMSint bufLen);

/*!
*	\brief	reset the decoder, it's used for software decoding.
*/
UMSint 	um_vdec_reset(VDCtx* thiz);

/*!
*	\brief	add the compensate frames for hardware decoder, it's used for hardware decoding.
*	\param	thiz	the decoder context.
*	\param	frameNumbers	needed frame number.
*/
void um_vdec_setCompensateNum(VDCtx* thiz,int frameNumbers);

#ifdef	__cplusplus
}
#endif
		
#endif //_UM_VIDEO_DEC_H_
