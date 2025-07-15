################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Core/core_riscv.c 

C_DEPS += \
./sdk/Core/core_riscv.d 

OBJS += \
./sdk/Core/core_riscv.o 


EXPANDS += \
./sdk/Core/core_riscv.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
sdk/Core/core_riscv.o: d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Core/core_riscv.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/Libraries/doc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Core" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Ld" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Peripheral" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/user/inc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_device" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/code" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

