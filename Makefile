C_SRCS += \
driver_init.c \
main.c \
rrrc/controller/pid.c \
rrrc/rrrc_worklogic.c \
rrrc/runtime/comm_handlers.c \
rrrc/runtime/connections_error_storage.c \
rrrc/runtime/connections_imu.c \
rrrc/runtime/connections_leds.c \
rrrc/utils/color.c \
rrrc/utils/converter.c \
rrrc/utils/crc.c \
rrrc/utils/functions.c

# Software Component Source Files
C_SRCS += \
rrrc/components/ADC/adc.c \
rrrc/components/BatteryCalculator/BatteryCalculator.c \
rrrc/components/BatteryCharger/BatteryCharger.c \
rrrc/components/BatteryIndicator/BatteryIndicator.c \
rrrc/components/BatteryStatusProvider/BatteryStatusProvider.c \
rrrc/components/BluetoothIndicator/BluetoothIndicator.c \
rrrc/components/BluetoothStatusObserver/BluetoothStatusObserver.c \
rrrc/components/BrainStatusIndicator/BrainStatusIndicator.c \
rrrc/components/CommunicationObserver/CommunicationObserver.c \
rrrc/components/Drivetrain/Drivetrain.c \
rrrc/components/ErrorStorageWrapper/ErrorStorageWrapper.c \
rrrc/components/ErrorStorage/ErrorStorage.c \
rrrc/components/GyroscopeOffsetCompensator/GyroscopeOffsetCompensator.c \
rrrc/components/IMU/IMU.c \
rrrc/components/IMU/imu_ll.c \
rrrc/components/HighResolutionTimer/HighResolutionTimer.c \
rrrc/components/McuStatusCollectorWrapper/McuStatusCollectorWrapper.c \
rrrc/components/McuStatusCollector/McuStatusCollector.c \
rrrc/components/MemoryAllocator/MemoryAllocator.c \
rrrc/components/MotorDerating/MotorDerating.c \
rrrc/components/MotorDriver_8833/MotorDriver_8833.c \
rrrc/components/MotorPortHandler/MotorPortHandler.c \
rrrc/components/MotorPortHandler/MotorPortHandlerInternal.c \
rrrc/components/MotorPortHandler/MotorPortLibraries/DcMotor/DcMotor.c \
rrrc/components/MotorPortHandler/MotorPortLibraries/Dummy/Dummy.c \
rrrc/components/MotorThermalModel/MotorThermalModel.c \
rrrc/components/RestartManager/RestartManager.c \
rrrc/components/SensorPortHandler/SensorPortHandlerInternal.c \
rrrc/components/SensorPortHandler/SensorPortHandler.c \
rrrc/components/SensorPortHandler/SensorPortLibraries/BumperSwitch/BumperSwitch.c \
rrrc/components/SensorPortHandler/SensorPortLibraries/DebugRTC/DebugRTC.c \
rrrc/components/SensorPortHandler/SensorPortLibraries/Dummy/Dummy.c \
rrrc/components/SensorPortHandler/SensorPortLibraries/HC_SR04/HC_SR04.c \
rrrc/components/SensorPortHandler/SensorPortLibraries/SensorPort_I2C.c \
rrrc/components/VersionProvider/VersionProvider.c \
rrrc/components/InternalTemperatureSensor/InternalTemperatureSensor.c \
rrrc/components/LEDController/LEDController.c \
rrrc/components/MasterCommunicationInterface/i2cHal.c \
rrrc/components/MasterCommunicationInterface/MasterCommunicationInterface.c \
rrrc/components/MasterCommunication/CommunicationManager.c \
rrrc/components/MasterCommunication/MasterCommunication.c \
rrrc/components/MasterStatusObserver/MasterStatusObserver.c \
rrrc/components/RingLedDisplay/RingLedDisplay.c \
rrrc/components/YawAngleTracker/YawAngleTracker.c
# End of Software Component Source Files

INCLUDE_PATHS += \
-I. \
-Irrrc

C_SRCS += \
Atmel/Device_Startup/startup_samd51.c \
Atmel/Device_Startup/system_samd51.c \
Atmel/hal/src/hal_adc_async.c \
Atmel/hal/src/hal_atomic.c \
Atmel/hal/src/hal_cache.c \
Atmel/hal/src/hal_delay.c \
Atmel/hal/src/hal_evsys.c \
Atmel/hal/src/hal_ext_irq.c \
Atmel/hal/src/hal_flash.c \
Atmel/hal/src/hal_gpio.c \
Atmel/hal/src/hal_i2c_m_async.c \
Atmel/hal/src/hal_i2c_s_async.c \
Atmel/hal/src/hal_init.c \
Atmel/hal/src/hal_io.c \
Atmel/hal/src/hal_sleep.c \
Atmel/hal/src/hal_spi_m_dma.c \
Atmel/hal/src/hal_spi_m_sync.c \
Atmel/hal/utils/src/utils_assert.c \
Atmel/hal/utils/src/utils_syscalls.c \
Atmel/hpl/adc/hpl_adc.c \
Atmel/hpl/cmcc/hpl_cmcc.c \
Atmel/hpl/core/hpl_core_m4.c \
Atmel/hpl/core/hpl_init.c \
Atmel/hpl/dmac/hpl_dmac.c \
Atmel/hpl/eic/hpl_eic.c \
Atmel/hpl/evsys/hpl_evsys.c \
Atmel/hpl/gclk/hpl_gclk.c \
Atmel/hpl/mclk/hpl_mclk.c \
Atmel/hpl/nvmctrl/hpl_nvmctrl.c \
Atmel/hpl/osc32kctrl/hpl_osc32kctrl.c \
Atmel/hpl/oscctrl/hpl_oscctrl.c \
Atmel/hpl/pm/hpl_pm.c \
Atmel/hpl/sercom/hpl_sercom.c \
Atmel/hpl/systick/hpl_systick.c \
Atmel/hpl/tc/hpl_tc.c \
Atmel/hpl/tc/tc_lite.c \
third_party/FreeRTOSV10.0.0/rtos_port.c \
third_party/FreeRTOSV10.0.0/Source/croutine.c \
third_party/FreeRTOSV10.0.0/Source/event_groups.c \
third_party/FreeRTOSV10.0.0/Source/list.c \
third_party/FreeRTOSV10.0.0/Source/portable/GCC/ARM_CM4F/port.c \
third_party/FreeRTOSV10.0.0/Source/portable/MemMang/heap_4.c \
third_party/FreeRTOSV10.0.0/Source/queue.c \
third_party/FreeRTOSV10.0.0/Source/stream_buffer.c \
third_party/FreeRTOSV10.0.0/Source/tasks.c \
third_party/FreeRTOSV10.0.0/Source/timers.c

