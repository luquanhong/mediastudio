#include "stdio.h"
#include "um_video_dec.h"

int main(){

	CodecParams params;

	um_vdec_create(&params);

	printf("hello world\n");
	
	return 0;
}
