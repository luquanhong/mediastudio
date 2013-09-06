/*!
*	\file	offer the same interface for decode.
*/

#include <surfaceflinger/Surface.h>
#include "um_video_dec.h"
#include "um_types.h"
#include "um_sys_thread.h"
#include "UM_MediaPlayer.h"



/*!
*	\brief	type define.
*/
typedef	struct _StageFrightVDecPriv
{
	UMMediaPlayer* mPlayer;	//!< main instance for decode.
	
}StageFrightVDecPriv;

/*!
*	\brief	set the surface for the output port	of decode, it's only needed by dsp.
*/
int um_vdec_setSurface(VDCtx* thiz,void* surface)
{
	StageFrightVDecPriv* priv = NULL;

	if(!thiz ||!surface)
		return UMFailure;
	
	priv = (StageFrightVDecPriv*)thiz->priv;
	
	Surface* const p = (Surface*)surface;
	
	sp<Surface> isurface=sp<Surface>(p);
	if(priv)
	{
		UMMediaPlayer* player=(UMMediaPlayer*)priv->mPlayer;  
		player->setSurface(isurface);
	}
	
	return UMSuccess;
}


/*!
*	\brief	video init.
*/
UMSint um_vdec_init()
{
	
	return UMSuccess;
}

/*!
*	\brief	video finit.
*/
UMSint um_vdec_fini()
{
	return UMSuccess;
}

/*!
*	\brief	create the video player instance for decode.
*/
VDCtx* um_vdec_create(CodecParams* params)
{
	
	VDCtx* thiz = NULL;
	
	if(!params)
		return UM_NULL;
		
	if((thiz = (VDCtx*)malloc(sizeof(VDCtx) + sizeof(StageFrightVDecPriv))) != NULL)
	{
		StageFrightVDecPriv* priv = (StageFrightVDecPriv* )thiz->priv;
		memcpy(&thiz->params, params, sizeof(CodecParams));
		
		priv->mPlayer = new UMMediaPlayer(params->frameType,params->v.width,params->v.height);
	}

	return thiz;
}

/*!
*	\brief	destroy the video player instance for decode.
*/
UMSint um_vdec_destroy(VDCtx* thiz)
{
	StageFrightVDecPriv* priv = NULL;
	
	if(!thiz)
		return UMFailure;
	
	priv = (StageFrightVDecPriv*)thiz->priv;

	if(priv->mPlayer!=NULL)
	{

		priv->mPlayer->release();	
		delete priv->mPlayer;
		priv->mPlayer = NULL;
	}
	
	free(thiz);

	return UMSuccess;
}

/*!
*	\brief	decode the input data.
*	\param	in	it's input frame data buf.
*	\param	len	it's the length of input frame data.
*	\return	if success, it will return UM_SUCCESS.
*/
UMSint um_vdec_decode(VDCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	StageFrightVDecPriv* priv = NULL;
	int ret = UMFailure;
	int pts= 0;

	if(!thiz || !buf || bufLen<=0) 
		return UMFailure;
	
	if(thiz->resetFlag)
	{
		//stage_reset_impl(thiz);
	}

	priv = (StageFrightVDecPriv*)thiz->priv;
	UMMediaPlayer* player=(UMMediaPlayer*)priv->mPlayer;
	
	if(NULL != player)
	{
		ret=player->setInputData((char*)buf,bufLen,pts);//!< when the first invoke, it will create decoder according to the buf. 
		
		player->play();//!< when the first invoke, it will start the decode thread.
	}

	return ret;
}

/*!
*	\brief	stop the decode input data.
*/
void um_vdec_stop(VDCtx* thiz)
{
	StageFrightVDecPriv* priv = NULL;

	if(!thiz)
		return ;
		
	priv = (StageFrightVDecPriv*)thiz->priv;
	
	if(priv->mPlayer)
	{
		if(priv->mPlayer->isPlaying())
			priv->mPlayer->stop();
	}
}

/*!
*	\brief	reset the decode whill it's needed, now it's not implement on the dsp.
*/
UMSint um_vdec_reset(VDCtx* thiz)
{
	if(!thiz) 
		return UMFailure;
	
	thiz->resetFlag++;

	return UMSuccess;
}

void um_vdec_setCompensateNum(VDCtx* thiz, int frameNumbers)
{
	StageFrightVDecPriv* priv = NULL;

	if(!thiz)
		return;
		
	priv = (StageFrightVDecPriv*)thiz->priv;
	if(priv->mPlayer)
	{
		priv->mPlayer->setCompensateNum(frameNumbers);
	}
	return;
}

