/* FILE NAME: srv_nw_handler.h
 *
 * Version: 1.0,  Date: 2009-4-1
 *
 * Description: 
 *
 * Platform: 
 *
 ***************************************************************************/
#ifndef _UTIL_CIRCLE_LIST_HEADER
#define _UTIL_CIRCLE_LIST_HEADER

#include "um_types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct __UMCircleList_
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

	//circle list
	void** list;
}
* pUMCircleList, UMCircleList;

pUMCircleList um_cl_create(int len);
void um_cl_destroy(pUMCircleList m_gcs_cycle_list);
void* um_cl_list_pop(pUMCircleList m_gcs_cycle_list);
void* um_cl_list_push(pUMCircleList m_gcs_cycle_list, void* pentry);
int um_cl_list_isFull(pUMCircleList m_gcs_cycle_list);
int um_cl_list_isEmpty(pUMCircleList m_gcs_cycle_list);
void* um_cl_list_fakePop(pUMCircleList m_gcs_cycle_list);
int um_cl_list_count(pUMCircleList m_gcs_cycle_list);
void* um_cl_list_popSwitch(pUMCircleList m_gcs_cycle_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !___UMTYPES_H */
