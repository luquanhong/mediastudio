/*
============================================================================
Name		: um_mm.h
Author		: 
Version		: 
Copyright   :
Description : 
============================================================================
*/

#ifndef _UB_MM_H_
#define _UB_MM_H_

#if defined(ENABLE_MEMORY_TRACING)
    #include "um_mtm.h"
    #define um_malloc(size)               mtm_malloc(size, __FILE__, __LINE__)
    #define um_calloc(elemCnt, elemSize)  mtm_calloc(elemCnt, elemSize, __FILE__, __LINE__)
    #define um_realloc(ptr, size)         mtm_realloc(ptr, size, __FILE__, __LINE__)
    #define um_strdup(str)                mtm_strdup(str, __FILE__, __LINE__)
    #define um_free(ptr)                  if(ptr!=NULL){mtm_free(ptr, __FILE__, __LINE__); ptr=NULL;}
    #define um_new(ptr, type)             {ptr = new type;mtm_list_add(ptr, sizeof(ptr), __FILE__, __LINE__);}
    #define um_delete(ptr)                {if(ptr){mtm_list_remove(ptr, __FILE__, __LINE__); delete ptr;ptr=NULL;}}
#else
    #define um_malloc(size)               malloc(size)
    #define um_calloc(elemCnt, elemSize)  calloc(elemCnt, elemSize)
    #define um_realloc(ptr, size)         realloc(ptr, size)
    #define um_strdup(str)                strdup(str)
    #define um_free(ptr)                  if(ptr!=NULL){free(ptr);ptr=NULL;}
    #define um_new(ptr, type)             ptr = new type
    #define um_delete(ptr)                if(ptr){delete ptr;ptr=NULL;}
#endif

#endif //_UB_MM_H_
