################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_dcf.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_emcy.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_lifegrd.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_nmtMaster.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_nmtSlave.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_objacces.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_pdo.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_sdo.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_states.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_sync.o \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/unix_timer.o 

C_SRCS += \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/dcf.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/emcy.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/lifegrd.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/lss.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/nmtMaster.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/nmtSlave.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/objacces.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/pdo.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/sdo.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/states.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/sync.c \
/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/timer.c 

OBJS += \
./Mongo-canfestival-3-asc-e07f8b94110e/src/dcf.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/emcy.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/lifegrd.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/lss.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/nmtMaster.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/nmtSlave.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/objacces.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/pdo.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/sdo.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/states.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/sync.o \
./Mongo-canfestival-3-asc-e07f8b94110e/src/timer.o 

C_DEPS += \
./Mongo-canfestival-3-asc-e07f8b94110e/src/dcf.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/emcy.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/lifegrd.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/lss.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/nmtMaster.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/nmtSlave.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/objacces.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/pdo.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/sdo.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/states.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/sync.d \
./Mongo-canfestival-3-asc-e07f8b94110e/src/timer.d 


# Each subdirectory must supply rules for building sources it contributes
Mongo-canfestival-3-asc-e07f8b94110e/src/dcf.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/dcf.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/emcy.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/emcy.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/lifegrd.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/lifegrd.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/lss.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/lss.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/nmtMaster.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/nmtMaster.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/nmtSlave.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/nmtSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/objacces.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/objacces.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/pdo.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/pdo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/sdo.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/sdo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/states.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/states.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/sync.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/sync.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Mongo-canfestival-3-asc-e07f8b94110e/src/timer.o: /home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/src/timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


