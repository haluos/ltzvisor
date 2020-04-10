ARCH = arm-none-eabi
CC = $(ARCH)-gcc -specs=nosys.specs
OBJCOPY = $(ARCH)-objcopy
CFLAGS = -g -mcpu=cortex-a9
CFLAGS += -Wall -mfpu=vfpv3 -mfloat-abi=hard
CFLAGS += -Wl,-build-id=none

LD = $(ARCH)-ld
LDFLAGS = -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -T src/lscript.ld
LDFLAGS += -Wl,-build-id=none

CASM = $(ARCH)-asm
CASM_FLAGS = -g -mcpu=cortex-a9

NS_DIR = .
SRC_DIR = src

BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.c)
SOURCES += $(wildcard $(SRC_DIR)/*.S)

INCLUDES = -I./inc -I./inc/xil_include
INC_FILES = $(wildcard inc/*.h)
INC_FILES = $(wildcard inc/xil_include/*.h)

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
