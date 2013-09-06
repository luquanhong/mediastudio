/** @defgroum the event related functions
  * @author xxxx
  * @name um_sys_event.cpp
  * @version 1.0
  */

/** @brief wait the event to occur
  * @param cond
  * @param mux
  * @param duration if -1, then wait for the event forever
  * @return
  */
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>


#include "um_types.h"
#include "um_sys_mutex.h"
#include "um_sys_event.h"

UMSint um_event_wait(UmEventHandle cond, UmMutexHandle mux, UMSint duration)
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
void um_event_notify(UmEventHandle condvarHandle)
{
    if(condvarHandle == UM_NULL)
	{
        return;
    }
	pthread_cond_signal((pthread_cond_t *)condvarHandle);
}


void um_event_notifyAll(UmEventHandle condvarHandle)
{
    if(condvarHandle == UM_NULL)
	{
        return;
    }
	pthread_cond_broadcast((pthread_cond_t *)condvarHandle);
}

UmEventHandle um_event_create(void)
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

void um_event_destroy(UmEventHandle condvarHandle)
{
	void* conHandle = (void *)condvarHandle;

    if(condvarHandle == UM_NULL)
	{
        return;
    }

	pthread_cond_destroy((pthread_cond_t *)(condvarHandle));
	free(conHandle);
}

