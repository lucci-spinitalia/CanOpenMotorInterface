################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/test_copcican_linux/test_copcican_linux.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/examples/test_copcican_linux/test_copcican_linux.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/examples/test_copcican_linux/test_copcican_linux.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/examples/test_copcican_linux/test_copcican_linux.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/test_copcican_linux/test_copcican_linux.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


