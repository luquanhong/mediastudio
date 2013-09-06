/* FILE NAME: ub_circle_queue.h
 *
 * Version: 1.0,  Date: 2011-2-11
 *
 * Description: 
 *
 * Platform: 
 *
 ***************************************************************************/
#ifndef _UM_CIRCLE_QUEUE_HEADER
#define _UM_CIRCLE_QUEUE_HEADER
#include "um_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _UmCQEntry
{
	char* dataHeader;
	
	int offset;//the offset of valid dataHeader
	int dataLen;
	int bufferSize;
	unsigned int pts;
}
UmCQEntry, *pUmCQEntry;


/** the handle general definitions */
typedef UMHandle UmMutexHandle;

#define UM_MUTEX_OK       0
#define UM_MUTEX_FAILURE  (-1)


/** @defgroup event wait definitions */
#define UM_EVENT_WAIT_INFINITE (-1)
#define UM_EVENT_WAIT_TIMEOUT  (-2)
#define UM_EVENT_WAIT_FAILURE  (-3)
#define UM_EVENT_WAIT_OK       0

/** the handle general definitions */
typedef UMHandle UmEventHandle;


UMSint  video_mutex_lock(UmMutexHandle mutexHandle);
UMSint  video_mutex_unlock(UmMutexHandle mutexHandle);
void video_mutex_destroy(UmMutexHandle mutexHandle);
UmMutexHandle video_mutex_create(void);
UMSint video_event_wait(UmEventHandle cond, UmMutexHandle mux, UMSint duration);
void video_event_notify(UmEventHandle condvarHandle);
void video_event_notifyAll(UmEventHandle condvarHandle);
UmEventHandle video_event_create(void);
void video_event_destroy(UmEventHandle condvarHandle);



void* um_cq_create(int len);
void  um_cq_destroy(void* pCQ);
void  um_cq_stop(void* pCQ);
void  um_cq_destroyEntry(pUmCQEntry pentry);

pUmCQEntry um_cq_pushEmptyEntry(void* pCQ, pUmCQEntry pentry);
pUmCQEntry um_cq_pushData(void* pCQ, char* data, int dataLen, unsigned int pts);
pUmCQEntry um_cq_popFilledEntry(void* pCQ);
pUmCQEntry um_cq_createEntry(void); //default buffer size is 32k
int um_cq_isFull(void* pCQ);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_UB_CIRCLE_QUEUE_HEADER */

