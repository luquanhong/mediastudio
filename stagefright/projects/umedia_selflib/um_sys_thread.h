/**
 *@defgroup the sync module
 *@{
 *@}
 */
#ifndef UM_SYS_THREAD_H_
#define UM_SYS_THREAD_H_

#include "um_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** define the callback function for thread creation */
typedef void  UmThreadStart(void*);

/** define the thread handler */
typedef void* UmThreadHandle;

typedef enum
{
	UM_THREAD_PRIORITY_HIGH,
	UM_THREAD_PRIORITY_NORMAL,
	UM_THREAD_PRIORITY_LOW,
	UM_THREAD_PRIORITY_IDLE
}UmThreadPrior;

UmThreadHandle um_thread_create( UMUint stackSize, UmThreadPrior priority, UmThreadStart func, void* arg , char *name);
UMBool um_thread_kill(UmThreadHandle threadHandle, UMSint retVal);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //UM_SYS_THREAD_H_
