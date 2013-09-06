#include <media/stagefright/MetaData.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/Utils.h>
#include <media/stagefright/OMXClient.h>
#include <media/stagefright/OMXCodec.h>
#include <utils/RefBase.h>
#include <binder/IPCThreadState.h>
#include <gui/Surface.h>
#include <utils/List.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include <map>
#include "test.h"
#include "libstagefright.h"

using namespace android;

struct AwesomeNativeWindowRenderer : public AwesomeRenderer
{
    AwesomeNativeWindowRenderer(const sp<ANativeWindow> &nativeWindow,
								int32_t rotationDegrees)
		: mNativeWindow(nativeWindow)
	{
        applyRotation(rotationDegrees);
    }

/*!
*	\brief 	The render method for hardware render.
*/
	virtual void render(MediaBuffer *buffer)
	{
		int64_t timeUs;
		CHECK(buffer->meta_data()->findInt64(kKeyTime, &timeUs));
		native_window_set_buffers_timestamp(mNativeWindow.get(), timeUs * 1000);

		if (buffer == NULL)
		{
			UMLOG_ERR("AwesomeNativeWindowRenderer buffer == NULL");
			return;
		}
		if (buffer->graphicBuffer() == NULL)
		{
			UMLOG_ERR("AwesomeNativeWindowRenderer buffer->graphicBuffer() == NULL");
			return;
		}

		if (buffer->graphicBuffer().get() == NULL)
		{
			UMLOG_ERR("AwesomeNativeWindowRenderer buffer->graphicBuffer().get() == NULL");
			return;
		}
		//UMLOG_ERR("mNativeWindow queueBuffer  buffer->range_length()=%d", buffer->range_length());
		status_t err = mNativeWindow->queueBuffer(
						mNativeWindow.get(), buffer->graphicBuffer().get());
		if (err != 0)
		{
			UMLOG_ERR("queueBuffer failed with error %s (%d)", strerror(-err),-err);
			return;
		}

		sp<MetaData> metaData = buffer->meta_data();
		metaData->setInt32(kKeyRendered, 1);
	}

protected:
    virtual ~AwesomeNativeWindowRenderer() {}

private:
    sp<ANativeWindow> mNativeWindow;

    void applyRotation(int32_t rotationDegrees)
	{
        uint32_t transform;
        switch (rotationDegrees)
		{
            case 0: transform = 0; break;
            case 90: transform = HAL_TRANSFORM_ROT_90; break;
            case 180: transform = HAL_TRANSFORM_ROT_180; break;
            case 270: transform = HAL_TRANSFORM_ROT_270; break;
            default: transform = 0; break;
        }

        if (transform)
		{
            CHECK_EQ(0, native_window_set_buffers_transform(mNativeWindow.get(), transform));
        }
    }

    AwesomeNativeWindowRenderer(const AwesomeNativeWindowRenderer &);
    AwesomeNativeWindowRenderer &operator=(
            const AwesomeNativeWindowRenderer &);
};


struct AwesomeLocalRenderer : public AwesomeRenderer
{
    AwesomeLocalRenderer(
            const sp<ANativeWindow> &nativeWindow, const sp<MetaData> &meta)
        : mTarget(new SoftwareRenderer(nativeWindow, meta)) {}

/*!
*	\brief 	The render method for software render.it factually invoke the input render target to render
*/
    virtual void render(MediaBuffer *buffer)
	{
		UMLOG_ERR("AwesomeLocalRenderer buffer->range_offset()=%d, size=%d", buffer->range_offset(), buffer->range_length());
        render((const uint8_t *)buffer->data() + buffer->range_offset(),
               buffer->range_length());
    }

    void render(const void *data, size_t size)
	{
        mTarget->render(data, size, NULL);
    }

protected:
    virtual ~AwesomeLocalRenderer()
	{
        delete mTarget;
        mTarget = NULL;
    }

private:
    SoftwareRenderer *mTarget;

