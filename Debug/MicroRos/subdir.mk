################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MicroRos/MicroRos.cpp \
../MicroRos/ServoTask.cpp 

OBJS += \
./MicroRos/MicroRos.o \
./MicroRos/ServoTask.o 

CPP_DEPS += \
./MicroRos/MicroRos.d \
./MicroRos/ServoTask.d 


# Each subdirectory must supply rules for building sources it contributes
MicroRos/%.o MicroRos/%.su MicroRos/%.cyclo: ../MicroRos/%.cpp MicroRos/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../micro_ros_stm32cubemx_utils/microros_static_library_ide/libmicroros/include -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Software/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Src/servo_ST3215" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc/servo_ST3215" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MicroRos

clean-MicroRos:
	-$(RM) ./MicroRos/MicroRos.cyclo ./MicroRos/MicroRos.d ./MicroRos/MicroRos.o ./MicroRos/MicroRos.su ./MicroRos/ServoTask.cyclo ./MicroRos/ServoTask.d ./MicroRos/ServoTask.o ./MicroRos/ServoTask.su

.PHONY: clean-MicroRos

