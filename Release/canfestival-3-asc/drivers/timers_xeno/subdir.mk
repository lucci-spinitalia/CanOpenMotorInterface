################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/canfestival-3-asc/drivers/timers_xeno/timers_xeno.c 

OBJS += \
./canfestival-3-asc/drivers/timers_xeno/timers_xeno.o 

C_DEPS += \
./canfestival-3-asc/drivers/timers_xeno/timers_xeno.d 


# Each subdirectory must supply rules for building sources it contributes
canfestival-3-asc/drivers/timers_xeno/timers_xeno.o: /home/lucciu/canfestival-3-asc/drivers/timers_xeno/timers_xeno.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


