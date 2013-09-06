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

using namespace android;

struct Frame{

	int status;
	size_t	 size;
	int64_t	 time;
	int 	 key;
	uint8_t	 *buffer;
};

struct TimeStamp{

	int64_t pts;
	int64_t	reordered_opaque;
};

class CustomSource;

struct StagefrightContext{

	sp<MediaSource>* source;
	List<Frame*> *in_queue;
	List<Frame*> *out_queue;

	pthread_mutex_t	in_mutex, out_mutex;
	pthread_cond_t  condition;
	pthread_t 		decode_thread_id;

	bool 	source_done;
	volatile sig_atomic_t thread_started, thread_exited, stop_decode;

	OMXClient* 	client;
	sp<MediaSource>* decoder;
	const char* decoder_component;

	sp<AwesomeRenderer>* mVideoRenderer;
    sp<ANativeWindow>* mNativeWindow;

    int32_t mVideoScalingMode;
};

struct AwesomeRenderer : public RefBase
{
    AwesomeRenderer() {}

    virtual void render(MediaBuffer *buffer) = 0;//!<	the pure virtual function for render

private:
    AwesomeRenderer(const AwesomeRenderer &);
    AwesomeRenderer &operator=(const AwesomeRenderer &);
};

int Stagefright_init(StagefrightContext *s, int width, int height);
int Stagefright_close(StagefrightContext *s);
int Stagefright_decode_frame(StagefrightContext *s, uint8_t* data, int data_size);

