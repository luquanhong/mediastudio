/*!
 * \file	FILE NAME: UM_MediaPlayer.cpp
 *
 * Version: 1.0,  Date: 2010-11-02
 *
 * Description: ubvideo player imply
 *
 * Platform:
 *
 */

 
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
#include "um_sys_log.h"
#include "UM_MediaPlayer.h"

#define LOG_TAG "ummediaplayer"


using namespace android;

/*!
*	\brief 	Release the input buffer memory 
*	\param 	The target buffer that will release.
*/
static void releaseBufferIfNonNULL(MediaBuffer **buffer)
{
    if (*buffer)
    {
        (*buffer)->release();
        *buffer = NULL;
    }
}

/*!
*	\brief 	Debug function that write input information to file.
*/
void printPlayerlogToFile(char* msg)
{
	FILE *fd=NULL;
	fd = fopen("/data/data/com.ubiLive.GameCloud/ubitus/ubplayer_log.txt", "a");
	if(fd!=NULL)
	{
		fwrite(msg,sizeof( char),strlen(msg),fd);
		fclose(fd); 
	}
}


/*!
*	\brief 	Constructor function of the UMMediaPlayer 
*	\param	codectype	The decode type 
						1: MPEG4
						2: H264
	\param	width	The video width
	\param	height	The video height
*/
UMMediaPlayer::UMMediaPlayer(int codectype, int width, int height): 
			mInitCheck(NO_INIT),
		 	codec_type(0), 
			mVideoWidth(0), 
			mVideoHeight(0), 
			mVideoPosition(0), 
			mPlaying(false)
{
	
    codec_type = codectype;
    mVideoWidth = width;
    mVideoHeight = height;
    status_t err = mClient.connect();
    if (err != OK)
    {
        UMLOG_ERR("Failed to connect to OMXClient.");
        return ;
    }
	
	mVideoRenderer=NULL;
	mVideoSource=NULL;
	mVideoDecoder=NULL;
	UMLOG_ERR("UM_MediaPlayer()-------UMMediaSource start");
	mVideoSource = new UMMediaSource(codec_type,mVideoWidth,mVideoHeight);
}

/*!
*	\brief 	Destructor function of the UMMediaPlayer 
*/
UMMediaPlayer::~UMMediaPlayer()
{
}

/*!
*	\brief 	Release the device instance 
*/
void UMMediaPlayer::release()
{
	stop();

	mVideoDecoder.clear();
	mSurface.clear();
	setSurface(NULL);
	IPCThreadState::self()->flushCommands();
	mClient.disconnect();
	UMLOG_ERR("UM_MediaPlayer()-------release end");
}

/*!
*	\brief 	The main method that used to decode data.
*			When the first invoke, it will start a decode thread.
*			The thread is use to read data from decode,and put them to render.
*/
void UMMediaPlayer::play()
{

	if(mInitCheck != OK)
		return;
		
    if (mPlaying)
    {
        return ;
    }
    mPlaying = true;
	compenstatecount=0;
	frameindex=0;
	frameoutindex=0;

    if (mVideoDecoder != NULL)
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        pthread_create(&mVideoThread, &attr, videoWrapper, this);

        pthread_attr_destroy(&attr);
    }
}

/*!
*	\brief Stop the decoder and delete the relationship instance 
*/
void UMMediaPlayer::stop()
{
	UMLOG_ERR("UMMediaPlayer::stop begin ");	
	Mutex::Autolock autoLock(mLock);

	if (!mPlaying)
	{
		return ;
    }
	mPlaying = false;	
	mVideoSource->stop();
	
	if (mVideoDecoder != NULL)
	{
    		UMLOG_ERR("UM_MediaPlayer()-------stop 3 wait mVideoThread");
        	void *dummy;
        	pthread_join(mVideoThread, &dummy);	//!< stop the decode thread, and release the thread resource.
    }
	usleep(200*1000);
	
}

