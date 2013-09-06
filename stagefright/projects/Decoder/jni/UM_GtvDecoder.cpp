#include <stdio.h>
#include <stddef.h>

#include <OMX_Component.h>
#include <unistd.h>
#include <pthread.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/CameraSource.h>
#include <media/stagefright/MediaDefs.h>
#include <ui/PixelFormat.h>
#include <binder/IPCThreadState.h>
#include <surfaceflinger/Surface.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/OMXCodec.h>
#include <utils/threads.h>
#include <media/MediaPlayerInterface.h>
#include <media/stagefright/OMXClient.h>
#include <utils/RefBase.h>
#include <media/stagefright/Utils.h>
#include <media/IOMX.h>
#include <utils/String8.h>
#include <media/stagefright/MediaSource.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <media/AudioTrack.h>
#include <binder/ProcessState.h>
#include <utils/List.h>
#include <new>

#include "um_video_dec.h"

using namespace android;

struct Frame {    
	int32_t status;
	size_t size;    
	int64_t time;    
	int key;    
	uint8_t *buffer;   
	MediaBuffer* mbuffer;    
	int32_t w, h;
};

class GtvSource;

struct GtvDecoderPriv {
    //AVCodecContext *avctx;
    //AVBitStreamFilterContext *bsfc;
    //uint8_t* orig_extradata;
    //int orig_extradata_size;
    List<Frame*> *in_queue;//, *out_queue;
    pthread_mutex_t in_mutex;//, out_mutex;
    pthread_cond_t condition;
    pthread_t decode_thread_id;

    Frame *end_frame;
    bool source_done;
    volatile sig_atomic_t thread_started, thread_exited, stop_decode;
	bool decoder_start;

    //AVFrame ret_frame;

    uint8_t *dummy_buf;
    int dummy_bufsize;

    OMXClient *client;
	sp<MediaSource> *source;
    sp<MediaSource> *decoder;
	sp<ANativeWindow> *renderer;
	sp<Surface> *surface;
    const char *decoder_component;
};

struct GtvDecoderPriv* g_dec;

class GtvSource : public MediaSource {
public:
    GtvSource(VDCtx *avctx, sp<MetaData> meta) {
        priv = (GtvDecoderPriv*)avctx->priv;
        source_meta = meta;
        frame_size  = (avctx->params.v.width * avctx->params.v.width * 3) / 2;
        buf_group.add_buffer(new MediaBuffer(frame_size));
    }

    virtual sp<MetaData> getFormat() {
        return source_meta;
    }

    virtual int32_t start(MetaData *params) {
        return OK;
    }

    virtual int32_t stop() {
        return OK;
    }

    virtual int32_t read(MediaBuffer **buffer,
                          const MediaSource::ReadOptions *options) {
        Frame *frame;
        int32_t ret;

        if (priv->thread_exited)
            return ERROR_END_OF_STREAM;
        pthread_mutex_lock(&priv->in_mutex);

        while (priv->in_queue->empty())
            pthread_cond_wait(&priv->condition, &priv->in_mutex);

        frame = *priv->in_queue->begin();
        ret = frame->status;

        if (ret == OK) {
            ret = buf_group.acquire_buffer(buffer);
            if (ret == OK) {
                memcpy((*buffer)->data(), frame->buffer, frame->size);
                (*buffer)->set_range(0, frame->size);
                (*buffer)->meta_data()->clear();
                (*buffer)->meta_data()->setInt32(kKeyIsSyncFrame,frame->key);
                (*buffer)->meta_data()->setInt64(kKeyTime, frame->time);
            } else {
                LOGE("Failed to acquire MediaBuffer");
            }
            free(&frame->buffer);
        }

        priv->in_queue->erase(priv->in_queue->begin());
        pthread_mutex_unlock(&priv->in_mutex);

        free(&frame);
        return ret;
    }

private:
    MediaBufferGroup buf_group;
    sp<MetaData> source_meta;
    GtvDecoderPriv *priv;
    int frame_size;
};

int um_vdec_setSurface(VDCtx* thiz,void* surface)
{
	GtvDecoderPriv* priv = NULL;

	if(!thiz ||!surface)
		return UMFailure;
	
	priv = (GtvDecoderPriv*)thiz->priv;
	
	Surface* const p = (Surface*)surface;
	
	sp<Surface> isurface=sp<Surface>(p);
	if(priv)
	{
		*priv->surface = isurface;
		//UMMediaPlayer* player=(UMMediaPlayer*)priv->mPlayer;  
		//player->setSurface(isurface);
	}
	
	return UMSuccess;
}


