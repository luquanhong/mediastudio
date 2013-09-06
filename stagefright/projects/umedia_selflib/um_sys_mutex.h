/** @defgroup the mutex related functions
  * @author xxxx
  * @name up_sys_mutex.h
  * @version 1.0
  */

#ifndef UM_SYS_MUTEX_H_
#define UM_SYS_MUTEX_H_

#include "um_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** the handle general definitions */
typedef UMHandle UmMutexHandle;

#define UM_MUTEX_OK       0
#define UM_MUTEX_FAILURE  (-1)

UMSint um_mutex_lock(UmMutexHandle mutexHandle);

UMSint um_mutex_unlock(UmMutexHandle mutexHandle);

UmMutexHandle um_mutex_create(void);

void um_mutex_destroy(UmMutexHandle mutexHandle);

void um_mutex_init(void);
void um_mutex_fini(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //UM_SYS_MUTEX_H_
