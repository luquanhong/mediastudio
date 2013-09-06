/*
 * allformats.c
 *
 *  Created on: 2013-4-7
 *      Author: Administrator
 */

#include "avformat.h"

extern	URLProtocol	file_protocol;

void av_register_all(void){



	register_protocol(&file_protocol);

}