    AwesomeLocalRenderer(const AwesomeLocalRenderer &);
    AwesomeLocalRenderer &operator=(const AwesomeLocalRenderer &);;
};



class CustomSource : public MediaSource{

public:
	CustomSource(StagefrightContext* context,int width, int height, sp<MetaData> meta){

		s = context;
		source_meta = meta;
		frame_size = (width * height)/2;
		buf_group.add_buffer( new MediaBuffer(frame_size) );
	}

	~CustomSource(){}

	virtual sp<MetaData> getFormat(){

		return source_meta;
	}

	virtual int start(MetaData* params){

		return OK;
	}

	virtual int stop(){

		return OK;
	}

	virtual int read(MediaBuffer **buffer, const MediaSource::ReadOptions *options){

		Frame* frame;
		int ret;

		if(s->thread_exited)
			return -1;

		pthread_mutex_lock(&s->in_mutex);

		while( s->in_queue->empty())
			pthread_cond_wait(&s->condition, &s->in_mutex);

		LOGE("Stagefright_decode_frame begin");
		frame = *s->in_queue->begin();
		ret = frame->status;

		if(ret == OK){
			LOGE("Stagefright_decode_frame begin 1");
			ret = buf_group.acquire_buffer(buffer);
			if(ret == OK){
				LOGE("Stagefright_decode_frame begin 2");
				memcpy((*buffer)->data(), frame->buffer, frame->size);
				(*buffer)->set_range(0, frame->size);
				(*buffer)->meta_data()->clear();
				(*buffer)->meta_data()->setInt32(kKeyIsSyncFrame, frame->key );
				(*buffer)->meta_data()->setInt64(kKeyTime, frame->time);
				LOGE("Stagefright_decode_frame begin 3");
			} else {

				LOGE("Failed to acquire MediaBuffer");
			}
			free(frame->buffer);
			frame->buffer = NULL;
		}

		LOGE("Stagefright_decode_frame begin 4");
		s->in_queue->erase(s->in_queue->begin() );

		pthread_mutex_unlock(&s->in_mutex);

		free(frame);
		frame = NULL;
		LOGE("Stagefright_decode_frame begin 5");
		return ret;
	}

private:
	MediaBufferGroup	buf_group;
	sp<MetaData>		source_meta;
	int frame_size;
	StagefrightContext*	s;
};



void* decode_thread(void *arg)
{
	StagefrightContext *s = (StagefrightContext*)arg;

    //const AVPixFmtDescriptor *pix_desc = &av_pix_fmt_descriptors[avctx->pix_fmt];

	Frame* frame;
    MediaBuffer *buffer;
    int32_t w, h;
    int decode_done = 0;
    int ret;
    int src_linesize[3];
    const uint8_t *src_data[3];
    int64_t out_frame_index = 0;

    do {
        buffer = NULL;
        frame = (Frame*)malloc(sizeof(Frame));

        if (!frame) {
            frame->status = -2;
            decode_done   = 1;
            goto push_frame;
        }
        frame->status = (*s->decoder)->read(&buffer);

        LOGE("Stagefright_decode_frame begin 6");
        if (frame->status == OK) {
            sp<MetaData> outFormat = (*s->decoder)->getFormat();
            LOGE("Stagefright_decode_frame begin 7");
            outFormat->findInt32(kKeyWidth , &w);
            outFormat->findInt32(kKeyHeight, &h);

            LOGE("Stagefright_decode_frame begin 8");
            buffer->meta_data()->findInt64(kKeyTime, &out_frame_index);
            LOGE("Stagefright_decode_frame begin 9");
            buffer->release();
		} else if (frame->status == INFO_FORMAT_CHANGED) {
			 LOGE("Stagefright_decode_frame begin  7.1");
			if (buffer)
				buffer->release();
			free(frame);
			continue;
		} else {
			 LOGE("Stagefright_decode_frame begin  7.2");
			decode_done = 1;
		}

push_frame:
        while (true) {
            pthread_mutex_lock(&s->out_mutex);
            if (s->out_queue->size() >= 10) {
                pthread_mutex_unlock(&s->out_mutex);
                usleep(10000);
                continue;
            }
            break;
        }
        //s->out_queue->push_back(frame);
        LOGE("Stagefright_decode_frame end");
        pthread_mutex_unlock(&s->out_mutex);
    } while (!decode_done && !s->stop_decode);

    s->thread_exited = true;

    return 0;
}



