################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/canfestival-3-asc/examples/linux/dcf/gendcf.c \
/home/lucciu/canfestival-3-asc/examples/linux/dcf/master.c \
/home/lucciu/canfestival-3-asc/examples/linux/dcf/masterdic.c \
/home/lucciu/canfestival-3-asc/examples/linux/dcf/slave.c \
/home/lucciu/canfestival-3-asc/examples/linux/dcf/slavedic.c 

OBJS += \
./canfestival-3-asc/examples/linux/dcf/gendcf.o \
./canfestival-3-asc/examples/linux/dcf/master.o \
./canfestival-3-asc/examples/linux/dcf/masterdic.o \
./canfestival-3-asc/examples/linux/dcf/slave.o \
./canfestival-3-asc/examples/linux/dcf/slavedic.o 

C_DEPS += \
./canfestival-3-asc/examples/linux/dcf/gendcf.d \
./canfestival-3-asc/examples/linux/dcf/master.d \
./canfestival-3-asc/examples/linux/dcf/masterdic.d \
./canfestival-3-asc/examples/linux/dcf/slave.d \
./canfestival-3-asc/examples/linux/dcf/slavedic.d 


# Each subdirectory must supply rules for building sources it contributes
canfestival-3-asc/examples/linux/dcf/gendcf.o: /home/lucciu/canfestival-3-asc/examples/linux/dcf/gendcf.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/linux/dcf/master.o: /home/lucciu/canfestival-3-asc/examples/linux/dcf/master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/linux/dcf/masterdic.o: /home/lucciu/canfestival-3-asc/examples/linux/dcf/masterdic.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/linux/dcf/slave.o: /home/lucciu/canfestival-3-asc/examples/linux/dcf/slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/linux/dcf/slavedic.o: /home/lucciu/canfestival-3-asc/examples/linux/dcf/slavedic.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


