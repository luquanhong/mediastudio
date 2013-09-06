#ifndef UP_SYS_UTIL_H
#define UP_SYS_UTIL_H

#ifndef __BREW__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#include "um_types.h"

#ifdef __cplusplus
extern "C"{
#endif

//#define UP_USE_UID_AS_DEVICE_ID

typedef enum
{
    UP_CONN_MODE_BLUETOOTH,
    UP_CONN_MODE_EDGE,
    UP_CONN_MODE_ETHERENT,
    UP_CONN_MODE_GPRS,
    UP_CONN_MODE_INFRARED,
    UP_CONN_MODE_USB,
    UP_CONN_MODE_WIFI,
    UP_CONN_MODE_LTE,
    UP_CONN_MODE_WIMAX,
    UP_CONN_MODE_NONE
}TUPConnMode;

typedef enum
{
	UPLOG_CALLSTACK_TOP = 1, //is the call to this routine itself 
	UPLOG_CALLSTACK_TOP_1,//the next is the routine that called us
	UPLOG_CALLSTACK_TOP_2, //the third is the one that called our caller
	UPLOG_CALLSTACK_TOP_3,
	UPLOG_CALLSTACK_TOP_4,
	UPLOG_CALLSTACK_COUNT
}UPLOG_CS_DEPTH;

void up_util_init(void* p);
void up_util_fini(void);
TUPConnMode up_util_getConnMode(void);
UMUint up_util_getCurrentTick(void);
UMSint up_util_s2i(UMSint8* str);
UMSint up_util_randomNumber(void);
UMSint8* up_util_uniqueID(UMSint* len);
UMSint8* up_util_getDeviceID(UMSint* pLength);
UMSint8* up_util_getManufacture(void);
UMSint8* up_util_getOS(void);
char* up_util_hostName(void);
void up_util_setUniqueID(char* uniqueID, int len);
void up_util_setOS(char *pStr);
void up_util_setManufacture(char *pStr);

#ifdef GAME_CLOUD_CLIENT_ANDROID
char* up_util_getcurrenttime();
#endif

void up_util_setClientVersion(const UMSint8* ver);
UMSint8* up_util_getClientVersion(void);

#ifdef __cplusplus
}
#endif
#endif //UP_SYS_UTIL_H
