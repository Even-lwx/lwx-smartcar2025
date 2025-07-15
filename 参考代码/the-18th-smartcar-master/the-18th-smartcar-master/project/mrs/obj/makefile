################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include zf_driver/subdir.mk
-include zf_device/subdir.mk
-include zf_common/subdir.mk
-include user_c/subdir.mk
-include sdk/Startup/subdir.mk
-include sdk/Peripheral/subdir.mk
-include sdk/Core/subdir.mk
-include code/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Seekfree_CH32V307VCT6_Opensource_Library.hex \

SECONDARY_LIST += \
Seekfree_CH32V307VCT6_Opensource_Library.lst \

SECONDARY_SIZE += \
Seekfree_CH32V307VCT6_Opensource_Library.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: Seekfree_CH32V307VCT6_Opensource_Library.elf secondary-outputs

# Tool invocations
Seekfree_CH32V307VCT6_Opensource_Library.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -T "d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/sdk/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -L"d:\smartcar\lwx-smartcar2025\参考代码\the-18th-smartcar-master\the-18th-smartcar-master\project\mrs" -L"d:/smartcar/lwx-smartcar2025/参考代码/the-18th-smartcar-master/the-18th-smartcar-master/libraries/zf_device" -Xlinker --print-memory-usage -Wl,-Map,"Seekfree_CH32V307VCT6_Opensource_Library.map" --specs=nano.specs -u _printf_float --specs=nosys.specs -Wl,--print-memory-usage -o "Seekfree_CH32V307VCT6_Opensource_Library.elf" $(OBJS)  $(LIBS)
Seekfree_CH32V307VCT6_Opensource_Library.hex: Seekfree_CH32V307VCT6_Opensource_Library.elf
	@	riscv-none-embed-objcopy -O ihex "Seekfree_CH32V307VCT6_Opensource_Library.elf" "Seekfree_CH32V307VCT6_Opensource_Library.hex"
Seekfree_CH32V307VCT6_Opensource_Library.lst: Seekfree_CH32V307VCT6_Opensource_Library.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "Seekfree_CH32V307VCT6_Opensource_Library.elf" > "Seekfree_CH32V307VCT6_Opensource_Library.lst"
Seekfree_CH32V307VCT6_Opensource_Library.siz: Seekfree_CH32V307VCT6_Opensource_Library.elf
	riscv-none-embed-size --format=berkeley "Seekfree_CH32V307VCT6_Opensource_Library.elf"

# Other Targets
clean:
	-$(RM) $(OBJS) $(EXPANDS) $(CALLGRAPH_DOT) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) Seekfree_CH32V307VCT6_Opensource_Library.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets