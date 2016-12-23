################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/canfestival-3-asc/examples/kerneltest/Master.c \
/home/lucciu/canfestival-3-asc/examples/kerneltest/Slave.c \
/home/lucciu/canfestival-3-asc/examples/kerneltest/TestMaster.c \
/home/lucciu/canfestival-3-asc/examples/kerneltest/TestMasterSlave.c \
/home/lucciu/canfestival-3-asc/examples/kerneltest/TestSlave.c \
/home/lucciu/canfestival-3-asc/examples/kerneltest/kernel_module.c 

OBJS += \
./canfestival-3-asc/examples/kerneltest/Master.o \
./canfestival-3-asc/examples/kerneltest/Slave.o \
./canfestival-3-asc/examples/kerneltest/TestMaster.o \
./canfestival-3-asc/examples/kerneltest/TestMasterSlave.o \
./canfestival-3-asc/examples/kerneltest/TestSlave.o \
./canfestival-3-asc/examples/kerneltest/kernel_module.o 

C_DEPS += \
./canfestival-3-asc/examples/kerneltest/Master.d \
./canfestival-3-asc/examples/kerneltest/Slave.d \
./canfestival-3-asc/examples/kerneltest/TestMaster.d \
./canfestival-3-asc/examples/kerneltest/TestMasterSlave.d \
./canfestival-3-asc/examples/kerneltest/TestSlave.d \
./canfestival-3-asc/examples/kerneltest/kernel_module.d 


# Each subdirectory must supply rules for building sources it contributes
canfestival-3-asc/examples/kerneltest/Master.o: /home/lucciu/canfestival-3-asc/examples/kerneltest/Master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/kerneltest/Slave.o: /home/lucciu/canfestival-3-asc/examples/kerneltest/Slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/kerneltest/TestMaster.o: /home/lucciu/canfestival-3-asc/examples/kerneltest/TestMaster.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/kerneltest/TestMasterSlave.o: /home/lucciu/canfestival-3-asc/examples/kerneltest/TestMasterSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/kerneltest/TestSlave.o: /home/lucciu/canfestival-3-asc/examples/kerneltest/TestSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/kerneltest/kernel_module.o: /home/lucciu/canfestival-3-asc/examples/kerneltest/kernel_module.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