VDCtx* um_vdec_create(CodecParams* params)
{
	VDCtx* thiz = NULL;
	sp<MetaData> meta, outFormat;
	
	if(!params)
		return NULL;
	
	thiz = (VDCtx*)malloc(sizeof(VDCtx) + sizeof(GtvDecoderPriv));
	if(!thiz){
		LOGE("VDCtx mallloc memory fail");
		return NULL;
	}
	
	GtvDecoderPriv* priv = (GtvDecoderPriv *)thiz->priv;
	
	pthread_mutex_init(&priv->in_mutex, NULL);
    pthread_cond_init(&priv->condition, NULL);
	
	priv->thread_started = false;
	priv->decoder_start = false;
	
	meta = new MetaData;
    if (meta == NULL) {
		LOGE("MetaData mallloc memory fail");
        return NULL;
    }
	
	memcpy(&thiz->params,params,sizeof(CodecParams));
	
	if(params->frameType == 1)
		meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG4);
	else if(params->frameType == 2)
		meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_AVC);
		
    meta->setInt32(kKeyWidth, params->v.width);
    meta->setInt32(kKeyHeight, params->v.height);
    //meta->setData(kKeyAVCC, kTypeAVCC, avctx->extradata, avctx->extradata_size);

    android::ProcessState::self()->startThreadPool();

    priv->source    = new sp<MediaSource>();
    *priv->source   = new GtvSource(thiz, meta);
    priv->in_queue  = new List<Frame*>;
    //priv->out_queue = new List<Frame*>;
    priv->client    = new OMXClient;
	
	priv->surface = new sp<Surface>();
	
	if (priv->client->connect() !=  OK) {
        LOGE("Cannot connect OMX client");
		return NULL;
    }
	
	priv->decoder  = new sp<MediaSource>();
	
//	*priv->decoder = OMXCodec::Create(
//		*priv->source,
//		NULL, 0, NULL);
	
	priv->renderer = new sp<ANativeWindow>();
	
    return thiz;
}


static void releaseBufferIfNonNULL(MediaBuffer **buffer)
{
    if (*buffer)
    {
        (*buffer)->release();
        *buffer = NULL;
    }
}

void sendFrameToISurface(MediaBuffer *buffer)
{
	//int32_t err = mVideoRenderer->queueBuffer(
	//					mVideoRenderer.get(), buffer->graphicBuffer().get());
	
	LOGE("sendFrameToISurface");
	int32_t err = (*g_dec->renderer)->queueBuffer(
					(g_dec->renderer)->get(), buffer->graphicBuffer().get());
	
	if (err != 0) 
	{
		LOGE("queueBuffer failed with error %s (%d)", strerror(-err),-err);
		return;
	}
	sp<MetaData> metaData = buffer->meta_data();
	metaData->setInt32(kKeyRendered, 1);
	
}

void displayOrDiscardFrame(MediaBuffer **lastBuffer, MediaBuffer
    *buffer, int64_t pts_us)
{
	LOGE("displayOrDiscardFrame buffer != NULL");
    if (buffer == NULL)
    {
    	LOGE("displayOrDiscardFrame buffer == NULL");
		return;		 
    }
    //if (mVideoRenderer.get() != NULL)
	LOGE("displayOrDiscardFrame buffer != NULL");
	if((g_dec->renderer)->get() != NULL)
    {
        sendFrameToISurface(buffer);
    }
    
	releaseBufferIfNonNULL(lastBuffer);   
	*lastBuffer = buffer;
   
 
}



void* decode_thread(void *arg){

	VDCtx *avctx = (VDCtx*)arg;
    GtvDecoderPriv *s = (GtvDecoderPriv*)avctx->priv;
    Frame* frame;
    MediaBuffer *buffer;
	MediaBuffer *lastBuffer = NULL;
    int decode_done = 0;
    do {
        buffer = NULL;
        frame = (Frame*)malloc(sizeof(Frame));
        {
			LOGE("(*s->decoder)->read(&buffer) enter");
            frame->status = (*s->decoder)->read(&buffer); //!< decode
			LOGE("(*s->decoder)->read(&buffer) exit");
            if (frame->status == OK) {
                sp<MetaData> outFormat = (*s->decoder)->getFormat();
                outFormat->findInt32(kKeyWidth , &frame->w);
                outFormat->findInt32(kKeyHeight, &frame->h);
                frame->size    = buffer->range_length();
                frame->mbuffer = buffer;
				LOGE("(*s->decoder)->read(&buffer) ok ");
            } else if (frame->status == INFO_FORMAT_CHANGED) {
			LOGE("(*s->decoder)->read(&buffer) INFO_FORMAT_CHANGED");
                if (buffer)
                    buffer->release();
                free(frame);
                continue;
            } else {
			LOGE("(*s->decoder)->read(&buffer) done");
                decode_done = 1;
            }
        }
        
		displayOrDiscardFrame(&lastBuffer, buffer, 0); //!< render the frame
		
    } while (!decode_done && !s->stop_decode);

    s->thread_exited = true;

    return 0;
}

