################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/canfestival-3-asc/examples/AT91/Master/ObjDict.c \
/home/lucciu/canfestival-3-asc/examples/AT91/Master/interrupt_timer.c \
/home/lucciu/canfestival-3-asc/examples/AT91/Master/main.c 

OBJS += \
./canfestival-3-asc/examples/AT91/Master/ObjDict.o \
./canfestival-3-asc/examples/AT91/Master/interrupt_timer.o \
./canfestival-3-asc/examples/AT91/Master/main.o 

C_DEPS += \
./canfestival-3-asc/examples/AT91/Master/ObjDict.d \
./canfestival-3-asc/examples/AT91/Master/interrupt_timer.d \
./canfestival-3-asc/examples/AT91/Master/main.d 


# Each subdirectory must supply rules for building sources it contributes
canfestival-3-asc/examples/AT91/Master/ObjDict.o: /home/lucciu/canfestival-3-asc/examples/AT91/Master/ObjDict.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/AT91/Master/interrupt_timer.o: /home/lucciu/canfestival-3-asc/examples/AT91/Master/interrupt_timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/AT91/Master/main.o: /home/lucciu/canfestival-3-asc/examples/AT91/Master/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


