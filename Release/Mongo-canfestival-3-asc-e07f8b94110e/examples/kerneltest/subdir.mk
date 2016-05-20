################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Master.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Slave.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMaster.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMasterSlave.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestSlave.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/kernel_module.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Master.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Slave.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMaster.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMasterSlave.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestSlave.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/kernel_module.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Master.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Slave.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMaster.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMasterSlave.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestSlave.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/kernel_module.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Master.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Slave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/Slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMaster.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMaster.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMasterSlave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestMasterSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestSlave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/TestSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/kernel_module.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/kerneltest/kernel_module.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


