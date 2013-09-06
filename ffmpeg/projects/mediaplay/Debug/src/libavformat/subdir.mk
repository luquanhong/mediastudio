################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/libavformat/allformats.c \
../src/libavformat/avio.c \
../src/libavformat/aviobuf.c \
../src/libavformat/cutils.c \
../src/libavformat/file.c \
../src/libavformat/utils_format.c 

OBJS += \
./src/libavformat/allformats.o \
./src/libavformat/avio.o \
./src/libavformat/aviobuf.o \
./src/libavformat/cutils.o \
./src/libavformat/file.o \
./src/libavformat/utils_format.o 

C_DEPS += \
./src/libavformat/allformats.d \
./src/libavformat/avio.d \
./src/libavformat/aviobuf.d \
./src/libavformat/cutils.d \
./src/libavformat/file.d \
./src/libavformat/utils_format.d 


# Each subdirectory must supply rules for building sources it contributes
src/libavformat/%.o: ../src/libavformat/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -D__WIN32__ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


