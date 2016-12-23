################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/canfestival-3-asc/examples/AVR/Slave/ObjDict.c \
/home/lucciu/canfestival-3-asc/examples/AVR/Slave/ds401.c \
/home/lucciu/canfestival-3-asc/examples/AVR/Slave/main.c 

OBJS += \
./canfestival-3-asc/examples/AVR/Slave/ObjDict.o \
./canfestival-3-asc/examples/AVR/Slave/ds401.o \
./canfestival-3-asc/examples/AVR/Slave/main.o 

C_DEPS += \
./canfestival-3-asc/examples/AVR/Slave/ObjDict.d \
./canfestival-3-asc/examples/AVR/Slave/ds401.d \
./canfestival-3-asc/examples/AVR/Slave/main.d 


# Each subdirectory must supply rules for building sources it contributes
canfestival-3-asc/examples/AVR/Slave/ObjDict.o: /home/lucciu/canfestival-3-asc/examples/AVR/Slave/ObjDict.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/AVR/Slave/ds401.o: /home/lucciu/canfestival-3-asc/examples/AVR/Slave/ds401.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

canfestival-3-asc/examples/AVR/Slave/main.o: /home/lucciu/canfestival-3-asc/examples/AVR/Slave/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/canfestival-3-asc/include -I/usr/src/linux-headers-4.4.0-45-generic/include -I/usr/src/linux-headers-4.4.0-45-generic/include/linux -I/usr/src/linux-headers-4.4.0-45-generic/include/asm -I/home/lucciu/canfestival-3-asc/include/unix -I/home/lucciu/canfestival-3-asc/include/timers_unix -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


