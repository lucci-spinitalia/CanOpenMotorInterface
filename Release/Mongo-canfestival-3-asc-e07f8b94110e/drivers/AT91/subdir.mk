################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/can_AT91.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/timer_AT91.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/can_AT91.o \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/timer_AT91.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/can_AT91.d \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/timer_AT91.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/can_AT91.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/can_AT91.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/timer_AT91.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/AT91/timer_AT91.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


