/*
 * avio.c
 *
 *  Created on: 2013-4-9
 *      Author: Administrator
 */

#include "avformat.h"

URLProtocol* first_protocol;

int register_protocol(URLProtocol* protocol){

	URLProtocol** p;

	p = &first_protocol;

	while(*p != NULL)
		p = &( (*p)->next );

	*p = protocol;
	protocol->next = NULL;

	return 0;
}


int url_open(URLContext** puc, const char* filename, int flags){

	int err;
	URLContext*  uc;
	URLProtocol* up;
	const char* p;
	char proto_str[128];
	char* q;

	p = filename;
	q = proto_str;

	while(*p != '\0' && *p != ':'){

		if(!isalpha(*p) )
			goto file_proto;

		if( (q - proto_str) < sizeof(proto_str) -1)
			*q++ = *p;

		p++;
	}

	//if the protocol has lenght 1, we consider it is a dos driver.
	if(*p == '\0' || ( q-proto_str ) <= 1){

file_proto:
		strcpy(proto_str,"file");
	}else{

		*q = '\0';
	}

	up = first_protocol;

	while( up != NULL){

		if(!strcmp(proto_str, up->name) )
			goto found;

		up->next;
	}

	err = -ENOENT;
	goto fail;

found:
	uc = av_malloc(sizeof(URLContext) + strlen(filename));

	if(!uc){

		err = -ENOMEM;
		goto fail;
	}

	strcpy(uc->filename, filename);

	uc->prot = up;
	uc->flags = flags;
	uc->max_packet_size = 0;

	err = up->url_open(uc, filename, flags);

	if(err < 0){

		av_free(uc);
		*puc = NULL;
		return err;
	}

	*puc = uc;
	return 0;

fail:
	*puc = NULL;

	return err;
}

int url_read(URLContext* h, unsigned char* buf, int size){

	int ret;

	if(h->flags & URL_WRONLY)
		return AVERROR_IO;

	ret = h->prot->url_read(h, buf, size);

	return ret;
}

offset_t url_seek(URLContext* h, offset_t offset, int whence){

	offset_t ret;

	if(!h->prot->url_seek)
		return -EPIPE;

	ret = h->prot->url_seek(h, offset, whence);

	return ret;
}

int url_close(URLContext* h){

	int ret;

	ret = h->prot->url_close(h);

	av_free(h);

	return ret;
}