void init_render(GtvDecoderPriv* gtvp){
LOGE("init_render 1");
	if (*gtvp->source == NULL) 
	{        
		return;    
	}    
	LOGE("init_render 2");
	sp<MetaData> meta = (*gtvp->source)->getFormat(); 
LOGE("init_render 3");	
	int32_t format;    
	const char *component;    
	int32_t decodedWidth, decodedHeight;    
	//HECK(meta->findInt32(kKeyColorFormat, &format));    
	//CHECK(meta->findCString(kKeyDecoderComponent, &component));    
	CHECK(meta->findInt32(kKeyWidth, &decodedWidth));    
	CHECK(meta->findInt32(kKeyHeight, &decodedHeight));    
	
	LOGE("init_render 4" + decodedWidth + decodedHeight);    
	IPCThreadState::self()->flushCommands();    
	
	LOGE("init_render 5");
	*gtvp->renderer=NULL;
	
LOGE("init_render 6");
	if ((*gtvp->surface).get() != NULL){
		*gtvp->renderer = (sp<ANativeWindow> )*gtvp->surface;
		//mVideoRenderer=mSurface;
LOGE("init_render 7");
	}
}

void init_decoder(GtvDecoderPriv* gtvp){

	
	LOGE("init_decoder 1");

	*gtvp->decoder = OMXCodec::Create(
		gtvp->client->interface(), (*gtvp->source)->getFormat(), false,
		*gtvp->source,
		NULL, 0, *gtvp->surface);
	LOGE("init_decoder 2");
	if ((*gtvp->decoder)->start() !=  OK) {
        LOGE("Cannot start decoder\n");
        gtvp->client->disconnect();
		return;
    }
	LOGE("init_decoder 3");
	if( (*gtvp->surface).get() != NULL)
	{
		init_render(gtvp); //!< create render
	}
}





UMSint um_vdec_decode(VDCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	GtvDecoderPriv* priv = NULL;
	int ret = UMFailure;
	int pts= 0;

	if(!thiz || !buf || bufLen<=0) 
		return UMFailure;
	
	LOGE("um_vdec_decode 1");
	priv = (GtvDecoderPriv*)thiz->priv;
		LOGE("um_vdec_decode 1 1");
	g_dec = priv;
		LOGE("um_vdec_decode 1 2");
	
	if(priv->decoder_start == false){
		LOGE("um_vdec_decode 1 3");
		init_decoder(priv); //!< 1
		priv->decoder_start = true;
	}
	LOGE("um_vdec_decode 2");
	
	if (!priv->thread_started) {
        pthread_create(&priv->decode_thread_id, NULL, &decode_thread, thiz); //!< 2
        priv->thread_started = true;
    }
	LOGE("um_vdec_decode 3");
	
	Frame* frame = (Frame*)malloc(sizeof(Frame));
	frame->status  = OK;
    frame->size    = bufLen;
			
	frame->buffer  = (uint8_t*)malloc(bufLen);
	memcpy(frame->buffer, buf, bufLen);
	
	LOGE("um_vdec_decode 4");
	while (true) {
		if (priv->thread_exited) {
			priv->source_done = true;
			break;
		}
		pthread_mutex_lock(&priv->in_mutex);
		if (priv->in_queue->size() >= 10) {
			pthread_mutex_unlock(&priv->in_mutex);
			usleep(10000);
			continue;
		}
		priv->in_queue->push_back(frame); 
		pthread_cond_signal(&priv->condition); //!< input data
		pthread_mutex_unlock(&priv->in_mutex);
		break;
    }
	LOGE("um_vdec_decode 5");
	free(frame);
	
	return ret;
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

UMSint um_vdec_destroy(VDCtx* thiz)
{


	return UMSuccess;
}

void um_vdec_stop(VDCtx* thiz)
{

}

UMSint um_vdec_reset(VDCtx* thiz)
{


	return UMSuccess;
}

void um_vdec_setCompensateNum(VDCtx* thiz, int frameNumbers)
{
	
}