int Stagefright_decode_frame(StagefrightContext *s, uint8_t* data, int data_size){

	Frame* frame;
	int status;

	LOGE("Stagefright_decode_frame");
	if (!s->thread_started) {
		pthread_create(&s->decode_thread_id, NULL, &decode_thread, s);
		s->thread_started = true;
	}


    if (!s->source_done) {


        frame = (Frame*)malloc(sizeof(Frame));
        if (data) {
            frame->status  = OK;
            frame->size    = data_size;
            //frame->key     = avpkt->flags & AV_PKT_FLAG_KEY ? 1 : 0;
            frame->buffer  = (uint8_t*)malloc(data_size);
            if (!frame->buffer) {
                free(frame);
                return -1;
            }

            uint8_t *ptr = data;

            memcpy(frame->buffer, ptr, data_size);

        } else {
            frame->status  = -1;
            s->source_done = true;
        }

        while (true) {

        	if (s->thread_exited) {
                s->source_done = true;
                break;
            }
            pthread_mutex_lock(&s->in_mutex);

            if (s->in_queue->size() >= 10) {

            	pthread_mutex_unlock(&s->in_mutex);
                usleep(10000);
                continue;
            }
            LOGE("Stagefright_decode_frame push_back");
            s->in_queue->push_back(frame);
            pthread_cond_signal(&s->condition);
            pthread_mutex_unlock(&s->in_mutex);
            break;
        }
    }

    LOGE("Stagefright_decode_frame push_back 1");
    //frame = *s->out_queue->begin();
    LOGE("Stagefright_decode_frame push_back 2");
	//s->out_queue->erase(s->out_queue->begin());
	LOGE("Stagefright_decode_frame push_back 3");
	//pthread_mutex_unlock(&s->in_mutex);


	status  = frame->status;
	//free(frame);

	/*
	if (status == -1)
	   return 0;

	if (status != OK) {

		LOGE( "Decode failed: %x\n", status);
	   return -1;
	}
	*/
	LOGE("Stagefright_decode_frame push_back 4");
	return 1;

}

int setVideoScalingMode(int32_t mode)
{
	mVideoScalingMode = mode;

	if(mNativeWindow != NULL)
	{
		int err = native_window_set_scaling_mode( mNativeWindow.get(),
				mVideoScalingMode);
		if(err != OK)
		{
			LOGE("Failed to set scaling mode: %d", err);
		}
	}
	return OK;
}

void initRenderer(){


	sp<MetaData> meta = mVideoDecoder->getFormat();
	int32_t format;
	const char *component;
	int32_t decodedWidth, decodedHeight;
	CHECK(meta->findInt32(kKeyColorFormat, &format));
	CHECK(meta->findCString(kKeyDecoderComponent, &component));
	CHECK(meta->findInt32(kKeyWidth, &decodedWidth));
	CHECK(meta->findInt32(kKeyHeight, &decodedHeight));
	int32_t rotationDegrees;

	if (!mVideoSource->getFormat()->findInt32(
		kKeyRotation, &rotationDegrees))
	{
		rotationDegrees = 0;
	}

	mVideoRenderer.clear();
	// Must ensure that mVideoRenderer's destructor is actually executed
	// before creating a new one.
	IPCThreadState::self()->flushCommands();

	// Even if set scaling mode fails, we will continue anyway
	setVideoScalingMode(mVideoScalingMode); 	//!< add at version 4.1

	if (!strncmp(component, "OMX.", 4)
		&& strncmp(component, "OMX.google.", 11)
		&& strcmp(component, "OMX.Nvidia.mpeg2v.decode"))
	{
		// Hardware decoders avoid the CPU color conversion by decoding
		// directly to ANativeBuffers, so we must use a renderer that
		// just pushes those buffers to the ANativeWindow.
		mVideoRenderer =
				new AwesomeNativeWindowRenderer(mNativeWindow, rotationDegrees);
	} else {
		// Other decoders are instantiated locally and as a consequence
		// allocate their buffers in local address space.  This renderer
		// then performs a color conversion and copy to get the data
		// into the ANativeBuffer.
		mVideoRenderer = new AwesomeLocalRenderer(mNativeWindow, meta);
	}
}

