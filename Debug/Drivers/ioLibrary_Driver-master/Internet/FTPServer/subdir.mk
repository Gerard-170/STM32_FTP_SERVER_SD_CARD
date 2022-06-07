################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver-master/Internet/FTPServer/ftpd.c 

OBJS += \
./Drivers/ioLibrary_Driver-master/Internet/FTPServer/ftpd.o 

C_DEPS += \
./Drivers/ioLibrary_Driver-master/Internet/FTPServer/ftpd.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver-master/Internet/FTPServer/%.o Drivers/ioLibrary_Driver-master/Internet/FTPServer/%.su: ../Drivers/ioLibrary_Driver-master/Internet/FTPServer/%.c Drivers/ioLibrary_Driver-master/Internet/FTPServer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Application/loopback" -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Application/multicast" -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Ethernet" -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Ethernet/W5500" -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Internet/FTPServer" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2d-master-2f-Internet-2f-FTPServer

clean-Drivers-2f-ioLibrary_Driver-2d-master-2f-Internet-2f-FTPServer:
	-$(RM) ./Drivers/ioLibrary_Driver-master/Internet/FTPServer/ftpd.d ./Drivers/ioLibrary_Driver-master/Internet/FTPServer/ftpd.o ./Drivers/ioLibrary_Driver-master/Internet/FTPServer/ftpd.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2d-master-2f-Internet-2f-FTPServer

