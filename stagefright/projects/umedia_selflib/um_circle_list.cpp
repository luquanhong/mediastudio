
/***************************************************************************
 * Include Files                 	
 ***************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "um_types.h"
#include "um_circle_list.h"

pUMCircleList um_cl_create(int len)
{
	pUMCircleList m_gcs_cycle_list = UM_NULL;

	//allocate the circle list struct
	m_gcs_cycle_list = (pUMCircleList)memset(malloc(sizeof(UMCircleList)),
		0, sizeof(UMCircleList));

	m_gcs_cycle_list->entry_number = len;
	m_gcs_cycle_list->high_index   = len -1;
	m_gcs_cycle_list->list = (void**)malloc(len * sizeof(void*));

	return m_gcs_cycle_list;
}

void um_cl_destroy(pUMCircleList m_gcs_cycle_list)
{
	if(m_gcs_cycle_list)
	{
		free(m_gcs_cycle_list->list);
		free(m_gcs_cycle_list);
	}
}

//switch the 2 frames of the most oldest, and popup the new older one. [thread safe]
void* um_cl_list_popSwitch(pUMCircleList m_gcs_cycle_list)
{
	void* oldest = UM_NULL, *younger;
	
	if(m_gcs_cycle_list && m_gcs_cycle_list->circle_push_count != m_gcs_cycle_list->circle_pop_count &&
		m_gcs_cycle_list->circle_push_count != (m_gcs_cycle_list->circle_pop_count + 1))
	{
		//get the oldest one
		oldest = m_gcs_cycle_list->list[m_gcs_cycle_list->circle_pop_index];
		m_gcs_cycle_list->circle_pop_count++;
		
		if(m_gcs_cycle_list->circle_pop_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->circle_pop_index = 0;   
		else m_gcs_cycle_list->circle_pop_index++;

		//get the younger one
		younger = m_gcs_cycle_list->list[m_gcs_cycle_list->circle_pop_index];

		//put the oldest one back
		m_gcs_cycle_list->list[m_gcs_cycle_list->circle_pop_index] = oldest;
		
		return younger;
	}

	return UM_NULL;
}

void* um_cl_list_push(pUMCircleList m_gcs_cycle_list, void* pentry)
{
	if(m_gcs_cycle_list && (m_gcs_cycle_list->circle_push_count - m_gcs_cycle_list->circle_pop_count < m_gcs_cycle_list->entry_number))
	{
	  m_gcs_cycle_list->list[m_gcs_cycle_list->circle_push_index] = pentry;
	  m_gcs_cycle_list->circle_push_count++;
	  
	  if(m_gcs_cycle_list->circle_push_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->circle_push_index = 0;
	  else m_gcs_cycle_list->circle_push_index++;
	  
	  return pentry;
	}
	else
	  return UM_NULL;
}

int um_cl_list_isFull(pUMCircleList m_gcs_cycle_list)
{
	if(m_gcs_cycle_list->circle_push_count - m_gcs_cycle_list->circle_pop_count < m_gcs_cycle_list->entry_number) return 0;
	return 1;
}

int um_cl_list_isEmpty(pUMCircleList m_gcs_cycle_list)
{
	if(m_gcs_cycle_list->circle_push_count != m_gcs_cycle_list->circle_pop_count) return 0;
	return 1;
}

int um_cl_list_count(pUMCircleList m_gcs_cycle_list)
{
	unsigned int pushCount, popCount;

	pushCount = m_gcs_cycle_list->circle_push_count;
	popCount  = m_gcs_cycle_list->circle_pop_count;

	if(popCount < pushCount) return (pushCount - popCount);
	else if(popCount == pushCount)
	{
		return 0;
	}
	else
	{
		return (4294967295UL - popCount + pushCount);
	}
}

void* um_cl_list_pop(pUMCircleList m_gcs_cycle_list)
{
	void* result = UM_NULL;
	
	if(m_gcs_cycle_list && m_gcs_cycle_list->circle_push_count != m_gcs_cycle_list->circle_pop_count)
	{
		result = m_gcs_cycle_list->list[m_gcs_cycle_list->circle_pop_index];
		m_gcs_cycle_list->circle_pop_count++;

		if(m_gcs_cycle_list->circle_pop_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->circle_pop_index = 0;   
		else m_gcs_cycle_list->circle_pop_index++;
		return result;
	}

	return UM_NULL;
}

void* um_cl_list_fakePop(pUMCircleList m_gcs_cycle_list)
{
	void* result = UM_NULL;
	
	if(m_gcs_cycle_list && m_gcs_cycle_list->circle_push_count != m_gcs_cycle_list->circle_pop_count)
	{
		result = m_gcs_cycle_list->list[m_gcs_cycle_list->circle_pop_index];
		return result;
	}

	return UM_NULL;
}

