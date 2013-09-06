#ifndef	__TEST_H__
#define __TEST_H__

#include <android/log.h>

#define DEBUG 

#ifdef DEBUG
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "StageFright", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "StageFright", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , "StageFright", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , "StageFright", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "StageFright", __VA_ARGS__)
#else	
#define LOGV(...) 
#define LOGD(...) 
#define LOGI(...) 
#define LOGW(...) 
#define LOGE(...) 
#endif


/*
#ifdef	__cplusplus
extern "C"{
#endif
*/

//int  test_case();

//int test_video_dec();

//int test_circle_list();

/*
#ifdef	__cplusplus
}
#endif
*/

#endif
