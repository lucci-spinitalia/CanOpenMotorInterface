################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/Master.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveA.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveB.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/getopt.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/Master.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveA.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveB.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/getopt.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/Master.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveA.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveB.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/getopt.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/Master.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/Master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveA.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveA.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveB.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/SlaveB.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/getopt.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterSlaveLSS/getopt.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


