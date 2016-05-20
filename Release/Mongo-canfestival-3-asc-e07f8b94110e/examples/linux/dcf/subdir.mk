################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/gendcf.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/master.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/masterdic.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slave.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slavedic.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/gendcf.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/master.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/masterdic.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slave.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slavedic.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/gendcf.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/master.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/masterdic.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slave.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slavedic.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/gendcf.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/gendcf.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/master.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/masterdic.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/masterdic.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slavedic.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/linux/dcf/slavedic.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


