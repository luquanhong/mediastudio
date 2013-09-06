LOCAL_PATH := $(call my-dir)
PROJECT_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)


COM_INC := $(LOCAL_PATH)/include		
OEM_INC := $(LOCAL_PATH)/OEM/include/media/libstagefright \
		$(LOCAL_PATH)/OEM/include/include/media \
		$(LOCAL_PATH)/OEM/include/include \
		$(LOCAL_PATH)/OEM/include/include/utils \
		$(LOCAL_PATH)/OEM/include/include/media/stagefright \
		$(LOCAL_PATH)/OEM/include/include/media/stagefright/openmax \
		$(LOCAL_PATH)/OEM/include/frameworks/base/core/jni \
		$(LOCAL_PATH)/OEM/include/hardware/libhardware/include \
		$(LOCAL_PATH)/OEM/include/system/core/include \
		$(LOCAL_PATH)/OEM/include/skia/include \
		$(LOCAL_PATH)/OEM/include/pvmi/pvmf/include \
		$(LOCAL_PATH)/OEM/include/extern_libs_v2/khronos/openmax/include


COM_SRC := UM_GtvDecoder.cpp

OEM_LDLIBS :=  -L$(LOCAL_PATH)/OEM/libs -lOpenMAXAL -lOpenSLES -lvorbisidec -lz -lutils  -lstagefright_foundation -landroid -lcutils -licuuc -lui -ldl -lbinder -landroid_runtime -lstagefright -lstagefright_omx -lmedia -lstagefright_avc_common -lstagefright_amrnb_common -lsurfaceflinger_client 	
	
LOCAL_MODULE := libtest-ubmedia

LOCAL_CXXFLAGS := -DHAVE_PTHREADS -w -DENABLE_LOG

LOCAL_C_INCLUDES := \
		$(COM_INC)	\
		$(OEM_INC)

LOCAL_SRC_FILES := 	\
		$(COM_SRC)
	
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += $(OEM_LDLIBS)

include $(BUILD_SHARED_LIBRARY)
