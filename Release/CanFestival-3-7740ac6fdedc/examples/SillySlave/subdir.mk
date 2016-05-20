################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/SillySlave/SillySlave.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/SillySlave/main.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/SillySlave/slave.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/examples/SillySlave/SillySlave.o \
./CanFestival-3-7740ac6fdedc/examples/SillySlave/main.o \
./CanFestival-3-7740ac6fdedc/examples/SillySlave/slave.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/examples/SillySlave/SillySlave.d \
./CanFestival-3-7740ac6fdedc/examples/SillySlave/main.d \
./CanFestival-3-7740ac6fdedc/examples/SillySlave/slave.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/examples/SillySlave/SillySlave.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/SillySlave/SillySlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/SillySlave/main.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/SillySlave/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/SillySlave/slave.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/SillySlave/slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


