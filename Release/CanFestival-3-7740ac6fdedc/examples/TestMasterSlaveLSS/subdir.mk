################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/Master.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveA.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveB.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/getopt.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/Master.o \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveA.o \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveB.o \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.o \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/getopt.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/Master.d \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveA.d \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveB.d \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.d \
./CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/getopt.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/Master.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/Master.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveA.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveA.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveB.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/SlaveB.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/TestMasterSlaveLSS.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/getopt.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/TestMasterSlaveLSS/getopt.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


