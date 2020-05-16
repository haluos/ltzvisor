#include "printk.h"
#include "gic.h"
#include "boot_cpu1.h"

// volatile uint32_t flag;
#define flag  		(*(volatile unsigned long *)(0x001F0000))

void print_addr (uint32_t arg)
{
  printk("address: 0x%x\n", arg);
}

void start_cpu1_ns (void)
{
  printk("CPU1 NS boot start\r\n");
  flag = 1;
}

void helloworld_cpu1 (void)
{
  uint32_t cnt = 0;
  printk("CPU1 awake\r\n");
  // asm volatile(".arch_extension sec\n");
  asm volatile("smc #0\n");
  // interrupt_IPI_generate(0, 1);
  while(1)
  {
    while(!flag);
    // for (int i = 0; i < 100000000; i++);
    // asm volatile("dsb");
    printk("CPU1: Hello World 0x%x\r\n", cnt);
    flag = 0;
    cnt++;
  }
}

int main (void)
{
  uint32_t i, cnt = 0;
  while(1)
  {
    for (i = 0; i < 100000000; i++);
    printk("NS Guest: Hello World %d %d\r\n", cnt, flag);
    flag = 1;
    cnt++;
    // while(flag);
    if(cnt == 15)
    {
      boot_CPU1();
    }
  }
  return 1;
}
