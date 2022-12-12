################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/MAX7219/max7219.c 

OBJS += \
./Drivers/MAX7219/max7219.o 

C_DEPS += \
./Drivers/MAX7219/max7219.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/MAX7219/%.o Drivers/MAX7219/%.su: ../Drivers/MAX7219/%.c Drivers/MAX7219/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"S:/STM32/1er_STM32/Drivers/music" -I"S:/STM32/1er_STM32/Drivers/MAX7219" -I"S:/STM32/1er_STM32/Drivers/LED-Array" -I"S:/STM32/1er_STM32/Core/Pong" -I"S:/STM32/1er_STM32/Drivers/CMSIS" -I"S:/STM32/1er_STM32/Drivers/Timer" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-MAX7219

clean-Drivers-2f-MAX7219:
	-$(RM) ./Drivers/MAX7219/max7219.d ./Drivers/MAX7219/max7219.o ./Drivers/MAX7219/max7219.su

.PHONY: clean-Drivers-2f-MAX7219

