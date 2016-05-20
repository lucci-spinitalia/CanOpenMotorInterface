################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Mongo-canfestival-3-asc-e07f8b94110e/src/subdir.mk
-include Mongo-canfestival-3-asc-e07f8b94110e/drivers/unix/subdir.mk
-include Mongo-canfestival-3-asc-e07f8b94110e/drivers/timers_unix/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SW.33.05.02.0.0

# Tool invocations
SW.33.05.02.0.0: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	arm-linux-gnueabihf-gcc -L/home/lucciu/Mongo-canfestival-3-asc-e07f8b94110e/drivers/can_socket -o "SW.33.05.02.0.0" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) SW.33.05.02.0.0
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