/*!
*	\brief	Prepare render for decoder, it will be invoke whill decoder create later.
*/
void UMMediaPlayer::initRenderer() 
{  

	if (mVideoSource == NULL) 
	{        
		return;    
	}    
	sp<MetaData> meta = mVideoDecoder->getFormat();    
	int32_t format;    
	const char *component;    
	int32_t decodedWidth, decodedHeight;    
	CHECK(meta->findInt32(kKeyColorFormat, &format));    
	CHECK(meta->findCString(kKeyDecoderComponent, &component));    
	CHECK(meta->findInt32(kKeyWidth, &decodedWidth));    
	CHECK(meta->findInt32(kKeyHeight, &decodedHeight));    
	
	    
	IPCThreadState::self()->flushCommands();    
	
	mVideoRenderer=NULL;
	

	if (mSurface.get() != NULL)
		mVideoRenderer=mSurface;

}


void *UMMediaPlayer::videoWrapper(void *me)
{
    ((UMMediaPlayer*)me)->videoEntry();
	
    return NULL;
}

/*!
*	\brief	The decode thread function. 
*			The main Loop is used to read data from decoder,and put them to render. 
*/
void UMMediaPlayer::videoEntry()
{
	
    bool eof = false;
    MediaBuffer *lastBuffer = NULL;
    mVideoStartTime = 0;
    
    UMLOG_ERR("videoEntry() ---- mVideoDecoder->start() begin");
    status_t err = mVideoDecoder->start();

    if(err != OK)
    {
   		UMLOG_ERR("videoEntry() ---- mVideoDecoder->start() end failed err=%d", err);
		return;
    }
    
    while (mPlaying)
    {
        MediaBuffer *buffer;
        MediaSource::ReadOptions options;
	
		status_t err = mVideoDecoder->read(&buffer, &options);
		options.clearSeekTo();	
		//LOGE("videoEntry() mVideoDecoder->read buffer data result err=%d buffer=%p", err,buffer);	

		if(err == -3)
        {
        	UMLOG_ERR("videoEntry() ----err ---");
			// continue;
   			mVideoSource->stop();
   			releaseBufferIfNonNULL(&lastBuffer);
   			mVideoDecoder->stop();
			setSurface(NULL);
   			mInitCheck=NO_INIT;
   			mClient.disconnect();
   			mClient.connect();
   			mPlaying=false;
            return;
        }
		
	    if (buffer == NULL)
	    {
			usleep(5000);
			eof = true;		
			continue;
	    }
	    CHECK((err == OK && buffer != NULL) || (err != OK && buffer == NULL));
	    if (err != OK)
	    {
	        eof = true;
	        mPlaying=false;
	        continue;
	    }
	    if (buffer->range_length() == 0)
	    {
			unsigned int index=0;
			index=frameoutindex%128;
			frameindexbuf[index]=0;
			if(compenstatecount>0)
			{
				int count=frameindex-frameoutindex;
				if(count>0 && count <compenstatecount)
					compenstatecount=count;
			}
			else
				compenstatecount=frameindex-frameoutindex;
			frameoutindex++;
			if(frameoutindex>65535)
				frameoutindex=0;
				
			buffer->release();
			buffer=NULL;
				continue;  
	    }

////////////////////////wait for later		
		unsigned int index=frameoutindex%128;
		if(compenstatecount>0)
		{
			int count=frameindex-frameoutindex;
			if(count>0 && count <compenstatecount)
				compenstatecount=count;
		}
		else
			compenstatecount=frameindex-frameoutindex;

		frameoutindex++;
		if(frameoutindex>65535)
			frameoutindex=0;

		
		if(frameindexbuf[index]<0)
		{
			frameindexbuf[index]=0;
			buffer->release(); 
			buffer=NULL;
			continue;
		}
			
		//UMLOG_ERR("displayOrDiscardFrame--------------");
		displayOrDiscardFrame(&lastBuffer, buffer, 0);
	
    }
	releaseBufferIfNonNULL(&lastBuffer);
	UMLOG_ERR("========================mVideoDecoder->stop();");
    mVideoDecoder->stop();
}

/*!
*	\brief	Pass the decoded data to the upper surface 
*/
void UMMediaPlayer::displayOrDiscardFrame(MediaBuffer **lastBuffer, MediaBuffer
    *buffer, int64_t pts_us)
{
	
    if (buffer == NULL)
    {
    	 UMLOG_ERR("displayOrDiscardFrame buffer == NULL");
		return;		 
    }
    if (mVideoRenderer.get() != NULL)
    {
        sendFrameToISurface(buffer);
    }
    
	releaseBufferIfNonNULL(lastBuffer);   
	*lastBuffer = buffer;
   
 
}

