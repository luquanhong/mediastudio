/*
 * cutils.c
 *
 *  Created on: 2013-4-7
 *      Author: Administrator
 */

#include "avformat.h"

int strstart(const char *str, const char *val, const char **ptr)
{
    const char *p,  *q;
    p = str;
    q = val;
    while (*q != '\0')
    {
        if (*p !=  *q)
            return 0;
        p++;
        q++;
    }
    if (ptr)
        *ptr = p;
    return 1;
}


void pstrcpy(char* dst, int size, const char* str){

	int c;
	char* tmp = dst;

	if(size <= 0)
		return;

	for(;;){

		c = *str++;

		if(c == 0 || (tmp >= dst + size -1 ))
			break;

		*tmp++ = c;
	}

	*tmp = '\0';
}

