#include "um_mtm.h"
#include "um_sys_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define PARAMS_CHECK(exp, ret)			if(!(exp)){UMLOG_INFO("Warning: %s failed.", #exp);ret;}

typedef struct _MtmNode
{
	void* ptr;
	unsigned int size;
	const char* file;
	int line;
	struct _MtmNode *prev, *next;
}MtmNode;

typedef struct _MtmHeader
{
	int nodeCnt;
	MtmNode *head, *tail;
    pthread_mutex_t mutex;
	void* (*mallocImpl)(unsigned int size);
	void (*freeImpl)(void* ptr);
}MtmHeader;

static MtmHeader *g_mtm = NULL;

int mtm_init()
{
	PARAMS_CHECK(g_mtm==NULL, return -1);
	g_mtm = (MtmHeader*)malloc(sizeof(MtmHeader));
	if(g_mtm!=NULL)
	{
		g_mtm->nodeCnt = 0;
		g_mtm->head = g_mtm->tail = NULL;
		pthread_mutex_init(&g_mtm->mutex, NULL);
		g_mtm->mallocImpl = malloc;
		g_mtm->freeImpl = free;
	}
	return (g_mtm!=NULL) ? 0 : -1;
}
void mtm_fini()
{
	if(g_mtm != NULL)
	{
		if(mtm_is_memLeak() != 0)
		{
			mtm_list_free();
		}
		pthread_mutex_destroy(&g_mtm->mutex);
		free(g_mtm);
		g_mtm = NULL;
	}
}
int mtm_list_add(void* ptr, unsigned int size, const char* file, int line)
{
	MtmNode *node = NULL;

	PARAMS_CHECK(g_mtm!=NULL && ptr!=NULL, return -1);
	pthread_mutex_lock(&g_mtm->mutex);
	node = (MtmNode*)g_mtm->mallocImpl(sizeof(MtmNode));
	if(node!=NULL)
	{
		if(g_mtm->head==NULL)
		{
			g_mtm->head = g_mtm->tail = node;
			node->prev = NULL;
		}
		else
		{
			node->prev = g_mtm->tail;
			g_mtm->tail->next = node;
			g_mtm->tail = node;
		}
		node->ptr = ptr;
		node->file = file;
		node->line = line;
		node->size = size;
		node->next = NULL;
		++g_mtm->nodeCnt;
	}
    pthread_mutex_unlock(&g_mtm->mutex);
	return (node!=NULL) ? 0 : -1;
}
int mtm_list_remove(void* ptr, const char* file, int line)
{
	int count;
	MtmNode *cur, *nd1, *nd2;

	PARAMS_CHECK(g_mtm!=NULL && ptr!=NULL, return -1);
    pthread_mutex_lock(&g_mtm->mutex);
	nd1 = g_mtm->head;
	nd2 = g_mtm->tail;
	cur = NULL;
	count = g_mtm->nodeCnt;
	/* 双向查找 */
	while(count>0)
	{
		if(nd1->ptr==ptr)
		{
			cur = nd1;
			break;
		}
		if(nd2->ptr==ptr)
		{
			cur = nd2;
			break;
		}
		nd1 = nd1->next;
		nd2 = nd2->prev;
		count -= 2;
	}
	/* not found */
	if(cur==NULL)
	{ 
        UMLOG_INFO("fail to free 0x%08X [%s:%d]", ptr, file, line);
		goto done;
	}
	if(cur->prev != NULL)
	{
		cur->prev->next = cur->next;
	}
	if(cur->next !=NULL)
	{
		cur->next->prev = cur->prev;
	}
	if(cur == g_mtm->head)
	{
		g_mtm->head = cur->next;
		assert(g_mtm->head==NULL || (g_mtm->head!=NULL && g_mtm->head->prev==NULL));
	}
	if(cur == g_mtm->tail)
	{
		g_mtm->tail = cur->prev;
		assert(g_mtm->tail==NULL || (g_mtm->tail!=NULL && g_mtm->tail->next == NULL));
	}
	g_mtm->freeImpl(cur);
	--g_mtm->nodeCnt;
done:
    pthread_mutex_unlock(&g_mtm->mutex);
	return (cur!=NULL) ? 0 : -1;
}
int mtm_is_memLeak()
{
	PARAMS_CHECK(g_mtm!=NULL, return -1);
	return ((g_mtm->nodeCnt!=0)||(g_mtm->head!=NULL)) ? 1 : 0;
}
void mtm_list_free()
{
	MtmNode *cur, *nxt;
	int ret = 0;

	PARAMS_CHECK(g_mtm!=NULL, return);
	cur = g_mtm->head;
	while(cur!=NULL)
	{
        	UMLOG_INFO("memory leak at 0x%08X, size=%d, [%s:%d]", cur->ptr, cur->size, cur->file, cur->line);
		nxt = cur->next;
		g_mtm->freeImpl(cur);
		cur = nxt;
		++ret;
	}
	g_mtm->head = NULL;
	g_mtm->nodeCnt = 0;
    if(ret>0)
	{
        	UMLOG_INFO("warning: There are %d pointer not freed!", ret);
	}
}
void mtm_list_print()
{
	MtmNode *cur;

	PARAMS_CHECK(g_mtm!=NULL, return);
	cur = g_mtm->head;
	while(cur!=NULL)
	{
		UMLOG_INFO("record at 0x%08X, size=%d, [%s:%d]", cur->ptr, cur->size, cur->file, cur->line);
		cur = cur->next;
	}
}
void* mtm_malloc(unsigned int size,const char* file,int line)
{
	void* ptr = malloc(size);
	mtm_list_add(ptr, size, file, line);
	return ptr;
}
void* mtm_calloc(unsigned int num, unsigned int size, const char* file, int line)
{
	void* ptr = calloc(num, size);
	mtm_list_add(ptr, num*size, file, line);
	return ptr;
}
void* mtm_realloc(void *ptr, unsigned int size, const char* file, int line)
{
	void* ret = realloc(ptr, size);
	PARAMS_CHECK(ret!=NULL, return ptr);
	mtm_list_remove(ptr, file, line);
	mtm_list_add(ret, size, file, line);
	return ret;
}
char* mtm_strdup(const char* str, const char* file, int line)
{
	char* newStr = strdup(str);
	mtm_list_add(newStr, (unsigned int)strlen(newStr), file, line);
	return newStr;
}
void mtm_free(void *ptr, const char* file, int line)
{
	mtm_list_remove(ptr, file, line);
	free(ptr);
}

#if defined(ENABLE_OS_WATCH)
#include <glib-object.h>
static void on_weak_notify(void* data, GObject* obj)
{
	mtm_list_remove(obj, __FILE__, __LINE__);
}
void mtm_watch(void *ptr, const char* file, int line)
{
    if(ptr!=NULL && G_IS_OBJECT(ptr))
	{
        mtm_list_add(ptr, -1, file, line);
        g_object_weak_ref((GObject*)ptr, (GWeakNotify)on_weak_notify, NULL);
    }
}
#endif
