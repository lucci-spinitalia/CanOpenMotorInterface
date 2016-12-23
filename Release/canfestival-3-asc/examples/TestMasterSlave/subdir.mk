################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/Master.o \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/Slave.o \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/TestMaster.o \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/TestMasterSlave.o \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/TestSlave.o 

C_SRCS += \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/Master.c \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/Slave.c \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/TestMaster.c \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/TestMasterSlave.c \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/TestSlave.c \
/home/lucciu/canfestival-3-asc/examples/TestMasterSlave/getopt.c 

OBJS += \
./canfestival-3-asc/examples/TestMasterSlave/Master.o \
./canfestival-3-asc/examples/TestMasterSlave/Slave.o \
./canfestival-3-asc/examples/TestMasterSlave/TestMaster.o \
./canfestival-3-asc/examples/TestMasterSlave/TestMasterSlave.o \
./canfestival-3-asc/examples/TestMasterSlave/TestSlave.o \
./canfestival-3-asc/examples/TestMasterSlave/getopt.o 

C_DEPS += \
./canfestival-3-asc/examples/TestMasterSlave/Master.d \
./canfestival-3-asc/examples/TestMasterSlave/Slave.d \
./canfestival-3-asc/examples/TestMasterSlave/TestMaster.d \
./canfestival-3-asc/examples/TestMasterSlave/TestMasterSlave.d \
./canfestival-3-asc/examples/TestMasterSlave/TestSlave.d \
./canfestival-3-asc/examples/TestMasterSlave/getopt.d 


# Each subdirectory must supply rules for building sources it contributes
canfestival-3-asc/examples/TestMasterSlave/Master.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlave/Master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/TestMasterSlave/Slave.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlave/Slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/TestMasterSlave/TestMaster.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlave/TestMaster.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/TestMasterSlave/TestMasterSlave.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlave/TestMasterSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/TestMasterSlave/TestSlave.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlave/TestSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/TestMasterSlave/getopt.o: /home/lucciu/canfestival-3-asc/examples/TestMasterSlave/getopt.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


