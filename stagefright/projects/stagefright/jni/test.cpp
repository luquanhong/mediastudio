#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "um_sys_thread.h"
#include "um_sys_event.h"
#include "um_sys_mutex.h"
#include "um_circle_list.h"
#include "um_video_dec.h"
#include "um.h"

#include "test.h"

#if 0
char* tag = "test";

int count = 0;

typedef struct test_node{
	int   no;
	char* name;
	int   run;
	
	UmMutexHandle 	pMutex;
	UmEventHandle 	pEvent;
	
}*PTestNode,TestNode;

PTestNode test_arg = UM_NULL;


pUMCircleList test_list = UM_NULL;


void test_producer_func(void* arg)
{
	printf("creaste test_producer_func successful\n");
	
	PTestNode testArg = (PTestNode)arg;
	
	while(1)
	{
		um_mutex_lock(testArg->pMutex);
		if(!testArg->run)
		{
			um_event_wait(testArg->pEvent,testArg->pMutex,-1);
		}
		else
		{
			sleep(3);
			printf("test_producer_func is running\n");
		}
		um_mutex_unlock(testArg->pMutex);
	}
	
}

void test_consumer_func(void* arg)
{
	char ch;
	PTestNode testArg = (PTestNode)arg;
	
	printf("creaste test_producer_func successful\n");
	while(1)
	{
		scanf("&ch",ch);
		printf("ch is %c\n",ch);
		if(ch == 'q')
		{
			testArg->run = 0;
			printf("exit\n");
		}
		else if (ch == 'r')
		{
			testArg->run = 1;
			printf("run\n");
			um_event_notify(testArg->pEvent);
		}
		else
		{
			break;
		}
	}	
		
}


int test_thread()
{
	UmThreadHandle	pHandle1 = UM_NULL;
	UmThreadHandle	pHandle2 = UM_NULL;
	
	test_arg = (PTestNode)malloc(sizeof(struct test_node));
	if(test_arg == UM_NULL)
	{
		printf("create test_arg fail\n");
		return -1;
	}
	memset(test_arg,0,sizeof(struct test_node));
	test_arg->no = 100;
	test_arg->name = tag;
	test_arg->run = 0;
	
	test_arg->pMutex = um_mutex_create();
	test_arg->pEvent = um_event_create();
	
	pHandle1 = um_thread_create(0,UM_THREAD_PRIORITY_NORMAL,test_consumer_func,test_arg,UM_NULL);
	pHandle2 = um_thread_create(0,UM_THREAD_PRIORITY_NORMAL,test_producer_func,test_arg,UM_NULL);
}


int test_circle_list()
{

	PTestNode test_arg1 = UM_NULL;
	PTestNode test_arg2 = UM_NULL;

	test_arg1 = (PTestNode)malloc(sizeof(struct test_node));	
	if(test_arg1 == UM_NULL)
	{
		LOGE("create test_arg fail\n");
		return -1;
	}
	memset(test_arg1,0,sizeof(struct test_node));
	test_arg1->no = 100;
	test_arg1->name = tag;
	test_arg1->run = 0;

	test_list = um_cl_create(3);
	
	um_cl_list_push(test_list,test_arg1);
	test_arg2 = (TestNode *)um_cl_list_pop(test_list);

	LOGE("the test_arg2 is %d, and name is %s\n",test_arg2->no,test_arg2->name);
	
}


int test_video_dec()
{
#if 1
	UMSint ret;
	VDCtx* ctx;
	CodecParams* params;

	UMSint8 buf[26] = {0x00,0x00,0x00,0x01,0x67,0x42,0xc0,0x0a,0x9a,0x74,0x0b,0x04,
		0xb6,0x04,0x20,0x00,0x00,0x0c,0x80,0x00,0x02,0xed,0x51,0xe2,0x44,0xd4};

	UMSint  bufLen = 26;

	UMSint*  pts = &bufLen;

	params = (CodecParams *)malloc(sizeof(struct _CodecParams));
	if(!params)
		LOGE("params memory error");
		
	ret = um_vdec_init();
	if(ret != UMSuccess)
	{
		LOGE("um_vdec_init fail\n");
		return -1;
	}
	
	params->frameType= FRAME_TYPE_RGB;
	params->v.width = 1280;
	params->v.height = 720;
	
		
	ctx = um_vdec_create(params);
	if(!ctx)
	{
		LOGE("create codec fail\n");
		return -2;
	}

	ret = um_vdec_decode(ctx, buf, bufLen);

	if(ret == -1)
	{
		LOGE("um_vdec_decode  fail\n");
		return -3;
	}


	ret = um_vdec_destroy(ctx);

	ret = um_vdec_fini();
#endif
	return 0;
}


//int test_case()
//{
//	return 100;
//}

void close_all()
{
//	if(test_list != UM_NULL)
//		util_cl_destroy(test_list);
	
	if(test_arg->pMutex)
		um_mutex_destroy(test_arg->pMutex);
		
	if(test_arg->pEvent)
		um_event_destroy(test_arg->pEvent);
		
	if(!test_arg)
		free(test_arg);
		
}

#endif

#if 0
/*!
*	\brief	native test case
*/
int main(int argc, char* argv[])
{
	
	 
	test_thread();
	test_circle_list();
	
	sleep(100);
	
	close_all();
	printf("hello world\n");
	return 0;
}

#endif
