################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Master.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Slave.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMaster.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMasterSlave.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestSlave.o 

C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Master.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Slave.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMaster.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMasterSlave.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestSlave.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/getopt.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Master.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Slave.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMaster.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMasterSlave.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestSlave.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/getopt.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Master.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Slave.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMaster.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMasterSlave.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestSlave.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/getopt.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Master.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Slave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/Slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMaster.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMaster.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMasterSlave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestMasterSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestSlave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/TestSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/getopt.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlave/getopt.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


