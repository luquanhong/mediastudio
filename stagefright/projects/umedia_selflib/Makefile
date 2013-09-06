SYSROOT=/opt/android_build/sysroot

cc=/opt/android_build/bin/arm-linux-androideabi-gcc  --sysroot=$(SYSROOT)


CFLAGS = -g -O2 -fPIC -DANDROID  -DHAVE_PTHREADS -w -DENABLE_LOG

CFLAGS += -IOEM/include/media/libstagefright \
		-IOEM/include/include/media \
		-IOEM/include/include \
		-IOEM/include/include/utils \
		-IOEM/include/include/media/stagefright \
		-IOEM/include/include/media/stagefright/openmax \
		-IOEM/include/frameworks/base/core/jni \
		-IOEM/include/hardware/libhardware/include \
		-IOEM/include/system/core/include \
		-IOEM/include/skia/include \
		-IOEM/include/pvmi/pvmf/include \
		-IOEM/include/extern_libs_v2/khronos/openmax/include \
		-I/opt/android_build/arm-linux-androideabi/include/c++/4.6 \
		-I/opt/android_build/arm-linux-androideabi/include/c++/4.6/arm-linux-androideabi/armv7-a	\
		-Iinclude

SDFLAGS = -nostdlib -Wl,-shared -L/opt/android_build/sysroot/usr/lib  -lgcc -lc -lm  -ldl 
#-lc -lsupc++ -lz -lm  -ldl 
#SDFLAGS = -nostdlib -Wl,-T,armelf.xsc -Wl,-Bsymbolic -Wl,-soname,$@ -lc

SDFLAGS += -Wno-multichar -fno-exceptions -fno-rtti 
SDFLAGS +=  -L/mnt/hgfs/GameClound/googleTV/native/test/OEM/libs -lOpenMAXAL -lOpenSLES -lvorbisidec -lz -lutils -lsysutils -lstagefright_foundation -landroid -lcutils -licuuc -lui -ldl -lbinder -landroid_runtime -lstagefright -lstagefright_omx -lmedia -lstagefright_avc_common -lstagefright_amrnb_common -lsurfaceflinger_client -lstdc++
#-Wl,-rpath-link=/mnt/hgfs/GameClound/googleTV/native/test/OEM/libs

OBJECT= um_sys_event.o \
	um_sys_mutex.o \
	um_circle_list.o \
	um_circle_queue.o \
	um_mtm.o	\
	um_video_dec.o	\
	UM_MediaSource.o	\
	UM_MediaPlayer.o	

all : main.o libubmedia.so	
	$(cc) $(CFLAGS) $(SDFLAGS) -o main main.o  -L. -lubmedia
main.o : main.c
	$(cc) $(CFLAGS) $(SDFLAGS) -o main.o -c main.c

libubmedia.so : $(OBJECT)
	$(cc) $(SDFLAGS) -o  $@ $^ $(CRT_OBJS)
um_sys_event.o : um_sys_event.cpp
	$(cc) $(CFLAGS) $(SDFLAGS) -c um_sys_event.cpp
um_sys_mutex.o : um_sys_mutex.cpp
	$(cc) $(CFLAGS) $(SDFLAGS) -c um_sys_mutex.cpp
um_sys_thread.o:um_sys_thread.cpp
	$(cc) $(CFLAGS) $(SDFLAGS) -c um_sys_thread.cpp
um_circle_list.o:um_circle_list.cpp
	$(cc) $(CFLAGS) $(SDFLAGS) -c um_circle_list.cpp
um_circle_queue.o:um_circle_queue.cpp
	$(cc) $(CFLAGS) $(SDFLAGS) -c um_circle_queue.cpp
um_mtm.o:um_mtm.cpp
	$(cc) $(CFLAGS) $(SDFLAGS) -c um_mtm.cpp
um_video_dec.o:um_video_dec.cpp
	$(cc) $(CFLAGS) $(SDFLAGS) -c um_video_dec.cpp
UM_MediaSource.o:UM_MediaSource.cpp
	$(cc) $(CFLAGS) $(SDFLAGS) -c UM_MediaSource.cpp
UM_MediaPlayer.o:UM_MediaPlayer.cpp
	$(cc) $(CFLAGS) $(SDFLAGS) -c UM_MediaPlayer.cpp
	
clean:
	rm -rf *.o libubmedia.so
