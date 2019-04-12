################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LED.c \
../src/UART0.c \
../src/main.c \
../src/mtb.c \
../src/report.c \
../src/ring.c \
../src/semihost_hardfault.c 

OBJS += \
./src/LED.o \
./src/UART0.o \
./src/main.o \
./src/mtb.o \
./src/report.o \
./src/ring.o \
./src/semihost_hardfault.o 

C_DEPS += \
./src/LED.d \
./src/UART0.d \
./src/main.d \
./src/mtb.d \
./src/report.d \
./src/ring.d \
./src/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL25Z128VFM4 -DCPU_MKL25Z128VFM4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I../board -I../ -I../drivers -I../CMSIS -I../utilities -I../startup -I../src -I../inc -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


