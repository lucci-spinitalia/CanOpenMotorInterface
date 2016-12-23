################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/Master.c \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveA.c \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveB.c \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.c \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/getopt.c 

OBJS += \
./canfestival-3-asc/examples/TestMasterSlaveLSS/Master.o \
./canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveA.o \
./canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveB.o \
./canfestival-3-asc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.o \
./canfestival-3-asc/examples/TestMasterSlaveLSS/getopt.o 

C_DEPS += \
./canfestival-3-asc/examples/TestMasterSlaveLSS/Master.d \
./canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveA.d \
./canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveB.d \
./canfestival-3-asc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.d \
./canfestival-3-asc/examples/TestMasterSlaveLSS/getopt.d 


# Each subdirectory must supply rules for building sources it contributes
canfestival-3-asc/examples/TestMasterSlaveLSS/Master.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/Master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveA.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveA.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveB.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/SlaveB.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/TestMasterSlaveLSS/getopt.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlaveLSS/getopt.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


