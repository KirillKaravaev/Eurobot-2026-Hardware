################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MicroRos/Control/Hardware/Src/motor_driver_ZLAC8015D.cpp \
../MicroRos/Control/Hardware/Src/motor_jgb37_bl.cpp \
../MicroRos/Control/Hardware/Src/servo_mg90s.cpp \
../MicroRos/Control/Hardware/Src/sonar.cpp 

C_SRCS += \
../MicroRos/Control/Hardware/Src/optic_odom_PAA5160E1.c 

C_DEPS += \
./MicroRos/Control/Hardware/Src/optic_odom_PAA5160E1.d 

OBJS += \
./MicroRos/Control/Hardware/Src/motor_driver_ZLAC8015D.o \
./MicroRos/Control/Hardware/Src/motor_jgb37_bl.o \
./MicroRos/Control/Hardware/Src/optic_odom_PAA5160E1.o \
./MicroRos/Control/Hardware/Src/servo_mg90s.o \
./MicroRos/Control/Hardware/Src/sonar.o 

CPP_DEPS += \
./MicroRos/Control/Hardware/Src/motor_driver_ZLAC8015D.d \
./MicroRos/Control/Hardware/Src/motor_jgb37_bl.d \
./MicroRos/Control/Hardware/Src/servo_mg90s.d \
./MicroRos/Control/Hardware/Src/sonar.d 


# Each subdirectory must supply rules for building sources it contributes
MicroRos/Control/Hardware/Src/%.o MicroRos/Control/Hardware/Src/%.su MicroRos/Control/Hardware/Src/%.cyclo: ../MicroRos/Control/Hardware/Src/%.cpp MicroRos/Control/Hardware/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../micro_ros_stm32cubemx_utils/microros_static_library_ide/libmicroros/include -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Software/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Src/servo_ST3215" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc/servo_ST3215" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
MicroRos/Control/Hardware/Src/%.o MicroRos/Control/Hardware/Src/%.su MicroRos/Control/Hardware/Src/%.cyclo: ../MicroRos/Control/Hardware/Src/%.c MicroRos/Control/Hardware/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../micro_ros_stm32cubemx_utils/microros_static_library_ide/libmicroros/include -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Software/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Src/servo_ST3215" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc/servo_ST3215" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MicroRos-2f-Control-2f-Hardware-2f-Src

clean-MicroRos-2f-Control-2f-Hardware-2f-Src:
	-$(RM) ./MicroRos/Control/Hardware/Src/motor_driver_ZLAC8015D.cyclo ./MicroRos/Control/Hardware/Src/motor_driver_ZLAC8015D.d ./MicroRos/Control/Hardware/Src/motor_driver_ZLAC8015D.o ./MicroRos/Control/Hardware/Src/motor_driver_ZLAC8015D.su ./MicroRos/Control/Hardware/Src/motor_jgb37_bl.cyclo ./MicroRos/Control/Hardware/Src/motor_jgb37_bl.d ./MicroRos/Control/Hardware/Src/motor_jgb37_bl.o ./MicroRos/Control/Hardware/Src/motor_jgb37_bl.su ./MicroRos/Control/Hardware/Src/optic_odom_PAA5160E1.cyclo ./MicroRos/Control/Hardware/Src/optic_odom_PAA5160E1.d ./MicroRos/Control/Hardware/Src/optic_odom_PAA5160E1.o ./MicroRos/Control/Hardware/Src/optic_odom_PAA5160E1.su ./MicroRos/Control/Hardware/Src/servo_mg90s.cyclo ./MicroRos/Control/Hardware/Src/servo_mg90s.d ./MicroRos/Control/Hardware/Src/servo_mg90s.o ./MicroRos/Control/Hardware/Src/servo_mg90s.su ./MicroRos/Control/Hardware/Src/sonar.cyclo ./MicroRos/Control/Hardware/Src/sonar.d ./MicroRos/Control/Hardware/Src/sonar.o ./MicroRos/Control/Hardware/Src/sonar.su

.PHONY: clean-MicroRos-2f-Control-2f-Hardware-2f-Src

