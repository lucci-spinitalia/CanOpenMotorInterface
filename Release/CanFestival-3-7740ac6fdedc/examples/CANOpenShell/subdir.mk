################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShell.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellMasterOD.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellSlaveOD.o 

C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShell.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellMasterOD.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellSlaveOD.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShell.o \
./CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellMasterOD.o \
./CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellSlaveOD.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShell.d \
./CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellMasterOD.d \
./CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellSlaveOD.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShell.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShell.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellMasterOD.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellMasterOD.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellSlaveOD.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/CANOpenShell/CANOpenShellSlaveOD.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


