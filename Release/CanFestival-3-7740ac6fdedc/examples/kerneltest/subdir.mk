################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/Master.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/Slave.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMaster.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMasterSlave.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/TestSlave.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/kernel_module.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/Master.o \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/Slave.o \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMaster.o \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMasterSlave.o \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/TestSlave.o \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/kernel_module.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/Master.d \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/Slave.d \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMaster.d \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMasterSlave.d \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/TestSlave.d \
./CanFestival-3-7740ac6fdedc/examples/kerneltest/kernel_module.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/examples/kerneltest/Master.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/Master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/kerneltest/Slave.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/Slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMaster.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMaster.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMasterSlave.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/TestMasterSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/kerneltest/TestSlave.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/TestSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/kerneltest/kernel_module.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/kerneltest/kernel_module.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


