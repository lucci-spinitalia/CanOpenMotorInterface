################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/appli.c \
/home/lucciu/CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/objdict.c 

OBJS += \
./CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/appli.o \
./CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/objdict.o 

C_DEPS += \
./CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/appli.d \
./CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/objdict.d 


# Each subdirectory must supply rules for building sources it contributes
CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/appli.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/appli.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/objdict.o: /home/lucciu/CanFestival-3-7740ac6fdedc/examples/gene_SYNC_HCS12/objdict.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/lucciu/CanFestival-3-7740ac6fdedc/include -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/timers_unix -I/home/lucciu/CanFestival-3-7740ac6fdedc/include/unix -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


