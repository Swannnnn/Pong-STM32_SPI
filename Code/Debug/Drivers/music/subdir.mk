################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/music/music.c 

OBJS += \
./Drivers/music/music.o 

C_DEPS += \
./Drivers/music/music.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/music/%.o Drivers/music/%.su: ../Drivers/music/%.c Drivers/music/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"S:/STM32/1er_STM32/Drivers/music" -I"S:/STM32/1er_STM32/Drivers/MAX7219" -I"S:/STM32/1er_STM32/Drivers/LED-Array" -I"S:/STM32/1er_STM32/Core/Pong" -I"S:/STM32/1er_STM32/Drivers/CMSIS" -I"S:/STM32/1er_STM32/Drivers/Timer" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-music

clean-Drivers-2f-music:
	-$(RM) ./Drivers/music/music.d ./Drivers/music/music.o ./Drivers/music/music.su

.PHONY: clean-Drivers-2f-music

