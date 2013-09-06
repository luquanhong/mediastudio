/***************************************************************************
 * FILE NAME: um_circle_queue.c
 *
 * Version: 1.0,  Date: 2011-2-27
 *
 * Description: 
 *
 * Platform: 
 *
 ***************************************************************************/

/***************************************************************************
 * Include Files                 	
 ***************************************************************************/
//#include "up_sys_config.h" 

//#include "utils/Log.h"
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "um_types.h"
#include "um_sys_mutex.h"
#include "um_sys_event.h"

#include "um_mm.h"
#include "um_circle_queue.h"




enum _umCQStatus
{
	UMCQ_STATUS_RUN  = 0,
	UMCQ_STATUS_WAIT = 1,
	UMCQ_STATUS_EXIT = 2
};

typedef struct __utilCircleListStr_
{
	//global
	unsigned int entry_number;
	unsigned int high_index;

	//circle index
	unsigned int circle_push_count;
	unsigned int circle_push_index;
	unsigned int circle_pop_count;
	unsigned int circle_pop_index;

	//resource pool
	unsigned int pool_push_count;
	unsigned int pool_push_index;
	unsigned int pool_pop_count;
	unsigned int pool_pop_index;

	//sync
	UmMutexHandle mutex;
	UmEventHandle event;
	int status; //0 == running, 1 == waiting, 2 == exited

	//circle queueList
	pUmCQEntry* queueList;

	//pool queueList
	pUmCQEntry* poolList;
}
*pUmCircleQueueStr, umCircleQueueStr;




/**
 * Lock a mutex
 */
UMSint  video_mutex_lock(UmMutexHandle mutexHandle)
{
    //JKLOG(JkTaskLockMutex,("lock mutex enter..."))
    if(mutexHandle == UM_NULL)
	{
        return UM_MUTEX_FAILURE;
    }

	if(0 == pthread_mutex_lock((pthread_mutex_t *)mutexHandle))
	{
		return UM_MUTEX_OK;
	}
	
	return UM_MUTEX_FAILURE;	
}

/**
 * Unlock a mutex
 */
UMSint  video_mutex_unlock(UmMutexHandle mutexHandle)
{
    if(mutexHandle == UM_NULL)
	{
        return UM_MUTEX_FAILURE;
    }

	if(0 == pthread_mutex_unlock((pthread_mutex_t *)mutexHandle))
	{
	    return UM_MUTEX_OK;
	}
	return UM_MUTEX_FAILURE;
}


/**
 * Destroy a mutex
 */
void video_mutex_destroy(UmMutexHandle mutexHandle)
{
    if(mutexHandle == UM_NULL)
	{
        return;
    }

	pthread_mutex_destroy((pthread_mutex_t *)mutexHandle);
	free((void*)mutexHandle);
}

/**
 * Create a mutex
 */
UmMutexHandle video_mutex_create(void)
{
	UmMutexHandle mutexHandle;
	mutexHandle = (UmMutexHandle)malloc(sizeof(pthread_mutex_t));

	if(mutexHandle == NULL)
	{
		return NULL;
	}

	if(pthread_mutex_init((pthread_mutex_t *)mutexHandle, NULL) == 0)
	{
		return mutexHandle;
	}
	else
	{
		free((void *)mutexHandle);
		return NULL;
	}
}

UMSint video_event_wait(UmEventHandle cond, UmMutexHandle mux, UMSint duration)
{
	int rc = 0;
	struct timespec timeout;
	struct timeval now;
	
    if(cond == UM_NULL || mux == UM_NULL)
	{
       	return UM_EVENT_WAIT_FAILURE;
	}
	
	if(duration == UM_EVENT_WAIT_INFINITE || duration < 0)
	{
		//wait forever
        rc = pthread_cond_wait((pthread_cond_t *)(cond), (pthread_mutex_t *)(mux));
	}
	else
	{
		//get the start time
		gettimeofday(&now,NULL);

		long seconds = duration/1000;
		long nanoseconds = (duration - seconds * 1000) * 1000000;
		
		//get the due time
		timeout.tv_sec   =   now.tv_sec + seconds;;
		timeout.tv_nsec =  now.tv_usec*1000 + nanoseconds;
	  	if (timeout.tv_nsec >= 1000000000) 
		{
			++timeout.tv_sec;
	  		timeout.tv_nsec -= 1000000000;
	  	}
	
		//wait begining
		rc = pthread_cond_timedwait((pthread_cond_t *)cond,(pthread_mutex_t *)mux, &timeout);
	}

	switch(rc)
	{
		case 0:
			return UM_EVENT_WAIT_OK;
		case ETIMEDOUT:
			return UM_EVENT_WAIT_TIMEOUT;
		case EINVAL:
			return UM_EVENT_WAIT_FAILURE;
		default:
			return UM_EVENT_WAIT_FAILURE;
	}
}

