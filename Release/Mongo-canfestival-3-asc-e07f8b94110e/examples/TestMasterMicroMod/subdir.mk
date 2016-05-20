################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMaster.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMasterMicroMod.o 

C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMaster.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMasterMicroMod.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/getopt.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMaster.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMasterMicroMod.o \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/getopt.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMaster.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMasterMicroMod.d \
./Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/getopt.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMaster.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMaster.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMasterMicroMod.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/TestMasterMicroMod.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/getopt.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/examples/TestMasterMicroMod/getopt.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/unix -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include -I/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/include/timers_unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


