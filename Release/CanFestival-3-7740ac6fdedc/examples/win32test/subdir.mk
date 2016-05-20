################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/win32test/main.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/win32test/win32test.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/examples/win32test/main.o \
./CanFestival-3-7740ac6fdedc/examples/win32test/win32test.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/examples/win32test/main.d \
./CanFestival-3-7740ac6fdedc/examples/win32test/win32test.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/examples/win32test/main.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/win32test/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/win32test/win32test.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/win32test/win32test.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


