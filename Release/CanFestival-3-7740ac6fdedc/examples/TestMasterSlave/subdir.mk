################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Master.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Slave.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMaster.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMasterSlave.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestSlave.o 

C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Master.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Slave.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMaster.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMasterSlave.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestSlave.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/getopt.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Master.o \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Slave.o \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMaster.o \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMasterSlave.o \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestSlave.o \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/getopt.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Master.d \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Slave.d \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMaster.d \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMasterSlave.d \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestSlave.d \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/getopt.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Master.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Slave.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/Slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMaster.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMaster.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMasterSlave.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestMasterSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestSlave.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/TestSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/getopt.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlave/getopt.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


