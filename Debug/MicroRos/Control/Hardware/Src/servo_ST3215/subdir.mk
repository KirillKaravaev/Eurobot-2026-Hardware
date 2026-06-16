################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MicroRos/Control/Hardware/Src/servo_ST3215/ArduinoFunctions.cpp \
../MicroRos/Control/Hardware/Src/servo_ST3215/HardwareSerial.cpp \
../MicroRos/Control/Hardware/Src/servo_ST3215/SCS.cpp \
../MicroRos/Control/Hardware/Src/servo_ST3215/SCSCL.cpp \
../MicroRos/Control/Hardware/Src/servo_ST3215/SCSerial.cpp \
../MicroRos/Control/Hardware/Src/servo_ST3215/SMS_STS.cpp 

OBJS += \
./MicroRos/Control/Hardware/Src/servo_ST3215/ArduinoFunctions.o \
./MicroRos/Control/Hardware/Src/servo_ST3215/HardwareSerial.o \
./MicroRos/Control/Hardware/Src/servo_ST3215/SCS.o \
./MicroRos/Control/Hardware/Src/servo_ST3215/SCSCL.o \
./MicroRos/Control/Hardware/Src/servo_ST3215/SCSerial.o \
./MicroRos/Control/Hardware/Src/servo_ST3215/SMS_STS.o 

CPP_DEPS += \
./MicroRos/Control/Hardware/Src/servo_ST3215/ArduinoFunctions.d \
./MicroRos/Control/Hardware/Src/servo_ST3215/HardwareSerial.d \
./MicroRos/Control/Hardware/Src/servo_ST3215/SCS.d \
./MicroRos/Control/Hardware/Src/servo_ST3215/SCSCL.d \
./MicroRos/Control/Hardware/Src/servo_ST3215/SCSerial.d \
./MicroRos/Control/Hardware/Src/servo_ST3215/SMS_STS.d 


# Each subdirectory must supply rules for building sources it contributes
MicroRos/Control/Hardware/Src/servo_ST3215/%.o MicroRos/Control/Hardware/Src/servo_ST3215/%.su MicroRos/Control/Hardware/Src/servo_ST3215/%.cyclo: ../MicroRos/Control/Hardware/Src/servo_ST3215/%.cpp MicroRos/Control/Hardware/Src/servo_ST3215/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../micro_ros_stm32cubemx_utils/microros_static_library_ide/libmicroros/include -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Software/Inc" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Src/servo_ST3215" -I"D:/Projects/Eurobot2026/Program/H723_Eurobot/Eurobot/MicroRos/Control/Hardware/Inc/servo_ST3215" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MicroRos-2f-Control-2f-Hardware-2f-Src-2f-servo_ST3215

clean-MicroRos-2f-Control-2f-Hardware-2f-Src-2f-servo_ST3215:
	-$(RM) ./MicroRos/Control/Hardware/Src/servo_ST3215/ArduinoFunctions.cyclo ./MicroRos/Control/Hardware/Src/servo_ST3215/ArduinoFunctions.d ./MicroRos/Control/Hardware/Src/servo_ST3215/ArduinoFunctions.o ./MicroRos/Control/Hardware/Src/servo_ST3215/ArduinoFunctions.su ./MicroRos/Control/Hardware/Src/servo_ST3215/HardwareSerial.cyclo ./MicroRos/Control/Hardware/Src/servo_ST3215/HardwareSerial.d ./MicroRos/Control/Hardware/Src/servo_ST3215/HardwareSerial.o ./MicroRos/Control/Hardware/Src/servo_ST3215/HardwareSerial.su ./MicroRos/Control/Hardware/Src/servo_ST3215/SCS.cyclo ./MicroRos/Control/Hardware/Src/servo_ST3215/SCS.d ./MicroRos/Control/Hardware/Src/servo_ST3215/SCS.o ./MicroRos/Control/Hardware/Src/servo_ST3215/SCS.su ./MicroRos/Control/Hardware/Src/servo_ST3215/SCSCL.cyclo ./MicroRos/Control/Hardware/Src/servo_ST3215/SCSCL.d ./MicroRos/Control/Hardware/Src/servo_ST3215/SCSCL.o ./MicroRos/Control/Hardware/Src/servo_ST3215/SCSCL.su ./MicroRos/Control/Hardware/Src/servo_ST3215/SCSerial.cyclo ./MicroRos/Control/Hardware/Src/servo_ST3215/SCSerial.d ./MicroRos/Control/Hardware/Src/servo_ST3215/SCSerial.o ./MicroRos/Control/Hardware/Src/servo_ST3215/SCSerial.su ./MicroRos/Control/Hardware/Src/servo_ST3215/SMS_STS.cyclo ./MicroRos/Control/Hardware/Src/servo_ST3215/SMS_STS.d ./MicroRos/Control/Hardware/Src/servo_ST3215/SMS_STS.o ./MicroRos/Control/Hardware/Src/servo_ST3215/SMS_STS.su

.PHONY: clean-MicroRos-2f-Control-2f-Hardware-2f-Src-2f-servo_ST3215

