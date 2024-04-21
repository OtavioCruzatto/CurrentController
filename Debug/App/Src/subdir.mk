################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Src/app.c \
../App/Src/comm.c \
../App/Src/controller.c 

OBJS += \
./App/Src/app.o \
./App/Src/comm.o \
./App/Src/controller.o 

C_DEPS += \
./App/Src/app.d \
./App/Src/comm.d \
./App/Src/controller.d 


# Each subdirectory must supply rules for building sources it contributes
App/Src/%.o App/Src/%.su: ../App/Src/%.c App/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/App/Inc" -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/DataPacket/Inc" -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/Pid/Inc" -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/Dsp/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-Src

clean-App-2f-Src:
	-$(RM) ./App/Src/app.d ./App/Src/app.o ./App/Src/app.su ./App/Src/comm.d ./App/Src/comm.o ./App/Src/comm.su ./App/Src/controller.d ./App/Src/controller.o ./App/Src/controller.su

.PHONY: clean-App-2f-Src

