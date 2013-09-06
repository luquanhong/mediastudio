CODEC_PATH	= $(PROJECT_PATH)/src/codecs/decoder/video/hw_stagefright_honeycomb
CODEC_R_PATH := src/codecs/decoder/video/hw_stagefright_honeycomb

OEM_INC	+= $(CODEC_PATH)/OEM/include/media/libstagefright \
		$(CODEC_PATH)/OEM/include/include/media \
		$(CODEC_PATH)/OEM/include/include \
		$(CODEC_PATH)/OEM/include/include/utils \
		$(CODEC_PATH)/OEM/include/include/media/stagefright \
		$(CODEC_PATH)/OEM/include/include/media/stagefright/openmax \
		$(CODEC_PATH)/OEM/include/frameworks/base/core/jni \
		$(CODEC_PATH)/OEM/include/hardware/libhardware/include \
		$(CODEC_PATH)/OEM/include/system/core/include \
		$(CODEC_PATH)/OEM/include/skia/include \
		$(CODEC_PATH)/OEM/include/pvmi/pvmf/include \
		$(CODEC_PATH)/OEM/include/extern_libs_v2/khronos/openmax/include \
		$(CODEC_PATH)/um_stagefright/inc 
	
		
OEM_SRC += $(CODEC_R_PATH)/um_video_dec.cpp	\
		$(CODEC_R_PATH)/um_stagefright/src/UM_MediaSource.cpp	\
		$(CODEC_R_PATH)/um_stagefright/src/UM_MediaPlayer.cpp	


LFLAGS		+=

OEM_LDLIBS	+= -L$(CODEC_PATH)/OEM/libs  -llog  -lcutils -licuuc -lvorbisidec -lui -ldl -lbinder -landroid_runtime -lstagefright -lstagefright_omx -lstagefright_foundation -lmedia -lstagefright_avc_common -lstagefright_amrnb_common -lutils -lsurfaceflinger_client
