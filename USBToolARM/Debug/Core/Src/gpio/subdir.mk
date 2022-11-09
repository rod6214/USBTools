################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/gpio/gpio.c 

OBJS += \
./Core/Src/gpio/gpio.o 

C_DEPS += \
./Core/Src/gpio/gpio.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/gpio/%.o Core/Src/gpio/%.su: ../Core/Src/gpio/%.c Core/Src/gpio/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../CustomClass/Class/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-gpio

clean-Core-2f-Src-2f-gpio:
	-$(RM) ./Core/Src/gpio/gpio.d ./Core/Src/gpio/gpio.o ./Core/Src/gpio/gpio.su

.PHONY: clean-Core-2f-Src-2f-gpio

