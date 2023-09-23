################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Pid/Src/pid.c 

OBJS += \
./Pid/Src/pid.o 

C_DEPS += \
./Pid/Src/pid.d 


# Each subdirectory must supply rules for building sources it contributes
Pid/Src/%.o Pid/Src/%.su: ../Pid/Src/%.c Pid/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/App/Inc" -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/DataPacket/Inc" -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/Pid/Inc" -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/Dsp/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Pid-2f-Src

clean-Pid-2f-Src:
	-$(RM) ./Pid/Src/pid.d ./Pid/Src/pid.o ./Pid/Src/pid.su

.PHONY: clean-Pid-2f-Src

