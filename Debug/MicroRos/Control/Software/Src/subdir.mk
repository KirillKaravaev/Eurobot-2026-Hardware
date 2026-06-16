################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MicroRos/Control/Software/Src/kinematics.cpp \
../MicroRos/Control/Software/Src/odometry.cpp \
../MicroRos/Control/Software/Src/pid.cpp \
../MicroRos/Control/Software/Src/rate_lim.cpp 

C_SRCS += \
../MicroRos/Control/Software/Src/DSPlib.c 

C_DEPS += \
./MicroRos/Control/Software/Src/DSPlib.d 

OBJS += \
./MicroRos/Control/Software/Src/DSPlib.o \
./MicroRos/Control/Software/Src/kinematics.o \
./MicroRos/Control/Software/Src/odometry.o \
./MicroRos/Control/Software/Src/pid.o \
./MicroRos/Control/Software/Src/rate_lim.o 

CPP_DEPS += \
./MicroRos/Control/Software/Src/kinematics.d \
./MicroRos/Control/Software/Src/odometry.d \
./MicroRos/Control/Software/Src/pid.d \
./MicroRos/Control/Software/Src/rate_lim.d 


# Each subdirectory must supply rules for building sources it contributes
MicroRos/Control/Software/Src/%.o MicroRos/Control/Software/Src/%.su MicroRos/Control/Software/Src/%.cyclo: ../MicroRos/Control/Software/Src/%.c MicroRos/Control/Software/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../micro_ros_stm32cubemx_utils/microros_static_library_ide/libmicroros/include -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Software/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Src/servo_ST3215" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc/servo_ST3215" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
MicroRos/Control/Software/Src/%.o MicroRos/Control/Software/Src/%.su MicroRos/Control/Software/Src/%.cyclo: ../MicroRos/Control/Software/Src/%.cpp MicroRos/Control/Software/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../micro_ros_stm32cubemx_utils/microros_static_library_ide/libmicroros/include -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Software/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Src/servo_ST3215" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc/servo_ST3215" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MicroRos-2f-Control-2f-Software-2f-Src

clean-MicroRos-2f-Control-2f-Software-2f-Src:
	-$(RM) ./MicroRos/Control/Software/Src/DSPlib.cyclo ./MicroRos/Control/Software/Src/DSPlib.d ./MicroRos/Control/Software/Src/DSPlib.o ./MicroRos/Control/Software/Src/DSPlib.su ./MicroRos/Control/Software/Src/kinematics.cyclo ./MicroRos/Control/Software/Src/kinematics.d ./MicroRos/Control/Software/Src/kinematics.o ./MicroRos/Control/Software/Src/kinematics.su ./MicroRos/Control/Software/Src/odometry.cyclo ./MicroRos/Control/Software/Src/odometry.d ./MicroRos/Control/Software/Src/odometry.o ./MicroRos/Control/Software/Src/odometry.su ./MicroRos/Control/Software/Src/pid.cyclo ./MicroRos/Control/Software/Src/pid.d ./MicroRos/Control/Software/Src/pid.o ./MicroRos/Control/Software/Src/pid.su ./MicroRos/Control/Software/Src/rate_lim.cyclo ./MicroRos/Control/Software/Src/rate_lim.d ./MicroRos/Control/Software/Src/rate_lim.o ./MicroRos/Control/Software/Src/rate_lim.su

.PHONY: clean-MicroRos-2f-Control-2f-Software-2f-Src

