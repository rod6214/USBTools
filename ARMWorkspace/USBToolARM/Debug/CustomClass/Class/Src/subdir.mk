################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CustomClass/Class/Src/usbc.c 

OBJS += \
./CustomClass/Class/Src/usbc.o 

C_DEPS += \
./CustomClass/Class/Src/usbc.d 


# Each subdirectory must supply rules for building sources it contributes
CustomClass/Class/Src/%.o CustomClass/Class/Src/%.su: ../CustomClass/Class/Src/%.c CustomClass/Class/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I"C:/Develop/USBTools/ARMWorkspace/USBToolARM/CustomClass/Class/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-CustomClass-2f-Class-2f-Src

clean-CustomClass-2f-Class-2f-Src:
	-$(RM) ./CustomClass/Class/Src/usbc.d ./CustomClass/Class/Src/usbc.o ./CustomClass/Class/Src/usbc.su

.PHONY: clean-CustomClass-2f-Class-2f-Src

