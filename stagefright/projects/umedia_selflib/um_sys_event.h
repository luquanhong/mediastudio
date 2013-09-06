/** @defgroup the event related functions
  * @author xxxx
  * @name up_sys_event.h
  * @version 1.0
  */

#ifndef UM_SYS_EVENT_H_
#define UM_SYS_EVENT_H_

#include "um_types.h"
#include "um_sys_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup event wait definitions */
#define UM_EVENT_WAIT_INFINITE (-1)
#define UM_EVENT_WAIT_TIMEOUT  (-2)
#define UM_EVENT_WAIT_FAILURE  (-3)
#define UM_EVENT_WAIT_OK       0

/** the handle general definitions */
typedef UMHandle UmEventHandle;

/** @brief wait the event to occur
  * @param cond
  * @param mux
  * @param duration if -1, then wait for the event forever
  * @return
  */
int um_event_wait(UmEventHandle cond, UmMutexHandle mux, UMSint duration);
void um_event_notify(UmEventHandle condvarHandle);
void um_event_notifyAll(UmEventHandle condvarHandle);
UmEventHandle um_event_create(void);
void um_event_destroy(UmEventHandle condvarHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //UM_SYS_EVENT_H_

