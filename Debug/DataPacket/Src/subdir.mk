################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DataPacket/Src/crc8.c \
../DataPacket/Src/dataPacketRx.c \
../DataPacket/Src/dataPacketTx.c 

OBJS += \
./DataPacket/Src/crc8.o \
./DataPacket/Src/dataPacketRx.o \
./DataPacket/Src/dataPacketTx.o 

C_DEPS += \
./DataPacket/Src/crc8.d \
./DataPacket/Src/dataPacketRx.d \
./DataPacket/Src/dataPacketTx.d 


# Each subdirectory must supply rules for building sources it contributes
DataPacket/Src/%.o DataPacket/Src/%.su: ../DataPacket/Src/%.c DataPacket/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/App/Inc" -I"C:/Users/Otavi/Documents/Embed/STM32/Projects/CurrentController/DataPacket/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DataPacket-2f-Src

clean-DataPacket-2f-Src:
	-$(RM) ./DataPacket/Src/crc8.d ./DataPacket/Src/crc8.o ./DataPacket/Src/crc8.su ./DataPacket/Src/dataPacketRx.d ./DataPacket/Src/dataPacketRx.o ./DataPacket/Src/dataPacketRx.su ./DataPacket/Src/dataPacketTx.d ./DataPacket/Src/dataPacketTx.o ./DataPacket/Src/dataPacketTx.su

.PHONY: clean-DataPacket-2f-Src

