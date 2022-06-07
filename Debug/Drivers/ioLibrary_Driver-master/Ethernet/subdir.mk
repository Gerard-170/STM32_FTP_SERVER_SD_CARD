################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver-master/Ethernet/socket.c \
../Drivers/ioLibrary_Driver-master/Ethernet/wizchip_conf.c 

OBJS += \
./Drivers/ioLibrary_Driver-master/Ethernet/socket.o \
./Drivers/ioLibrary_Driver-master/Ethernet/wizchip_conf.o 

C_DEPS += \
./Drivers/ioLibrary_Driver-master/Ethernet/socket.d \
./Drivers/ioLibrary_Driver-master/Ethernet/wizchip_conf.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver-master/Ethernet/%.o Drivers/ioLibrary_Driver-master/Ethernet/%.su: ../Drivers/ioLibrary_Driver-master/Ethernet/%.c Drivers/ioLibrary_Driver-master/Ethernet/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Application/loopback" -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Application/multicast" -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Ethernet" -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Ethernet/W5500" -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Internet/FTPServer" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2d-master-2f-Ethernet

clean-Drivers-2f-ioLibrary_Driver-2d-master-2f-Ethernet:
	-$(RM) ./Drivers/ioLibrary_Driver-master/Ethernet/socket.d ./Drivers/ioLibrary_Driver-master/Ethernet/socket.o ./Drivers/ioLibrary_Driver-master/Ethernet/socket.su ./Drivers/ioLibrary_Driver-master/Ethernet/wizchip_conf.d ./Drivers/ioLibrary_Driver-master/Ethernet/wizchip_conf.o ./Drivers/ioLibrary_Driver-master/Ethernet/wizchip_conf.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2d-master-2f-Ethernet

