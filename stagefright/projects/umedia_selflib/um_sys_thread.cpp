/** @defgroup The associated thread functionalities' implementation
 *  @authur   jason
 *  @version  1.0
 *  @data     2007/11/26
 *  @{
 */
#include <stdlib.h>
#include <pthread.h>

#include "um_types.h"
#include "um_sys_thread.h"

/** @name the internal structure definition
 *  @{
 */
struct args {
    void (*func)(void *);
    void *arg;
};



/** @} */ //static function definition

/** @brief to create a thread and start it from a specific function
  * @param [in] stackSize 
  * @param [in] priority
  * @param [in] ...
  * @return the handler of the thread
  */

UmThreadHandle um_thread_create(UMUint stackSize, UmThreadPrior priority, UmThreadStart func, void* arg , char *name)
{
	int result, priMax = -1, priMin = -1, policy;

	pthread_t thr_id;
    struct sched_param param;
    pthread_attr_t attr;
	pthread_attr_init(&attr);

	stackSize = 16*1024;

	if(stackSize > 0)
	{
  		result = pthread_attr_setstacksize(&attr, stackSize);
 		if(result != 0)
		{
			pthread_attr_destroy(&attr);
  	    	return NULL;
		}	
  	}
	else
	{
		result = pthread_attr_setstacksize(&attr, 16384);
	}

	
	//if(0 == pthread_attr_getschedpolicy(&attr, &policy))
	//{
	//	priMin = sched_get_priority_min(policy);
	//	priMax = sched_get_priority_max(policy);
	//

	pthread_attr_getschedparam(&attr, &param);

	if(priMax >= 0 && priMin >= 0)
	{
		switch(priority)
		{
			case UM_THREAD_PRIORITY_HIGH:
				priMin = (priMax + priMin)/2;
				param.sched_priority = (priMax + priMin)/2;
				break;
			case UM_THREAD_PRIORITY_LOW:
			case UM_THREAD_PRIORITY_IDLE:
				priMax = (priMax + priMin)/2;
				param.sched_priority = (priMax + priMin)/2;
				break;
			default:
				param.sched_priority = (priMax + priMin)/2;
				break;
		}
	}
	else
	{
	    /* %comment dlong001 */
	  	param.sched_priority = (priMax + priMin)/2;
	}
	
  	pthread_attr_setschedparam(&attr, &param);

    /* Don't leave thread/stack around after exit for join */
  	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  	result = pthread_create(&thr_id, &attr, (void*(*)(void *))func, arg);
  	pthread_attr_destroy(&attr);
    
	//return result != 0 ? free(args), NULL : (UmThreadHandle)thr_id;
	return (UmThreadHandle)thr_id;
}

/** @brief kill the specified thread
  * @return the operation result
  */

UMBool um_thread_kill(UmThreadHandle threadHandle, UMSint retVal)
{
//	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL); //zhgq
	//pthread_cancel((pthread_t)threadHandle);
	return 1;
}

/** @} *///the defgroup
