################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../my_drvs/Src/astronode_application.c \
../my_drvs/Src/astronode_transport.c \
../my_drvs/Src/my_astronode.c \
../my_drvs/Src/my_rtc.c 

OBJS += \
./my_drvs/Src/astronode_application.o \
./my_drvs/Src/astronode_transport.o \
./my_drvs/Src/my_astronode.o \
./my_drvs/Src/my_rtc.o 

C_DEPS += \
./my_drvs/Src/astronode_application.d \
./my_drvs/Src/astronode_transport.d \
./my_drvs/Src/my_astronode.d \
./my_drvs/Src/my_rtc.d 


# Each subdirectory must supply rules for building sources it contributes
my_drvs/Src/%.o my_drvs/Src/%.su my_drvs/Src/%.cyclo: ../my_drvs/Src/%.c my_drvs/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G0B1xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/mzeml/embedded/nemo2.space_tracker_p/my_lib/Inc" -I"C:/Users/mzeml/embedded/nemo2.space_tracker_p/my_drvs/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"

clean: clean-my_drvs-2f-Src

clean-my_drvs-2f-Src:
	-$(RM) ./my_drvs/Src/astronode_application.cyclo ./my_drvs/Src/astronode_application.d ./my_drvs/Src/astronode_application.o ./my_drvs/Src/astronode_application.su ./my_drvs/Src/astronode_transport.cyclo ./my_drvs/Src/astronode_transport.d ./my_drvs/Src/astronode_transport.o ./my_drvs/Src/astronode_transport.su ./my_drvs/Src/my_astronode.cyclo ./my_drvs/Src/my_astronode.d ./my_drvs/Src/my_astronode.o ./my_drvs/Src/my_astronode.su ./my_drvs/Src/my_rtc.cyclo ./my_drvs/Src/my_rtc.d ./my_drvs/Src/my_rtc.o ./my_drvs/Src/my_rtc.su

.PHONY: clean-my_drvs-2f-Src

