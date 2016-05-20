################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/unix/unix.o 

C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/unix/unix.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/unix/unix.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/drivers/unix/unix.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/drivers/unix/unix.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/unix/unix.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


