################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/drivers/timers_win32/timers_win32.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/drivers/timers_win32/timers_win32.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/drivers/timers_win32/timers_win32.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/drivers/timers_win32/timers_win32.o: /home/lucciu/CanFestival-3-7740ac6fdedc/drivers/timers_win32/timers_win32.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


