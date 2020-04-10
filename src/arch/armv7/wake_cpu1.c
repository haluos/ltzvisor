#include "io.h"

#define APP_CPU1_ADDR	0x1C010000
#define CPU1_CATCH		0xFFFFFFF0

void add_entry_addr_cpu1()
{
  write32(CPU1_CATCH,APP_CPU1_ADDR);
}
