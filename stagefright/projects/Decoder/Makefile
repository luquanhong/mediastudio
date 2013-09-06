SYSROOT=/opt/android_build/sysroot

cc=/opt/android_build/bin/arm-linux-androideabi-g++  --sysroot=$(SYSROOT)


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

SDFLAGS = -nostdlib -Wl,-shared -L/opt/android_build/sysroot/usr/lib   -lc -lm  -ldl 
SDFLAGS += -L/opt/android_build/lib/gcc/arm-linux-androideabi/4.6.x-google/armv7-a -lgcc
#-lgcc

SDFLAGS += -fno-exceptions -fno-rtti
#-Wno-multichar
SDFLAGS +=  -LOEM/libs -lOpenMAXAL -lOpenSLES -lvorbisidec -lz -lutils  -lstagefright_foundation -landroid -lcutils -licuuc -lui -ldl -lbinder -landroid_runtime -lstagefright -lstagefright_omx -lmedia -lstagefright_avc_common -lstagefright_amrnb_common -lsurfaceflinger_client -lstdc++
#-Wl,-rpath-link=/mnt/hgfs/GameClound/googleTV/native/umedia_selflib/OEM/libs

OBJECT= UM_GtvDecoder.o

libtest-ubmedia.so : $(OBJECT)
	$(cc) $(SDFLAGS) -o  $@ $^ $(CRT_OBJS)
UM_GtvDecoder.o : UM_GtvDecoder.cpp
	$(cc) $(CFLAGS) -c UM_GtvDecoder.cpp

	
clean:
	rm -rf *.o libubmedia.so
