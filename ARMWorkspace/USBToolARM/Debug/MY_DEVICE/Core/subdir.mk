################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MY_DEVICE/Core/usb_custom.c \
../MY_DEVICE/Core/usb_desc.c 

OBJS += \
./MY_DEVICE/Core/usb_custom.o \
./MY_DEVICE/Core/usb_desc.o 

C_DEPS += \
./MY_DEVICE/Core/usb_custom.d \
./MY_DEVICE/Core/usb_desc.d 


# Each subdirectory must supply rules for building sources it contributes
MY_DEVICE/Core/%.o MY_DEVICE/Core/%.su: ../MY_DEVICE/Core/%.c MY_DEVICE/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../MY_DEVICE/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-MY_DEVICE-2f-Core

clean-MY_DEVICE-2f-Core:
	-$(RM) ./MY_DEVICE/Core/usb_custom.d ./MY_DEVICE/Core/usb_custom.o ./MY_DEVICE/Core/usb_custom.su ./MY_DEVICE/Core/usb_desc.d ./MY_DEVICE/Core/usb_desc.o ./MY_DEVICE/Core/usb_desc.su

.PHONY: clean-MY_DEVICE-2f-Core

