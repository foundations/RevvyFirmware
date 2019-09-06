import json
import pystache

makefile_template = """# This Makefile was generated using "python -m tools.generate_makefile"
C_SRCS += \\
{{ #sources }}
{{ source }}{{ ^last }} \\{{ /last }}
{{ /sources }}

INCLUDE_PATHS += \\
{{ #includes }}
-I{{ path }}{{ ^last }} \\{{ /last }}
{{ /includes }}

COMPILE_FLAGS += \\
-x c \\
-mthumb \\
-D__SAMD51P19A__ \\
-DCOMPATIBLE_HW_VERSIONS=2 \\
-ffunction-sections \\
-mlong-calls \\
-Wall \\
-Wextra \\
-Wundef \\
-mcpu=cortex-m4 \\
-c \\
-std=gnu99 \\
-mfloat-abi=softfp \\
-mfpu=fpv4-sp-d16 \\
-MD \\
-MP

ifeq ($(OS),Windows_NT)
\tSHELL := cmd.exe
\tMKDIR := -md
\tIGNORE := >NUL 2>NUL
\tGCC_BINARY_PREFIX := "C:/Program Files (x86)/Atmel/Studio/7.0/toolchain/arm/arm-gnu-toolchain/bin/
\tGCC_BINARY_SUFFIX := .exe"
else
\tSHELL := /bin/bash
\tMKDIR := mkdir -p
\tIGNORE :=
\tGCC_BINARY_PREFIX :=
\tGCC_BINARY_SUFFIX :=
endif

ifeq ($(config), debug)
OUTPUT_DIR :=Debug
COMPILE_FLAGS += -DDEBUG -O0 -g3
else
OUTPUT_DIR :=Release
COMPILE_FLAGS += -O3
endif

OUTPUT_FILE :=$(OUTPUT_DIR)/rrrc_samd51

all: $(OUTPUT_FILE).elf

OBJS := $(C_SRCS:%.c=$(OUTPUT_DIR)/%.o)
C_DEPS := $(OBJS:%.o=%.d)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

$(OUTPUT_DIR)/%.o: %.c
\t@echo Building file: $<
\t$(MKDIR) "$(@D)" $(IGNORE)
\t$(GCC_BINARY_PREFIX)arm-none-eabi-gcc$(GCC_BINARY_SUFFIX) $(INCLUDE_PATHS) $(COMPILE_FLAGS) -MF $(@:%.o=%.d) -MT$(@:%.o=%.d) -MT$(@:%.o=%.o) -o $@ $<
\t@echo Finished building: $<

$(OUTPUT_FILE).elf: $(OBJS)
\t@echo Building target: $@
\t$(GCC_BINARY_PREFIX)arm-none-eabi-gcc$(GCC_BINARY_SUFFIX) -o$(OUTPUT_FILE).elf $(OBJS) -mthumb -Wl,-Map=$(OUTPUT_FILE).map --specs=nano.specs -Wl,--start-group -lm  -Wl,--end-group $(LINK_DIRS) -Wl,--gc-sections -mcpu=cortex-m4 -TAtmel/Device_Startup/samd51p19a_flash.ld
\t@echo Finished building target: $@
\t$(GCC_BINARY_PREFIX)arm-none-eabi-objcopy$(GCC_BINARY_SUFFIX) -O binary $(OUTPUT_FILE).elf $(OUTPUT_FILE).bin
\t$(GCC_BINARY_PREFIX)arm-none-eabi-objcopy$(GCC_BINARY_SUFFIX) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $(OUTPUT_FILE).elf $(OUTPUT_FILE).hex
\t$(GCC_BINARY_PREFIX)arm-none-eabi-objcopy$(GCC_BINARY_SUFFIX) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary $(OUTPUT_FILE).elf $(OUTPUT_FILE).eep || exit 0
\t$(GCC_BINARY_PREFIX)arm-none-eabi-objdump$(GCC_BINARY_SUFFIX) -h -S $(OUTPUT_FILE).elf > $(OUTPUT_FILE).lss
\t$(GCC_BINARY_PREFIX)arm-none-eabi-objcopy$(GCC_BINARY_SUFFIX) -O srec -R .eeprom -R .fuse -R .lock -R .signature  $(OUTPUT_FILE).elf $(OUTPUT_FILE).srec
\t$(GCC_BINARY_PREFIX)arm-none-eabi-size$(GCC_BINARY_SUFFIX) $(OUTPUT_FILE).elf

clean:
\t-rm -rf Debug
\t-rm -rf Release
"""

if __name__ == "__main__":
    with open("project.json") as f:
        config = json.load(f)

    template_context = {
        'sources':  [{'source': src} for src in config['sources']],
        'includes': [{'path': path} for path in config['includes']]
    }

    template_context['sources'][len(template_context['sources']) - 1]['last'] = True
    template_context['includes'][len(template_context['includes']) - 1]['last'] = True

    makefile_contents = pystache.render(makefile_template, template_context)
    with open("Makefile", "w+") as f:
        f.write(makefile_contents)
