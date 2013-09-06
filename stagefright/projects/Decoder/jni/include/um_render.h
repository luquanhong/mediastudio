/***************************************************************************
 * FILE NAME: um_render.h
 *
 * Version: 1.0,  Date: 2012-08-06
 *
 * Description: 
 *
 * Platform: 
 *
 ***************************************************************************/
#ifndef _UM_RENDER_H_
#define _UB_RENDER_H_

#include "um.h"
#include "um_types.h"

#ifdef	__cplusplus
extern "C"{
#endif

typedef enum
{
	UM_OPENGL,
	UM_OPENGLES11,
	UM_OPENGLES20,
}UM_RenderType;


typedef struct VideoRenderContext
{
	UM_RenderType type;
	CodecParams params;
} VRCtx;

UMSint um_render_init();
UMSint um_render_fini();
VRCtx* um_render_create(CodecParams* params);
UMSint um_render_destory(VRCtx* thiz);
UMSint um_render_draw(VRCtx* thiz, UMSint8* buf, UMSint width, UMSint height);
UMSint um_render_resize(VRCtx* thiz, UMSint width, UMSint height);

#ifdef	__cplusplus
}
#endif
		
#endif //_UM_RENDER_H_
