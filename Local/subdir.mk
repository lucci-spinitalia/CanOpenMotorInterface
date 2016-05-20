################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CANOpenShell.c \
../CANOpenShellMasterError.c \
../CANOpenShellMasterOD.c \
../CANOpenShellStateMachine.c \
../file_parser.c \
../smartmotor_table.c \
../utils.c 

OBJS += \
./CANOpenShell.o \
./CANOpenShellMasterError.o \
./CANOpenShellMasterOD.o \
./CANOpenShellStateMachine.o \
./file_parser.o \
./smartmotor_table.o \
./utils.o 

C_DEPS += \
./CANOpenShell.d \
./CANOpenShellMasterError.d \
./CANOpenShellMasterOD.d \
./CANOpenShellStateMachine.d \
./file_parser.d \
./smartmotor_table.d \
./utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


