################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/can_AVR.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/timer_AVR.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/can_AVR.o \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/timer_AVR.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/can_AVR.d \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/timer_AVR.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/can_AVR.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/can_AVR.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/timer_AVR.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/AVR/timer_AVR.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