INCLUDE_PATHS += \
-IConfig \
-IAtmel/hal/include \
-IAtmel/hal/utils/include \
-IAtmel/hpl/adc \
-IAtmel/hpl/cmcc \
-IAtmel/hpl/core \
-IAtmel/hpl/dmac \
-IAtmel/hpl/eic \
-IAtmel/hpl/gclk \
-IAtmel/hpl/mclk \
-IAtmel/hpl/osc32kctrl \
-IAtmel/hpl/oscctrl \
-IAtmel/hpl/pm \
-IAtmel/hpl/port \
-IAtmel/hpl/rtc \
-IAtmel/hpl/sercom \
-IAtmel/hpl/systick \
-IAtmel/hpl/tc \
-IAtmel/hri \
-IAtmel/samd51a/include \
-Ithird_party \
-Ithird_party/CMSIS/Include \
-Ithird_party/FreeRTOSV10.0.0 \
-Ithird_party/FreeRTOSV10.0.0/Source/include \
-Ithird_party/FreeRTOSV10.0.0/Source/portable/GCC/ARM_CM4F

COMPILE_FLAGS += \
-x c \
-mthumb \
-D__SAMD51P19A__ \
-DCOMPATIBLE_HW_VERSIONS=2 \
-ffunction-sections \
-mlong-calls \
-Wall \
-Wextra \
-Wundef \
-mcpu=cortex-m4 \
-c \
-std=gnu99 \
-mfloat-abi=softfp \
-mfpu=fpv4-sp-d16 \
-MD \
-MP

ifeq ($(OS),Windows_NT)
	SHELL := cmd.exe
	MKDIR := -md
	IGNORE := >NUL 2>NUL
	GCC_BINARY_PREFIX := "C:/Program Files (x86)/Atmel/Studio/7.0/toolchain/arm/arm-gnu-toolchain/bin/
	GCC_BINARY_SUFFIX := .exe"
else
	SHELL := /bin/bash
	MKDIR := mkdir -p
	IGNORE :=
	GCC_BINARY_PREFIX :=
	GCC_BINARY_SUFFIX :=
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
	@echo Building file: $<
	$(MKDIR) "$(@D)" $(IGNORE)
	$(GCC_BINARY_PREFIX)arm-none-eabi-gcc$(GCC_BINARY_SUFFIX) $(INCLUDE_PATHS) $(COMPILE_FLAGS) -MF $(@:%.o=%.d) -MT$(@:%.o=%.d) -MT$(@:%.o=%.o) -o $@ $< 
	@echo Finished building: $<

$(OUTPUT_FILE).elf: $(OBJS)
	@echo Building target: $@
	$(GCC_BINARY_PREFIX)arm-none-eabi-gcc$(GCC_BINARY_SUFFIX) -o$(OUTPUT_FILE).elf $(OBJS) -mthumb -Wl,-Map=$(OUTPUT_FILE).map --specs=nano.specs -Wl,--start-group -lm  -Wl,--end-group $(LINK_DIRS) -Wl,--gc-sections -mcpu=cortex-m4 -TAtmel/Device_Startup/samd51p19a_flash.ld  
	@echo Finished building target: $@
	$(GCC_BINARY_PREFIX)arm-none-eabi-objcopy$(GCC_BINARY_SUFFIX) -O binary $(OUTPUT_FILE).elf $(OUTPUT_FILE).bin
	$(GCC_BINARY_PREFIX)arm-none-eabi-objcopy$(GCC_BINARY_SUFFIX) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $(OUTPUT_FILE).elf $(OUTPUT_FILE).hex
	$(GCC_BINARY_PREFIX)arm-none-eabi-objcopy$(GCC_BINARY_SUFFIX) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary $(OUTPUT_FILE).elf $(OUTPUT_FILE).eep || exit 0
	$(GCC_BINARY_PREFIX)arm-none-eabi-objdump$(GCC_BINARY_SUFFIX) -h -S $(OUTPUT_FILE).elf > $(OUTPUT_FILE).lss
	$(GCC_BINARY_PREFIX)arm-none-eabi-objcopy$(GCC_BINARY_SUFFIX) -O srec -R .eeprom -R .fuse -R .lock -R .signature  $(OUTPUT_FILE).elf $(OUTPUT_FILE).srec
	$(GCC_BINARY_PREFIX)arm-none-eabi-size$(GCC_BINARY_SUFFIX) $(OUTPUT_FILE).elf

clean:
	-rm -rf Debug
	-rm -rf Release
