################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery.c \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_io.c \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_lcd.c \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_sdram.c \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_ts.c 

OBJS += \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery.o \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_io.o \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_lcd.o \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_sdram.o \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_ts.o 

C_DEPS += \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery.d \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_io.d \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_lcd.d \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_sdram.d \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32F429I-Discovery/%.o: ../Drivers/BSP/STM32F429I-Discovery/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STMF429I_DISCO -I"D:/user/Pavel/stm32/workspace/AC6/Stm32F429_stwin_tmpl/Inc" -I"D:/user/Pavel/stm32/workspace/AC6/Stm32F429_stwin_tmpl/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/user/Pavel/stm32/workspace/AC6/Stm32F429_stwin_tmpl/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/user/Pavel/stm32/workspace/AC6/Stm32F429_stwin_tmpl/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/user/Pavel/stm32/workspace/AC6/Stm32F429_stwin_tmpl/Drivers/CMSIS/Include" -I"D:/STM32Cube_FW_F4/repository/STM32Cube_FW_F4_V1.16.0/Middlewares/ST/STemWin/inc" -I"D:/user/Pavel/stm32/workspace/AC6/Stm32F429_stwin_tmpl/Drivers/BSP/STM32F429I-Discovery" -I"D:/user/Pavel/stm32/workspace/AC6/Stm32F429_stwin_tmpl/Utilities/Fonts"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


