/*
 * utils_format.c
 *
 *  Created on: 2013-4-9
 *      Author: Administrator
 */

#include "avformat.h"

#define	PROBE_BUF_MIN	2048
#define	PROBE_BUF_MAX	131072


int av_open_input_file(AVFormatContext** ic_ptr, const char* filename, AVInputFormat* fmt,
		int buf_size, AVFormatParameters* ap){

	int err, must_open_file, file_opened, probe_size;
	AVProbeData	probe_data, *pd = &probe_data;
	ByteIOContext pb1, *pb = &pb1;

	file_opened = 0;

	pd->fileName = "";

	if(filename)
		pd->fileName = filename;

	pd->buf = NULL;
	pd->buf_size = 0;

	must_open_file = 1;

	if(!fmt || must_open_file){

		if(url_fopen(pb, filename, URL_RDONLY) < 0){

			err = AVERROR_IO;
			goto fail;
		}

		file_opened = 1;

		if(buf_size > 0)
			url_setbufsize(pb,buf_size);

		for(probe_size = PROBE_BUF_MIN; probe_size <= PROBE_BUF_MAX &&
				!fmt; probe_size << 1 ){

			pd->buf = av_realloc(pd->buf, probe_size);
			pd->buf_size = url_fread(pb, pd->buf, probe_size);

			if(url_fseek(pb, 0, SEEK_SET) == (offset_t)-EPIPE){

				url_fclose(pb);

				if(url_fopen( pb,filename, URL_RDONLY) < 0){

					file_opened = 0;
					err = AVERROR_IO;
					goto fail;
				}
			}

			//fmt = av_probe_input_format(pd, 1);
		}

		av_freep(&pd->buf);
	}

	if(!fmt){

		err = AVERROR_NOFMT;
		goto fail;
	}

	//err = av_open_input_stream(ic_ptr, pb, filename, fmt, ap);

	if(err)
		goto fail;

	return 0;

fail:
	av_freep(&pd->buf);

	if(file_opened)
		url_fclose(pb);

	*ic_ptr = NULL;

	return err;

}



void av_close_input_file(AVFormatContext* s){


}
