################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../CANOpenShell.o 

C_SRCS += \
../CANOpenShell.c \
../CANOpenShellMasterError.c \
../CANOpenShellMasterOD.c \
../CANOpenShellStateMachine.c 

OBJS += \
./CANOpenShell.o \
./CANOpenShellMasterError.o \
./CANOpenShellMasterOD.o \
./CANOpenShellStateMachine.o 

C_DEPS += \
./CANOpenShell.d \
./CANOpenShellMasterError.d \
./CANOpenShellMasterOD.d \
./CANOpenShellStateMachine.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


