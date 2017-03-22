################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Program\ Files\ (x86)/Arduino/libraries/CAN_BUS_Shield-master/mcp_can.cpp 

LINK_OBJ += \
./libraries/CAN_BUS_Shield-master/mcp_can.cpp.o 

CPP_DEPS += \
.\libraries\CAN_BUS_Shield-master\mcp_can.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/CAN_BUS_Shield-master/mcp_can.cpp.o: C:/Program\ Files\ (x86)/Arduino/libraries/CAN_BUS_Shield-master/mcp_can.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Users\DavidAlain\eclipse\cpp-neon2\eclipse\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.3-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10609 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR   -I"C:\Users\DavidAlain\eclipse\cpp-neon2\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.17\cores\arduino" -I"C:\Users\DavidAlain\eclipse\cpp-neon2\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.17\variants\standard" -I"C:\Users\DavidAlain\eclipse\cpp-neon2\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.17\libraries\SPI" -I"C:\Users\DavidAlain\eclipse\cpp-neon2\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.17\libraries\SPI\src" -I"C:\Program Files (x86)\Arduino\libraries\CAN_BUS_Shield-master" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


