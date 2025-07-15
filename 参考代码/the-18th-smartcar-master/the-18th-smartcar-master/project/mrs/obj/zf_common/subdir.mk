################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_clock.c \
d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_debug.c \
d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_fifo.c \
d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_font.c \
d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_function.c \
d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_interrupt.c 

C_DEPS += \
./zf_common/zf_common_clock.d \
./zf_common/zf_common_debug.d \
./zf_common/zf_common_fifo.d \
./zf_common/zf_common_font.d \
./zf_common/zf_common_function.d \
./zf_common/zf_common_interrupt.d 

OBJS += \
./zf_common/zf_common_clock.o \
./zf_common/zf_common_debug.o \
./zf_common/zf_common_fifo.o \
./zf_common/zf_common_font.o \
./zf_common/zf_common_function.o \
./zf_common/zf_common_interrupt.o 


EXPANDS += \
./zf_common/zf_common_clock.c.234r.expand \
./zf_common/zf_common_debug.c.234r.expand \
./zf_common/zf_common_fifo.c.234r.expand \
./zf_common/zf_common_font.c.234r.expand \
./zf_common/zf_common_function.c.234r.expand \
./zf_common/zf_common_interrupt.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
zf_common/zf_common_clock.o: d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_clock.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/Libraries/doc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Core" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Ld" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Peripheral" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/user/inc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_device" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/code" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
zf_common/zf_common_debug.o: d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_debug.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/Libraries/doc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Core" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Ld" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Peripheral" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/user/inc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_device" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/code" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
zf_common/zf_common_fifo.o: d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_fifo.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/Libraries/doc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Core" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Ld" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Peripheral" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/user/inc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_device" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/code" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
zf_common/zf_common_font.o: d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_font.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/Libraries/doc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Core" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Ld" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Peripheral" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/user/inc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_device" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/code" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
zf_common/zf_common_function.o: d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_function.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/Libraries/doc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Core" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Ld" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Peripheral" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/user/inc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_device" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/code" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
zf_common/zf_common_interrupt.o: d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common/zf_common_interrupt.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/Libraries/doc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Core" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Ld" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Peripheral" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Startup" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/user/inc" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_common" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_device" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/project/code" -I"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

