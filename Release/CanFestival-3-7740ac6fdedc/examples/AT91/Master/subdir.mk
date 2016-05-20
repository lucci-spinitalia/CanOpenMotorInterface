################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/AT91/Master/ObjDict.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/AT91/Master/interrupt_timer.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/AT91/Master/main.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/examples/AT91/Master/ObjDict.o \
./CanFestival-3-7740ac6fdedc/examples/AT91/Master/interrupt_timer.o \
./CanFestival-3-7740ac6fdedc/examples/AT91/Master/main.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/examples/AT91/Master/ObjDict.d \
./CanFestival-3-7740ac6fdedc/examples/AT91/Master/interrupt_timer.d \
./CanFestival-3-7740ac6fdedc/examples/AT91/Master/main.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/examples/AT91/Master/ObjDict.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/AT91/Master/ObjDict.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/AT91/Master/interrupt_timer.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/AT91/Master/interrupt_timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/AT91/Master/main.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/AT91/Master/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


