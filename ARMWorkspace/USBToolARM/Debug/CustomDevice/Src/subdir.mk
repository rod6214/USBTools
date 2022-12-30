################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CustomDevice/Src/cusb_device.c 

OBJS += \
./CustomDevice/Src/cusb_device.o 

C_DEPS += \
./CustomDevice/Src/cusb_device.d 


# Each subdirectory must supply rules for building sources it contributes
CustomDevice/Src/%.o CustomDevice/Src/%.su: ../CustomDevice/Src/%.c CustomDevice/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I"C:/Develop/USBTools/ARMWorkspace/USBToolARM/CustomClass/Class/Inc" -I../CustomDevice/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-CustomDevice-2f-Src

clean-CustomDevice-2f-Src:
	-$(RM) ./CustomDevice/Src/cusb_device.d ./CustomDevice/Src/cusb_device.o ./CustomDevice/Src/cusb_device.su

.PHONY: clean-CustomDevice-2f-Src

