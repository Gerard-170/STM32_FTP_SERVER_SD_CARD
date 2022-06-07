################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FATFS/App/app_fatfs.c 

OBJS += \
./FATFS/App/app_fatfs.o 

C_DEPS += \
./FATFS/App/app_fatfs.d 


# Each subdirectory must supply rules for building sources it contributes
FATFS/App/%.o FATFS/App/%.su: ../FATFS/App/%.c FATFS/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Application/loopback" -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Application/multicast" -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Ethernet" -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Ethernet/W5500" -I"F:/RESPALDOS STM/Respaldos Version 2/SHM_V2_SD_FTP/Drivers/ioLibrary_Driver-master/Internet/FTPServer" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FATFS-2f-App

clean-FATFS-2f-App:
	-$(RM) ./FATFS/App/app_fatfs.d ./FATFS/App/app_fatfs.o ./FATFS/App/app_fatfs.su

.PHONY: clean-FATFS-2f-App
