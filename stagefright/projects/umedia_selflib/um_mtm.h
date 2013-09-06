/*
============================================================================
Name		: um_mtm.h
Author		: 
Version		: 
Copyright   :
Description : 
============================================================================
*/
#ifndef _UM_MTM_H_
#define _UM_MTM_H_

#undef malloc
#undef calloc
#undef realloc
#undef strdup
#undef free
#undef new
#undef delete

#ifdef __cplusplus
extern "C" {      
#endif 
	
/* initialization */
int		mtm_init();
void	mtm_fini();
/* memory allocation */
void*	mtm_malloc(unsigned int size, const char* file, int line);
void*	mtm_calloc(unsigned int num, unsigned int size, const char* file, int line);
void*	mtm_realloc(void *ptr, unsigned int size, const char* file, int line);
char*	mtm_strdup(const char* str, const char* file, int line);
void	mtm_free(void *ptr, const char* file, int line);
void	mtm_watch(void *ptr, const char* file, int line);
/* memory tracing list */
int		mtm_is_memLeak();
void	mtm_list_free();
int		mtm_list_add(void* ptr, unsigned int size, const char* file, int line);
int		mtm_list_remove(void* ptr, const char* file, int line);
void	mtm_list_print();

#ifdef __cplusplus
};
#endif

#endif //_UM_MTM_H_
