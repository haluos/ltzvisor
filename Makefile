HYPERVISOR_PRODUCT = LTZVisor
HYPERVISOR_MAJOR_VERSION = 0
HYPERVISOR_MINOR_VERSION = 2
HYPERVISOR_REVISION_VERSION = 1

BINFILE = src/ns_guest/zynq/ns_guest.elf

SYSTEM_MEMORY = 0x800000

ARCH = arm

QEMU_MEM = $(shell echo $$(( ($(SYSTEM_MEMORY)-1)/1048576+1 )) )
QEMU = qemu-system-$(ARCH)
QFLAGS = -M xilinx-zynq-a9 -m $(QEMU_MEM)M -nographic
QMSG = "Starting qemu ... (press Ctrl+a then x to stop)"

DEBUG_GDB = arm-none-eabi-gdb

export BOARD:= ZEDBOARD
export S_GUEST:= BARE_METAL
export NS_GUEST:= BARE_METAL
export CROSS_COMPILE:= arm-none-eabi-

export SDK_PATH = $(CURDIR)

#export CROSS_COMPILE = /opt/CodeSourcery/arm-xilinx-gnueabi/bin/arm-xilinx-eabi-
# export CROSS_COMPILE = /opt/Linaro/gcc-linaro-7.1.1-2017.08-x86_64_arm-eabi/bin/arm-eabi-

all:
	echo 'LTZVisor builds start'
	$(MAKE) -C $(SDK_PATH)/src

debug_qemu: all
	@echo $(QMSG)
	@$(QEMU) $(QFLAGS) -kernel $(BINFILE) -s -S
debug_gdb: all
	@echo Starting gdb ...
	@$(DEBUG_GDB) -s $(BINFILE) -ex 'target remote localhost:1234'

clean:
	$(MAKE) -C $(SDK_PATH)/src clean

	rm -f bin/*.bin bin/*.elf
	# rm -f src/ns_guest/zynq/ns_app.bin src/ns_guest/zynq/ns_app.elf
	rm -f $(VERSION_FILE)
