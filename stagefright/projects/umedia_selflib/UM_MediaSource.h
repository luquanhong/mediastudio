/*!
 *	\file FILE NAME: mpeg4source.h
 *
 * Version: 1.0,  Date: 2010-11-02
 *
 * Description: mpeg4 container soure data
 *
 * Platform: 
 *
 */
#ifndef UMMEDIA_SOURCE_H_
#define UMMEDIA_SOURCE_H_
#include <utils/Log.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "um_circle_queue.h"

#include <media/stagefright/MediaSource.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaBufferGroup.h>

typedef enum
{
	CODEC_NONE,
	CODEC_MPEG4,
	CODEC_H264,
	CODEC_H263,
	CODEC_AAC,
	CODEC_AMR_NB,
	CODEC_AMR_WB
}codectype;

using namespace android;

class UMMediaSource : public MediaSource {
public:
    UMMediaSource(int codectype,int para1,int para2);
	virtual ~UMMediaSource();

    virtual status_t start(MetaData *params = NULL);
    virtual status_t stop();
    virtual sp<MetaData> getFormat();
    status_t release_mBuffer();
    virtual status_t read(MediaBuffer **buffer, const ReadOptions *options = NULL);
    int SetInputData(char* buf, int len, int pts);
    void setCompensateNum(int frameNumber);
	
protected:
  
	
private:
    void GetInputData();
    status_t   getinputframempeg4(MediaBuffer **buffer, const ReadOptions *options);
    status_t   getInputframeavc(MediaBuffer **buffer, const ReadOptions *options);
    status_t   Getinputframeh263(MediaBuffer **buffer, const ReadOptions *options);


private:
    bool 	mStarted;
    bool	mHeaderSent;

    MediaBufferGroup *mGroup;
	sp < MetaData > mMeta; 
    MediaBuffer 	 *mBuffer;

    int			mpara1;
    int			mpara2;
    int			mSize;
    codectype 	codec_type;

    //the circle queue
    void* pCQ; //!<	the circle queue for media frames
    pUmCQEntry  pCQEntry;
	
    long		timepos;
    int64_t		mcurrenttimeus;
	
    int mCodecSpecificDataSize;
    uint8_t*	mCodecSpecificData;
    int mCompensateNumber;
    int mCurrentNeedCompensateCount;
    bool mHandleFirstframe;	

    UMMediaSource(const UMMediaSource &);
    UMMediaSource &operator=(const UMMediaSource &);
	void setMetaData();

};


#endif //UBMEDIA_SOURCE_H_
