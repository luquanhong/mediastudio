
# Build the unit tests.
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

PROJECT_ROOT=../

LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -L$(LOCAL_PATH) -lubmedia

$(warning $(LOCAL_LDLIBS))

LOCAL_MODULE := test

LOCAL_MODULE_TAGS := tests

LOCAL_SHARED_LIBRARIES := \
    libutils

LOCAL_SRC_FILES := \
    	test-jni.cpp


LOCAL_CFLAGS := 	\
	-I$(PROJECT_ROOT)src/utils	\
	-I$(PROJECT_ROOT)include	
	

LOCAL_CXXFLAGS := -DENABLE_LOG


include $(BUILD_SHARED_LIBRARY)
