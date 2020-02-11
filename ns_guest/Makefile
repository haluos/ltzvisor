ARCH = arm-none-eabi
CC = $(ARCH)-gcc -specs=nosys.specs
OBJCOPY = $(ARCH)-objcopy
CFLAGS = -g -mcpu=cortex-a9
CFLAGS += -Wall -mfpu=vfpv3 -mfloat-abi=hard 
CFLAGS += -Wl,-build-id=none -specs=Xilinx.spec

LD = $(ARCH)-ld
LDFLAGS = -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -T src/lscript.ld
LDFLAGS += -Wl,-build-id=none -specs=Xilinx.spec

CASM = $(ARCH)-asm
CASM_FLAGS = -g -mcpu=cortex-a9

NS_DIR = ../src/ns_guest/zynq
SRC_DIR = src
API_DIR = lib/lwip/api
NETIF_DIR = lib/lwip/netif
PPP_DIR = lib/lwip/netif/ppp
SSL_DIR = lib/lwip/netif/ppp/polarssl
CORE_DIR = lib/lwip/core
IPV4_DIR = lib/lwip/core/ipv4
IPV6_DIR = lib/lwip/core/ipv6
RTOS_DIR = lib/freertos
XIL_DIR = lib/xil

BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.c)
SOURCES += $(wildcard $(SRC_DIR)/*.S)
SOURCES += $(wildcard $(API_DIR)/*.c)
SOURCES += $(wildcard $(NETIF_DIR)/*.c)
SOURCES += $(wildcard $(PPP_DIR)/*.c)
SOURCES += $(wildcard $(SSL_DIR)/*.c)
SOURCES += $(wildcard $(CORE_DIR)/*.c)
SOURCES += $(wildcard $(IPV4_DIR)/*.c)
SOURCES += $(wildcard $(IPV6_DIR)/*.c)
SOURCES += $(wildcard $(RTOS_DIR)/*.c)
SOURCES += $(wildcard $(RTOS_DIR)/*.S)
SOURCES += $(wildcard $(XIL_DIR)/*.c)
SOURCES += $(wildcard $(XIL_DIR)/*.S)

INCLUDES = -I./inc -I./inc/include -I./inc/FreeRTOS
INC_FILES = $(wildcard inc/*.h)
INC_FILES += $(wildcard inc/include/*.h)
INC_FILES += $(wildcard inc/FreeRTOS/*.h)
INC_FILES += $(wildcard inc/include/arch/*.h)
INC_FILES += $(wildcard inc/include/lwip/*.h)
INC_FILES += $(wildcard inc/include/netif/*.h)
INC_FILES += $(wildcard inc/include/lwip/apps/*.h)
INC_FILES += $(wildcard inc/include/lwip/priv/*.h)
INC_FILES += $(wildcard inc/include/lwip/prot/*.h)
INC_FILES += $(wildcard inc/include/netif/ppp/*.h)
INC_FILES += $(wildcard inc/include/netif/polarssl/*.h)

OBJ = $(addsuffix .o, $(basename $(SOURCES)))
OBJS = $(addprefix build/, $(OBJ))

TARGETS = $(NS_DIR)/ns_guest.elf $(NS_DIR)/ns_guest.bin

all: $(TARGETS) $(INC_FILES)
	@echo "NS Guest ready!"

$(NS_DIR)/ns_guest.elf: $(OBJS) $(INC_FILES)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(NS_DIR)/ns_guest.bin: $(NS_DIR)/ns_guest.elf
	$(OBJCOPY) -O binary $^ $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(NS_DIR)/*.bin rm -f $(NS_DIR)/*.elf
	rm -rf build
	@echo 'Finished cleaning'
