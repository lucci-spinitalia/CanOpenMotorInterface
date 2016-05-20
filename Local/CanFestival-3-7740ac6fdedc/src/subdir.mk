################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_dcf.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_emcy.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_lifegrd.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_nmtMaster.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_nmtSlave.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_objacces.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_pdo.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_sdo.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_states.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_sync.o \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/unix_timer.o 

C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/dcf.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/emcy.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/lifegrd.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/lss.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/nmtMaster.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/nmtSlave.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/objacces.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/pdo.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/sdo.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/states.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/sync.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/src/timer.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/src/dcf.o \
./CanFestival-3-7740ac6fdedc/src/emcy.o \
./CanFestival-3-7740ac6fdedc/src/lifegrd.o \
./CanFestival-3-7740ac6fdedc/src/lss.o \
./CanFestival-3-7740ac6fdedc/src/nmtMaster.o \
./CanFestival-3-7740ac6fdedc/src/nmtSlave.o \
./CanFestival-3-7740ac6fdedc/src/objacces.o \
./CanFestival-3-7740ac6fdedc/src/pdo.o \
./CanFestival-3-7740ac6fdedc/src/sdo.o \
./CanFestival-3-7740ac6fdedc/src/states.o \
./CanFestival-3-7740ac6fdedc/src/sync.o \
./CanFestival-3-7740ac6fdedc/src/timer.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/src/dcf.d \
./CanFestival-3-7740ac6fdedc/src/emcy.d \
./CanFestival-3-7740ac6fdedc/src/lifegrd.d \
./CanFestival-3-7740ac6fdedc/src/lss.d \
./CanFestival-3-7740ac6fdedc/src/nmtMaster.d \
./CanFestival-3-7740ac6fdedc/src/nmtSlave.d \
./CanFestival-3-7740ac6fdedc/src/objacces.d \
./CanFestival-3-7740ac6fdedc/src/pdo.d \
./CanFestival-3-7740ac6fdedc/src/sdo.d \
./CanFestival-3-7740ac6fdedc/src/states.d \
./CanFestival-3-7740ac6fdedc/src/sync.d \
./CanFestival-3-7740ac6fdedc/src/timer.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/src/dcf.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/dcf.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/emcy.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/emcy.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/lifegrd.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/lifegrd.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/lss.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/lss.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/nmtMaster.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/nmtMaster.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/nmtSlave.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/nmtSlave.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/objacces.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/objacces.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/pdo.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/pdo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/sdo.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/sdo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/states.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/states.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/sync.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/sync.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/src/timer.o: /home/lucciu/CanFestival-3-7740ac6fdedc/src/timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