/*!
*	\brief	Render the data that have been decoded.
*/
void UMMediaPlayer::sendFrameToISurface(MediaBuffer *buffer)
{

	status_t err = mVideoRenderer->queueBuffer(
						mVideoRenderer.get(), buffer->graphicBuffer().get());
	
	if (err != 0) 
	{
		UMLOG_ERR("queueBuffer failed with error %s (%d)", strerror(-err),-err);
		return;
	}
	sp<MetaData> metaData = buffer->meta_data();
	metaData->setInt32(kKeyRendered, 1);
	

}

//static int testFrameCount = 0;
//char H264BlankFrame[] = {0x00, 0x00, 0x01, 0x0A};
// 0x00, 0x00, 0x01, 0x0A maybe
// 0x00, 0x00, 0x01, 0x2A maybe
// 0x00, 0x00, 0x01, 0x6A can't work
/*!
*	\brief 	Input the date that wanted to decode. 
*			when the first invoke, it will create decoder according to the buf param. 
*/
int UMMediaPlayer::setInputData(char *buf, int len, int pts)
{

	int ret=0;
   
    if(len<8)
        return 1;

    unsigned int index=frameindex%128;
    if(pts>0)
    {
		frameindexbuf[index]=pts;
    }
    else
    {
        frameindexbuf[index]=0;
    }
	
    frameindex++;

	if(frameindex>65535)
        frameindex=0;

    ret=mVideoSource->SetInputData(buf, len, pts);

	if(mInitCheck == NO_INIT)
	{
		initDecode();
	}
	
    return ret;

}

/*!
*	\brief	It will be invoke when the first input decode data.
*/
void UMMediaPlayer::initDecode()
{
	if(mInitCheck == OK)
		return;
	
	mInitCheck = OK;
	setVideoDecoder(mVideoSource);

}

/*!
*	\brief	It will be invoke when the first input decode data.
*/
void UMMediaPlayer::setVideoDecoder(const sp <UMMediaSource >  &source)
{


	UMLOG_ERR("setVideoSource entry point");
	Mutex::Autolock autoLock(mLock);
	mVideoSource = source;

	UMLOG_ERR("setVideoSource source->getFormat();");
	sp < MetaData > meta = source->getFormat();

	bool success = meta->findInt32(kKeyWidth, &mVideoWidth);
	CHECK(success);

	success = meta->findInt32(kKeyHeight, &mVideoHeight);
	CHECK(success);
	UMLOG_ERR("setVideoSource width=%d,height=%d", mVideoWidth, mVideoHeight);


	 mVideoDecoder = OMXCodec::Create(
            mClient.interface(), meta, false, //!< using the stagefright's OMXIL
            source,
            NULL, 0, mSurface);


	if (mVideoDecoder == NULL)
	{
		UMLOG_ERR("UM_MediaPlayer::setVideoSource is NULL");
		mInitCheck = NO_INIT;
		return ;
	}
	UMLOG_ERR("setVideoSource OMXCodec::Create successful!");
	
	if( mSurface.get() != NULL)
	{
		initRenderer();
	}

}



/*!
*	\brief	Set the surface 
*	\param	surface	The surface Passed from upper 
*/
void UMMediaPlayer::setSurface(const sp <Surface>  &surface)
{
	UMLOG_ERR("------------UM_MediaPlayer::setSurface");
	Mutex::Autolock autoLock(mLock);
	if (mSurface.get() != NULL && surface.get() != NULL)
    {

		UMLOG_ERR("-------------mSurface is not NULL");
		if(mSurface.get() != surface.get())
		{
			 finitRenderer();
			 mVideoRenderer=NULL;
			 mSurface=NULL;
			 mSurface = surface;
			 usleep(20*1000);
       		 initRenderer();
		}

    }
	else
	{
		finitRenderer();

		mSurface = surface;
	}
}

/*!
*	\brief	Judget if the decoder is running. 
*/
bool UMMediaPlayer::isPlaying()const
{
    return mPlaying;
}

status_t UMMediaPlayer::initCheck()const
{
    return mInitCheck;
}

/*!
*	\brief	Release the Render resource. 
*/
void UMMediaPlayer::finitRenderer()
{

}

void UMMediaPlayer::setCompensateNum(int number)
{
	if (mVideoSource != NULL)
		mVideoSource->setCompensateNum(number);
}

