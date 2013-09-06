
# Build the unit tests.
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

PROJECT_ROOT=../
NDK :=  /cygdrive/c/ndk/android-ndk-r8

LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -L$(LOCAL_PATH)/libs -lOpenMAXAL -lOpenSLES -lvorbisidec -lz -lutils -lsysutils -lstagefright_foundation -landroid -lcutils -licuuc -lui -ldl -lbinder -landroid_runtime -lstagefright -lstagefright_omx -lmedia -lstagefright_avc_common -lstagefright_amrnb_common -lsurfaceflinger -lgui 

STLPORT_FORCE_REBUILD := true


LOCAL_MODULE := test

LOCAL_MODULE_TAGS := tests

LOCAL_SHARED_LIBRARIES := \
    libutils

LOCAL_SRC_FILES :=	 \
    	test-jni.cpp \
    	libstagefright.cpp 


LOCAL_C_INCLUDES :=		\
		$(LOCAL_PATH)/include/frameworks/base/include \
		$(LOCAL_PATH)/include/frameworks/base/core/jni \
		$(LOCAL_PATH)/include/frameworks/native/include \
		$(LOCAL_PATH)/include/frameworks/native/include/media/openmax \
		$(LOCAL_PATH)/include/frameworks/native/include/gui \
		$(LOCAL_PATH)/include/frameworks/native/include/media/hardware \
		$(LOCAL_PATH)/include/frameworks/native/opengl/include	\
		$(LOCAL_PATH)/include/frameworks/av/include \
		$(LOCAL_PATH)/include/frameworks/av/media/libstagefright \
		$(LOCAL_PATH)/include/frameworks/av/media/libstagefright/include \
		$(LOCAL_PATH)/include/hardware/libhardware/include \
		$(LOCAL_PATH)/include/system/core/include \
		$(LOCAL_PATH)/include/libnativehelper/include \
		$(NDK)/sources/cxx-stl/gnu-libstdc++/include \
		$(NDK)/sources/cxx-stl/gnu-libstdc++/libs/armeabi-v7a/include
		
	
#$(warning $(LOCAL_C_INCLUDES))

LOCAL_CXXFLAGS := -Wno-multichar -fno-exceptions -fno-rtti



include $(BUILD_SHARED_LIBRARY)