/** @brief notify the waiting event
  * @param cond
  * @return
  */
void video_event_notify(UmEventHandle condvarHandle)
{
    if(condvarHandle == UM_NULL)
	{
        return;
    }
	pthread_cond_signal((pthread_cond_t *)condvarHandle);
}


void video_event_notifyAll(UmEventHandle condvarHandle)
{
    if(condvarHandle == UM_NULL)
	{
        return;
    }
	pthread_cond_broadcast((pthread_cond_t *)condvarHandle);
}

UmEventHandle video_event_create(void)
{
	UmEventHandle condvarHandle;
	condvarHandle = (UmEventHandle)malloc(sizeof(pthread_cond_t));
	if(pthread_cond_init((pthread_cond_t *)condvarHandle, NULL)==0)
	{
		return condvarHandle;
	}
	else
	{
		return NULL;
	}
}

void video_event_destroy(UmEventHandle condvarHandle)
{
	void* conHandle = (void *)condvarHandle;

    if(condvarHandle == UM_NULL)
	{
        return;
    }

	pthread_cond_destroy((pthread_cond_t *)(condvarHandle));
	free(conHandle);
}

//////////////////////////////////////////////////////////////////
static pUmCQEntry m_um_cq_pushFilledEntry(void* pCQ, pUmCQEntry pentry)
{
	pUmCircleQueueStr m_gcs_cycle_list = (pUmCircleQueueStr)pCQ;

	if(!m_gcs_cycle_list) return UM_NULL;
	
	if(m_gcs_cycle_list->circle_push_count < m_gcs_cycle_list->entry_number + m_gcs_cycle_list->circle_pop_count)   
	{
		
		m_gcs_cycle_list->queueList[m_gcs_cycle_list->circle_push_index] = pentry;
		m_gcs_cycle_list->circle_push_count++;

		if(m_gcs_cycle_list->circle_push_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->circle_push_index = 0;
		else m_gcs_cycle_list->circle_push_index++;

		return pentry;
	}

	return UM_NULL;
}

static pUmCQEntry m_um_cq_popEmptyEntry(void* pCQ)
{
	pUmCircleQueueStr m_gcs_cycle_list = (pUmCircleQueueStr)pCQ;
	void* result = UM_NULL;
	
	if(m_gcs_cycle_list && m_gcs_cycle_list->pool_push_count != m_gcs_cycle_list->pool_pop_count)
	{
		result = m_gcs_cycle_list->poolList[m_gcs_cycle_list->pool_pop_index];
		m_gcs_cycle_list->pool_pop_count++;

		if(m_gcs_cycle_list->pool_pop_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->pool_pop_index = 0;   
		else m_gcs_cycle_list->pool_pop_index++;
		return (pUmCQEntry)result;
	}

	return UM_NULL;
}

//default buffer size is 32k
pUmCQEntry um_cq_createEntry(void)
{
	pUmCQEntry pentry  = (pUmCQEntry)memset((void*)malloc(sizeof(UmCQEntry)), 0, sizeof(UmCQEntry));
	pentry->bufferSize = 32*1024;;
	pentry->dataHeader = (char*)malloc(pentry->bufferSize);

	return pentry;
}

void um_cq_destroyEntry(pUmCQEntry pentry)
{
	if(pentry->dataHeader) free(pentry->dataHeader);
	free(pentry);
}

void* um_cq_create(int len)
{
	pUmCircleQueueStr m_gcs_cycle_list = UM_NULL;
	pUmCQEntry pentry;
	
	//allocate the circle list struct
	m_gcs_cycle_list = (pUmCircleQueueStr)memset(malloc(sizeof(umCircleQueueStr)),
		0, sizeof(umCircleQueueStr));

	//LOGE("[um_cq_create] Enter with %d \r\n", len);
	
	//len >= 3
	len = (len < 3 ? 3 : len);
	
	m_gcs_cycle_list->entry_number = len;
	m_gcs_cycle_list->high_index   = len -1;
	m_gcs_cycle_list->queueList = (pUmCQEntry*)malloc(len * sizeof(void*));
	m_gcs_cycle_list->poolList  = (pUmCQEntry*)malloc(len * sizeof(void*));
	
	//push the empty entries
	while(--len >= 0)
	{
		pentry = (pUmCQEntry)memset((void*)malloc(sizeof(UmCQEntry)), 0, sizeof(UmCQEntry));
		um_cq_pushEmptyEntry((void*)m_gcs_cycle_list, pentry);
	}

	//create the mutex then
	m_gcs_cycle_list->mutex  = video_mutex_create();
	m_gcs_cycle_list->event  = video_event_create();
	m_gcs_cycle_list->status = UMCQ_STATUS_RUN;
	
	return (void*)m_gcs_cycle_list;
}

void um_cq_destroy(void* pCQ)
{
	pUmCircleQueueStr m_gcs_cycle_list = (pUmCircleQueueStr)pCQ;
	int count;

	if(!pCQ) return;

	//LOGE("[um_cq_destroy] Enter\r\n");
	
	count = m_gcs_cycle_list->entry_number;
	
	if(m_gcs_cycle_list)
	{
		pUmCQEntry pentry;
		while(m_gcs_cycle_list->circle_push_count != m_gcs_cycle_list->circle_pop_count)
		{
			pentry = m_gcs_cycle_list->queueList[m_gcs_cycle_list->circle_pop_index];
			m_gcs_cycle_list->circle_pop_count++;

			if(m_gcs_cycle_list->circle_pop_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->circle_pop_index = 0;   
			else m_gcs_cycle_list->circle_pop_index++;

			if(pentry->dataHeader) free(pentry->dataHeader);
			free(pentry);
		}

		while(m_gcs_cycle_list->pool_push_count != m_gcs_cycle_list->pool_pop_count)
		{
			pentry = m_gcs_cycle_list->poolList[m_gcs_cycle_list->pool_pop_index];
			m_gcs_cycle_list->pool_pop_count++;

			if(m_gcs_cycle_list->pool_pop_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->pool_pop_index = 0;   
			else m_gcs_cycle_list->pool_pop_index++;

			if(pentry->dataHeader) free(pentry->dataHeader);
			free(pentry);
		}

		free(m_gcs_cycle_list->poolList);
		free(m_gcs_cycle_list->queueList);
		free(m_gcs_cycle_list);
	}
}

void um_cq_stop(void* pCQ)
{
	pUmCircleQueueStr m_gcs_cycle_list = (pUmCircleQueueStr)pCQ;

	//LOGE("[um_cq_stop] Enter\r\n");
	if(NULL != m_gcs_cycle_list)
	{
	
		video_mutex_lock(m_gcs_cycle_list->mutex);
	
		if(m_gcs_cycle_list->status == UMCQ_STATUS_WAIT)
		{
			m_gcs_cycle_list->status = UMCQ_STATUS_EXIT;
			video_event_notify(m_gcs_cycle_list->event);
		}
	
		m_gcs_cycle_list->status = UMCQ_STATUS_EXIT;

		//wait for a while to let other threads exit
		video_event_wait(m_gcs_cycle_list->event, m_gcs_cycle_list->mutex, 100);
		video_mutex_unlock(m_gcs_cycle_list->mutex);
	}
}

pUmCQEntry um_cq_pushEmptyEntry(void* pCQ, pUmCQEntry pentry)
{
	pUmCircleQueueStr m_gcs_cycle_list = (pUmCircleQueueStr)pCQ;

	if(!pCQ) return UM_NULL;
	
	while(m_gcs_cycle_list->status == UMCQ_STATUS_RUN && 
		m_gcs_cycle_list->pool_push_count >= m_gcs_cycle_list->entry_number + m_gcs_cycle_list->pool_pop_count)
	{
		video_mutex_lock(m_gcs_cycle_list->mutex);
		
		if(m_gcs_cycle_list->status == UMCQ_STATUS_RUN)
		{
			//block then
			m_gcs_cycle_list->status = UMCQ_STATUS_WAIT;
			//LOGE("um_cq_pushEmptyEntry, WAIT\r\n");
			video_event_wait(m_gcs_cycle_list->event, m_gcs_cycle_list->mutex, UM_EVENT_WAIT_INFINITE);
			//LOGE("um_cq_pushEmptyEntry, WAIT END\r\n");
		}
		else if(m_gcs_cycle_list->status != UMCQ_STATUS_WAIT)
		{
			video_mutex_unlock(m_gcs_cycle_list->mutex);
			return UM_NULL;
		}

		video_mutex_unlock(m_gcs_cycle_list->mutex);
	}

	//if(m_gcs_cycle_list->status != UMCQ_STATUS_RUN) return UM_NULL;
	
	if(m_gcs_cycle_list->pool_push_count < m_gcs_cycle_list->entry_number + m_gcs_cycle_list->pool_pop_count)   
	{
		m_gcs_cycle_list->poolList[m_gcs_cycle_list->pool_push_index] = pentry;
		m_gcs_cycle_list->pool_push_count++;

		if(m_gcs_cycle_list->pool_push_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->pool_push_index = 0;
		else m_gcs_cycle_list->pool_push_index++;
		
		if(m_gcs_cycle_list->status == UMCQ_STATUS_WAIT)
		{
			video_mutex_lock(m_gcs_cycle_list->mutex);

			if(m_gcs_cycle_list->status == UMCQ_STATUS_WAIT)
			{
				//notify
				m_gcs_cycle_list->status = UMCQ_STATUS_RUN;
				video_event_notify(m_gcs_cycle_list->event);
			}
			video_mutex_unlock(m_gcs_cycle_list->mutex);
		}
		
		return pentry;
	}

	return UM_NULL;
}

//如果返回null，就说明出错了
pUmCQEntry um_cq_pushData(void* pCQ, char* data, int dataLen, unsigned int pts)
{
	pUmCircleQueueStr m_gcs_cycle_list = (pUmCircleQueueStr)pCQ;
	pUmCQEntry pentry = UM_NULL;

	if(!pCQ || m_gcs_cycle_list->status == UMCQ_STATUS_EXIT) return UM_NULL;
	
	while(m_gcs_cycle_list->status != UMCQ_STATUS_EXIT && UM_NULL == (pentry = m_um_cq_popEmptyEntry(pCQ)))
	{
		video_mutex_lock(m_gcs_cycle_list->mutex);
		
		if(m_gcs_cycle_list->status == UMCQ_STATUS_RUN)
		{
			if(UM_NULL != (pentry = m_um_cq_popEmptyEntry(pCQ)))
			{
				video_mutex_unlock(m_gcs_cycle_list->mutex);
				break;
			}
			
			//block then
			m_gcs_cycle_list->status = UMCQ_STATUS_WAIT;
			//LOGE("um_cq_pushData, WAIT\r\n");
			
			if(UM_EVENT_WAIT_TIMEOUT == um_event_wait(m_gcs_cycle_list->event, m_gcs_cycle_list->mutex, 500) &&
				m_gcs_cycle_list->status != UMCQ_STATUS_EXIT)
			{
				m_gcs_cycle_list->status = UMCQ_STATUS_RUN;
			}
				//LOGE("um_cq_pushData, WAIT END\r\n");
		}
		else if(m_gcs_cycle_list->status != UMCQ_STATUS_WAIT)
		{
			video_mutex_unlock(m_gcs_cycle_list->mutex);
			return UM_NULL;
		}

		video_mutex_unlock(m_gcs_cycle_list->mutex);
	}

	if(!pentry) return UM_NULL;

	pentry->dataLen = dataLen;

	if(pentry->bufferSize < dataLen)
	{
		if(pentry->dataHeader) free(pentry->dataHeader);
		pentry->dataHeader = (char*)malloc(dataLen);
		pentry->bufferSize = dataLen;
	}

	memcpy((void*)pentry->dataHeader, data, dataLen);
	pentry->pts = pts;

	//push it then
	m_um_cq_pushFilledEntry(pCQ, pentry);

	if(m_gcs_cycle_list->status == UMCQ_STATUS_WAIT)
	{
		video_mutex_lock(m_gcs_cycle_list->mutex);
		
		if(m_gcs_cycle_list->status == UMCQ_STATUS_WAIT)
		{
			//notify
			m_gcs_cycle_list->status = UMCQ_STATUS_RUN;
			video_event_notify(m_gcs_cycle_list->event);
		}
		video_mutex_unlock(m_gcs_cycle_list->mutex);
	}
	
	return pentry;
}

pUmCQEntry um_cq_popFilledEntry(void* pCQ)
{
	pUmCircleQueueStr m_gcs_cycle_list = (pUmCircleQueueStr)pCQ;
	pUmCQEntry pentry = UM_NULL;

	if(!pCQ || m_gcs_cycle_list->status == UMCQ_STATUS_EXIT) return UM_NULL;

	//LOGE("um_cq_popFilledEntry enter \r\n");
	
	while(m_gcs_cycle_list->status != UMCQ_STATUS_EXIT && m_gcs_cycle_list->circle_push_count == m_gcs_cycle_list->circle_pop_count)
	{
		video_mutex_lock(m_gcs_cycle_list->mutex);
		
		if(m_gcs_cycle_list->status == UMCQ_STATUS_RUN)
		{
			if(m_gcs_cycle_list->circle_push_count != m_gcs_cycle_list->circle_pop_count)
			{
				um_mutex_unlock(m_gcs_cycle_list->mutex);
				break;
			}
			
			//block then
			m_gcs_cycle_list->status = UMCQ_STATUS_WAIT;
			//LOGE("um_cq_popFilledEntry, WAIT\r\n");
			if(UM_EVENT_WAIT_TIMEOUT == video_event_wait(m_gcs_cycle_list->event, m_gcs_cycle_list->mutex, 500) &&
				m_gcs_cycle_list->status != UMCQ_STATUS_EXIT)
			{
				m_gcs_cycle_list->status = UMCQ_STATUS_RUN;
			}
			//LOGE("um_cq_popFilledEntry, WAIT END\r\n");
		}
		else if(m_gcs_cycle_list->status != UMCQ_STATUS_WAIT)
		{
			video_mutex_unlock(m_gcs_cycle_list->mutex);
			//LOGE("um_cq_popFilledEntry 1(%d) \r\n", m_gcs_cycle_list->status);
			return UM_NULL;
		}

		video_mutex_unlock(m_gcs_cycle_list->mutex);
	}

	if(m_gcs_cycle_list->status == UMCQ_STATUS_EXIT)
	{
		//LOGE("um_cq_popFilledEntry 2(%d) \r\n", m_gcs_cycle_list->status);
		if(pentry)
		{
			pentry->dataLen = 0;
			um_cq_pushEmptyEntry(pCQ, pentry);
		}
		
		return UM_NULL;
	}
	
	pentry = m_gcs_cycle_list->queueList[m_gcs_cycle_list->circle_pop_index];
	m_gcs_cycle_list->circle_pop_count++;

	if(m_gcs_cycle_list->circle_pop_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->circle_pop_index = 0;   
	else m_gcs_cycle_list->circle_pop_index++;

	//LOGE("um_cq_popFilledEntry leave(%d) \r\n", pentry->dataLen);

	if(m_gcs_cycle_list->status == UMCQ_STATUS_WAIT)
	{
		video_mutex_lock(m_gcs_cycle_list->mutex);

		if(m_gcs_cycle_list->status == UMCQ_STATUS_WAIT)
		{
			//notify
			m_gcs_cycle_list->status = UMCQ_STATUS_RUN;
			video_event_notify(m_gcs_cycle_list->event);
		}
		video_mutex_unlock(m_gcs_cycle_list->mutex);
	}

	return pentry;
}

int um_cq_isFull(void* pCQ)
{
	pUmCircleQueueStr m_gcs_cycle_list = (pUmCircleQueueStr)pCQ;
 
	if(!m_gcs_cycle_list) 
	{
		return 1;
	}
	
	if(m_gcs_cycle_list->circle_push_count - m_gcs_cycle_list->circle_pop_count < m_gcs_cycle_list->entry_number) return 0;

	return 1;
}
