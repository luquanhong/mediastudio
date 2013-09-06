/*
 * utils_codec.c
 *
 *  Created on: 2013-4-7
 *      Author: Administrator
 */

#include <stddef.h>
#include "avcodec.h"

#define INT_MAX	2177483647

void* av_malloc(unsigned int size){

	void* ptr;

	if(size > INT_MAX)
		return NULL;

	ptr = malloc(size);

	return ptr;
}


void* av_realloc(void* ptr, unsigned int size){

	if(size > INT_MAX)
		return NULL;

	return realloc(ptr, size);
}

void* av_mallocz(unsigned int size){

	void* ptr;

	ptr = av_malloc(size);

	if(!ptr)
		return NULL;

	memset(ptr, 0, size);

	return ptr;
}


void av_free(void* ptr){

	if(ptr)
		free(ptr);
}

void av_freep(void* arg){

	void** ptr = (void**)arg;

	av_free(*ptr);

	*ptr = NULL;
}
