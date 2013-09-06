/**
 * @file um_types.h
 * Primitive Data Types
 */

#ifndef _UM_TYPES_H_
#define _UM_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int                 UMSint;

typedef unsigned int        UMUint;

typedef signed long         UMSint32;

typedef long long           UMSint64;

typedef unsigned long long  UMUint64;

typedef unsigned int        UMUint32;

typedef signed short        UMSint16;

typedef unsigned short      UMUint16;

typedef signed char         UMSint8;

typedef unsigned char       UMUint8;

typedef UMUint16            UMWChar;

typedef UMUint8             UMUTF8Char;

typedef int                 UMBool;

typedef signed int          UMResult;

typedef double UMDouble;

typedef float  UMFloat;

#define UMFailure (-1)

#define UMSuccess (0)

#define UM_FALSE            0

#define UM_TRUE             1

typedef const void*         UMHandle;

#define UM_INVALID_HANDLE   ((void*)UM_NULL)

#define UP_INVALID_INDEX    (-1)

#ifndef __cplusplus
#define UM_NULL             ((void *)0)
#else
#define UM_NULL             (0)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_UM_TYPES_H_ */

