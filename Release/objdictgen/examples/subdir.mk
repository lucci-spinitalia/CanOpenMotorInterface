################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../objdictgen/examples/example_objdict.c 

OBJS += \
./objdictgen/examples/example_objdict.o 

C_DEPS += \
./objdictgen/examples/example_objdict.d 


# Each subdirectory must supply rules for building sources it contributes
objdictgen/examples/%.o: ../objdictgen/examples/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


