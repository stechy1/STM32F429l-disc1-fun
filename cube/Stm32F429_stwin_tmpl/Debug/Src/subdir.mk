################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/FramewinDLG.c \
../Src/ThinkGearStreamParser.c \
../Src/Window1DLG.c \
../Src/Window3DLG.c \
../Src/main.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/system_stm32f4xx.c 

OBJS += \
./Src/FramewinDLG.o \
./Src/ThinkGearStreamParser.o \
./Src/Window1DLG.o \
./Src/Window3DLG.o \
./Src/main.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/system_stm32f4xx.o 

C_DEPS += \
./Src/FramewinDLG.d \
./Src/ThinkGearStreamParser.d \
./Src/Window1DLG.d \
./Src/Window3DLG.d \
./Src/main.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STMF429I_DISCO -I"E:/programovani/arm/cube/Stm32F429_stwin_tmpl/Inc" -I"E:/programovani/arm/cube/Stm32F429_stwin_tmpl/Drivers/STM32F4xx_HAL_Driver/Inc" -I"E:/programovani/arm/cube/Stm32F429_stwin_tmpl/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"E:/programovani/arm/cube/Stm32F429_stwin_tmpl/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"E:/programovani/arm/cube/Stm32F429_stwin_tmpl/Drivers/CMSIS/Include" -I"E:/programovani/arm/cube/Stm32F429_stwin_tmpl/Drivers/BSP/STM32F429I-Discovery" -I"E:/programovani/arm/cube/Stm32F429_stwin_tmpl/Utilities/Fonts" -I"c:/Users/Petr/STM32Cube/Repository/STM32Cube_FW_F4_V1.18.0/Middlewares/ST/STemWin/inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


