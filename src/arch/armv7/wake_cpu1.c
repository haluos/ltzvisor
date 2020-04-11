#include "io.h"
#include <printk.h>

#define APP_CPU1_ADDR	0x1C010000
#define CPU1_CATCH		0xFFFFFFF0

void add_entry_addr_cpu1()
{
  printk("Wake CPU1\r\n");
  write32((volatile uint32_t *)CPU1_CATCH,APP_CPU1_ADDR);
}
