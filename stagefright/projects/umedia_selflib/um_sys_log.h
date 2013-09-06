/*
============================================================================
Name		: um_sys_log.h
Author		: 
Version		: 
Copyright   : 
Description : 
============================================================================
*/
#ifndef _UB_SYS_LOG_H_
#define _UB_SYS_LOG_H_

#include <android/log.h>

#define ENABLE_LOG	1
#define LOG_IO_FILE	0x00000001
#define LOG_IO_CONSOLE	0x00000002
#define LOG_TAG "UM"


/************************************************************************/
/* log definition                                                       */
/************************************************************************/

#ifdef ENABLE_LOG
	#define  UMLOG_INFO(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
	#define  UMLOG_ERR(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
	#define  UMLOG_INFO(...)	 
	#define  UMLOG_ERR(...)	
#endif

#endif //_UB_SYS_LOG_H_