int Stagefright_init(StagefrightContext *s, int width, int height, const sp<ANativeWindow> &surface){

	int ret;
	int32_t colorFormat = 0;
	sp<MetaData> meta, outFormat;
	//StagefrightContext *s =  (struct StagefrightContex *)malloc(sizeof(struct StagefrightContex));

	LOGE("Stagefright_init");
	meta = new MetaData;
	if(meta == NULL){
		LOGE("meta is fail.");
		return -1;
	}

	meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_AVC);
	meta->setInt32(kKeyWidth, width);
	meta->setInt32(kKeyHeight, height);
	//meta->setData(kKeyAVCC, kTypeAVCC, NULL, 0);

	//android::ProcessState::self()->startThreadPool();
	s->native_window = &surface;
	s->source 		= new sp<MediaSource>();
	*s->source 		= new CustomSource(s, width, height, meta);
	s->in_queue 	= new List<Frame*>;
	s->out_queue 	= new List<Frame*>;
	s->client		= new OMXClient;

	if(s->client->connect() != OK){

		LOGE("Cannot connect OMX client");
		return -1;
	}

	s->decoder = new sp<MediaSource>();
	*s->decoder = OMXCodec::Create(s->client->interface(), meta, false,
			*s->source, NULL, 0, *s->native_window);

	if( (*s->decoder)->start() != OK){

		LOGE("cannot start decoder.");
		s->client->disconnect();
		return -1;
	}

	outFormat = (*s->decoder)->getFormat();
	outFormat->findInt32(kKeyColorFormat, &colorFormat);

	if (colorFormat == OMX_QCOM_COLOR_FormatYVU420SemiPlanar ||
		colorFormat == OMX_COLOR_FormatYUV420SemiPlanar)
		LOGE("colorFormat is PIX_FMT_NV21");
	else if (colorFormat == OMX_COLOR_FormatYCbYCr)
		LOGE("colorFormat is PIX_FMT_YUYV422");
	else if (colorFormat == OMX_COLOR_FormatCbYCrY)
		LOGE("colorFormat is PIX_FMT_UYVY422");
	else
		LOGE("colorFormat is PIX_FMT_YUV420P");

	outFormat->findCString(kKeyDecoderComponent, &s->decoder_component);
	if(s->decoder_component)
		LOGE("decoder_component is %s", s->decoder_component);

	pthread_mutex_init(&s->in_mutex, NULL);
	pthread_mutex_init(&s->out_mutex, NULL);
	pthread_cond_init(&s->condition, NULL);
	return 0;

}

int Stagefright_close(StagefrightContext *s){

	Frame *frame;

	while(!s->in_queue->empty()){

		frame = *s->in_queue->begin();
		s->in_queue->erase(s->in_queue->begin());
	}


	(*s->decoder)->stop();
	s->client->disconnect();

	delete s->in_queue;
	delete s->client;
	delete s->decoder;
	delete s->source;

	pthread_mutex_destroy(&s->in_mutex);
	pthread_mutex_destroy(&s->out_mutex);
	pthread_cond_destroy(&s->condition);
}





















