################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/libavcodec/utils_codec.c 

OBJS += \
./src/libavcodec/utils_codec.o 

C_DEPS += \
./src/libavcodec/utils_codec.d 


# Each subdirectory must supply rules for building sources it contributes
src/libavcodec/%.o: ../src/libavcodec/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -D__WIN32__ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


