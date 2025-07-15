################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup/startup_ch32v30x_D8C.S 

S_UPPER_DEPS += \
./sdk/Startup/startup_ch32v30x_D8C.d 

OBJS += \
./sdk/Startup/startup_ch32v30x_D8C.o 


EXPANDS += \
./sdk/Startup/startup_ch32v30x_D8C.S.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
sdk/Startup/startup_ch32v30x_D8C.o: d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup/startup_ch32v30x_D8C.S
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -x assembler -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_driver" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

