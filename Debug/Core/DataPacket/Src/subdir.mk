################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/DataPacket/Src/crc8.c \
../Core/DataPacket/Src/dataPacketRx.c \
../Core/DataPacket/Src/dataPacketTx.c 

OBJS += \
./Core/DataPacket/Src/crc8.o \
./Core/DataPacket/Src/dataPacketRx.o \
./Core/DataPacket/Src/dataPacketTx.o 

C_DEPS += \
./Core/DataPacket/Src/crc8.d \
./Core/DataPacket/Src/dataPacketRx.d \
./Core/DataPacket/Src/dataPacketTx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/DataPacket/Src/%.o Core/DataPacket/Src/%.su: ../Core/DataPacket/Src/%.c Core/DataPacket/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-DataPacket-2f-Src

clean-Core-2f-DataPacket-2f-Src:
	-$(RM) ./Core/DataPacket/Src/crc8.d ./Core/DataPacket/Src/crc8.o ./Core/DataPacket/Src/crc8.su ./Core/DataPacket/Src/dataPacketRx.d ./Core/DataPacket/Src/dataPacketRx.o ./Core/DataPacket/Src/dataPacketRx.su ./Core/DataPacket/Src/dataPacketTx.d ./Core/DataPacket/Src/dataPacketTx.o ./Core/DataPacket/Src/dataPacketTx.su

.PHONY: clean-Core-2f-DataPacket-2f-Src

