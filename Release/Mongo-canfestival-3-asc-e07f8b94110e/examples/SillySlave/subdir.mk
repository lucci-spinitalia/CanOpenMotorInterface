################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/SillySlave.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/main.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/slave.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/SillySlave.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/main.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/slave.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/SillySlave.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/main.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/slave.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/SillySlave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/SillySlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/main.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/slave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/SillySlave/slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


