################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/ObjDict.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/interrupt_timer.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/main.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/ObjDict.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/interrupt_timer.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/main.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/ObjDict.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/interrupt_timer.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/main.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/ObjDict.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/ObjDict.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/interrupt_timer.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/interrupt_timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/main.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/AT91/Master/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


