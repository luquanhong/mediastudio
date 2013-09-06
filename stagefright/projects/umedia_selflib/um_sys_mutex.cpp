
#include <stdlib.h>
#include <pthread.h>

#include "um_sys_mutex.h"

//#define UM_SYS_CHECK_DEADLOCK

#ifdef UM_SYS_CHECK_DEADLOCK

typedef struct _um_mutex_check_str
{
	UmMutexHandle mutex;
	int ref;
	struct _um_mutex_check_str* next;
}
um_mutex_check_str;

static um_mutex_check_str* m_check_list = UM_NULL;

static void m_um_mutex_check_add(UmMutexHandle mutex)
{
	um_mutex_check_str* check_entry = m_check_list;

	while(check_entry)
	{
		if(check_entry->mutex == mutex) return;
		check_entry = check_entry->next;
	}

	check_entry = m_check_list;

	if(check_entry == UM_NULL)
	{
		m_check_list = (um_mutex_check_str*)memset(
					malloc(sizeof(um_mutex_check_str)), 0, sizeof(um_mutex_check_str));
		m_check_list->mutex = mutex;
		return;
	}

	while(check_entry->next) check_entry = check_entry->next;
	check_entry->next = (um_mutex_check_str*)memset(
				malloc(sizeof(um_mutex_check_str)), 0, sizeof(um_mutex_check_str));
	check_entry->next->mutex = mutex;
}

static void m_um_mutex_check_rm(UmMutexHandle mutex)
{
	um_mutex_check_str* pre, *check_entry;

	pre = check_entry = m_check_list;
	while(check_entry)
	{
		if(check_entry->mutex == mutex)
		{
			if(m_check_list == check_entry)
			{
				m_check_list = check_entry->next;
				free(check_entry);
			}
			else
			{
				pre->next = check_entry->next;
				free(check_entry);
			}

			return;
		}
		pre = check_entry;
		check_entry = check_entry->next;
	}
}

static int m_um_mutex_check_find(UmMutexHandle mutex)
{
	um_mutex_check_str* check_entry = m_check_list;

	while(check_entry)
	{
		if(check_entry->mutex == mutex) return check_entry->ref;
		check_entry = check_entry->next;
	}

	return 0;
}

static void m_um_mutex_check_lock(UmMutexHandle mutex)
{
	um_mutex_check_str* check_entry = m_check_list;

	while(check_entry)
	{
		if(check_entry->mutex == mutex)
		{
			check_entry->ref++;
			return;
		}
		check_entry = check_entry->next;
	}
}

static void m_um_mutex_check_unlock(UmMutexHandle mutex)
{
	um_mutex_check_str* check_entry = m_check_list;

	while(check_entry)
	{
		if(check_entry->mutex == mutex)
		{
			check_entry->ref--;
			return;
		}
		check_entry = check_entry->next;
	}
}

static void m_um_mutex_check_release(void)
{
	um_mutex_check_str* check_entry;

	check_entry = m_check_list;
	while(check_entry)
	{
		m_check_list = check_entry->next;
		free(check_entry); 
		check_entry = m_check_list;
	}

	m_check_list = UM_NULL;
}

#endif

void um_mutex_init(void)
{
#ifdef UM_SYS_CHECK_DEADLOCK
	m_check_list = UM_NULL;
#endif
}

void um_mutex_fini(void)
{
#ifdef UM_SYS_CHECK_DEADLOCK
	m_um_mutex_check_release();
#endif
}

/**
 * Lock a mutex
 */
UMSint um_mutex_lock(UmMutexHandle mutexHandle)
{
    //JKLOG(JkTaskLockMutex,("lock mutex enter..."))
    if(mutexHandle == UM_NULL)
	{
        return UM_MUTEX_FAILURE;
    }

#ifdef UM_SYS_CHECK_DEADLOCK
	if(m_um_mutex_check_find(mutexHandle) >= 1)
	{
		//dosomething
		int i = 0;
		i++;
	}
#endif

	if(0 == pthread_mutex_lock((pthread_mutex_t *)mutexHandle))
	{
		return UM_MUTEX_OK;
	}
	
	return UM_MUTEX_FAILURE;	
}

/**
 * Unlock a mutex
 */
UMSint um_mutex_unlock(UmMutexHandle mutexHandle)
{
    if(mutexHandle == UM_NULL)
	{
        return UM_MUTEX_FAILURE;
    }

#ifdef UM_SYS_CHECK_DEADLOCK
	m_um_mutex_check_unlock(mutexHandle);
#endif

	if(0 == pthread_mutex_unlock((pthread_mutex_t *)mutexHandle))
	{
	    return UM_MUTEX_OK;
	}
	return UM_MUTEX_FAILURE;
}

/**
 * Create a mutex
 */


/**
 * Destroy a mutex
 */
void um_mutex_destroy(UmMutexHandle mutexHandle)
{
    if(mutexHandle == UM_NULL)
	{
        return;
    }
	
#ifdef UM_SYS_CHECK_DEADLOCK
	m_um_mutex_check_rm(mutexHandle);
#endif

	pthread_mutex_destroy((pthread_mutex_t *)mutexHandle);
	free((void*)mutexHandle);
}

UmMutexHandle um_mutex_create(void)
{
	UmMutexHandle mutexHandle;
	mutexHandle = (UmMutexHandle)malloc(sizeof(pthread_mutex_t));

	if(mutexHandle == NULL)
	{
		return NULL;
	}

#ifdef UM_SYS_CHECK_DEADLOCK
	m_um_mutex_check_add(mutexHandle);
#endif

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
