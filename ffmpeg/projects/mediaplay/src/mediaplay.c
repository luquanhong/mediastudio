/*
 ============================================================================
 Name        : mediaplay.c
 Author      : lqh
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#include "libavformat/avformat.h"

#undef main // We don't want SDL to override our main()

#define FF_QUIT_EVENT   (SDL_USEREVENT + 2)
#define VIDEO_PICTURE_QUEUE_SIZE 1


typedef	struct PacketQueue{

	AVPacketList *first_pkt, *last_pkt;
	int size;
	int abort_request;

	SDL_mutex* mutex;
	SDL_cond*  cond;
}PacketQueue;


typedef	struct 	VideoPicture{

	SDL_Overlay* bmp;
	int	width;
	int	height;
}VideoPicture;

typedef	struct	VideoState{

	SDL_Thread* parse_tid;
	SDL_Thread* video_tid;

	int abort_request;

	AVFormatContext* ic;

	int audio_stream;
	int video_stream;

	PacketQueue* audioq;
	PacketQueue* videoq;

	VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];
	double	frame_last_delay;

	uint8_t	audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE*3)/2];
	unsigned int audio_buf_size;
	int audio_buf_index;
	AVPacket	audio_pkt;
	uint8_t*	audio_pkt_data;
	int audio_pkt_size;

	SDL_mutex* 	video_decoder_mutex;
	SDL_mutex* 	audio_decoder_mutex;

	char filename[240];
}VideoState;


static const char* 		inputFile;
static AVInputFormat*	gInputFormat;
static VideoState*		curStream;
static SDL_Surface* 	screen;

//////////////////////////////////////////////////////////////////


static int decode_thread(void *arg)
{

	int err, i, ret, video_index, audio_index;
	AVFormatContext* ic;
	AVPacket	pkt1;
	AVPacket* 	pkt = &pkt1;
	AVFormatParameters	params;
	AVFormatParameters*	ap = &params;
	VideoState* is = arg;

	int flags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_RESIZABLE;

	video_index = -1;
	audio_index = -1;

	memset(ap, 0, sizeof(*ap));

	err = av_open_input_file(&ic, is->filename, NULL, 0, ap);

	if(err < 0){

		ret = -1;
		goto fail;
	}

	is->ic = ic;

	for(i = 0; i < ic->nb_stream; i++){

		AVCodecContext* dec = ic->streams[i]->actx;

		switch(dec->codec_type){

			case CODEC_TYPE_AUDIO:

				if(audio_index < 0){

					audio_index = i;
				}
				break;

			case CODEC_TYPE_VIDEO:

				if(video_index < 0)
					video_index = i;

				screen = SDL_SetVideoMode(480, 320, 0, flags);
				//screan = SDL_SetVideoMode(dec->width, dec->height);
				SDL_WM_SetCaption("Mediaplay", "Mediaplay");
				break;

			default:
				break;

		}
	}

	for(;;){

		if(is->abort_request)
			break;


	}


	while(!is->abort_request){	//wait until the end;

		SDL_Delay(100);
	}




	ret = 0;	// Õý³£ÍË³ö
fail:

	if(is->ic){
		av_close_input_file(is->ic);
		is->ic = NULL;
	}

	if(ret  != 0){

		SDL_Event event;

		event.type = FF_QUIT_EVENT;
		event.user.data1 = is;
		SDL_PushEvent(&event);
	}


	return 0;
}


static VideoState* stream_open(const char* filename,
					AVInputFormat* iformat){

	VideoState* is;

	is = av_mallocz(sizeof(VideoState));

	if(!is)
		return NULL;

	pstrcpy(is->filename,sizeof(is->filename),filename);

	is->audio_decoder_mutex = SDL_CreateMutex();
	is->video_decoder_mutex = SDL_CreateMutex();

	is->parse_tid =SDL_CreateThread(decode_thread, is);

	if(!is->parse_tid){
		av_free(is);
		return NULL;
	}
	return is;
}


static void stream_close(VideoState* is){

	VideoPicture* vp;

	is->abort_request = 1;
	SDL_WaitThread(is->parse_tid,NULL);


	SDL_DestroyMutex(is->audio_decoder_mutex);
	SDL_DestroyMutex(is->video_decoder_mutex);

	av_free(is);

}

void do_exit(void){

	if(curStream){

		stream_close(curStream);
		curStream = NULL;
	}

	SDL_Quit();
	exit(0);
}


void event_loop(void){

	SDL_Event event;

	for(;;){

		printf("wait for event\n");
		SDL_WaitEvent(&event);

		printf("wait for event %d\n",event.type);
		switch(event.type){


			case SDL_KEYDOWN:
				switch(event.key.keysym.sym ){
					case SDLK_ESCAPE:
					case SDLK_q:
						do_exit();
						break;
					default:
						break;

				}
				break;
			case SDL_QUIT:
			case FF_QUIT_EVENT:
				do_exit();
				break;
			default:
				break;

		}
	}
}

int main(void) {


	int flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;

	inputFile = "clock_320.avi";

	av_register_all();

	if(SDL_Init(flags)){
		exit(1);
	}

	SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	curStream = stream_open(inputFile, gInputFormat);

	event_loop();

	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
