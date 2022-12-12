################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Pong/pong.c 

OBJS += \
./Core/Pong/pong.o 

C_DEPS += \
./Core/Pong/pong.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Pong/%.o Core/Pong/%.su: ../Core/Pong/%.c Core/Pong/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"S:/STM32/1er_STM32/Drivers/music" -I"S:/STM32/1er_STM32/Drivers/MAX7219" -I"S:/STM32/1er_STM32/Drivers/LED-Array" -I"S:/STM32/1er_STM32/Core/Pong" -I"S:/STM32/1er_STM32/Drivers/CMSIS" -I"S:/STM32/1er_STM32/Drivers/Timer" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Pong

clean-Core-2f-Pong:
	-$(RM) ./Core/Pong/pong.d ./Core/Pong/pong.o ./Core/Pong/pong.su

.PHONY: clean-Core-2f-Pong

