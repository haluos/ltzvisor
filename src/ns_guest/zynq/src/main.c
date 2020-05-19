#include "printk.h"
#include "gic.h"
#include "boot_cpu1.h"

extern void invalidate_dcache(void);

// volatile uint32_t flag;
#define flag  		(*(volatile unsigned long *)(0x001F0000))
uint8_t booted;

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
  booted = 1;
  uint32_t cnt = 0;
  printk("CPU1 awake\r\n");
  while(1)
  {
    asm volatile("dsb");
    asm volatile("dmb");
    while(!flag);
    // for (int i = 0; i < 100000000; i++);
    printk("CPU1: Hello World 0x%x\r\n", cnt);
    flag = 0;
    cnt++;
  }
}

int main (void)
{
  uint32_t i, cnt = 0;
  booted = 0;
  // asm volatile("smc #0");
  while(1)
  {
    for (i = 0; i < 100000000; i++);
    printk("NS Guest: Hello World %d %d\r\n", cnt, flag);
    flag = 1;
    cnt++;
    // while(flag);
    if(cnt == 15)
    {
      // asm volatile( "ldr r0, =0\n"
      //               "mcr 	p15, 0, r0, c7, c5, 0\n");
      // asm volatile("dsb");
      // asm volatile("isb");
      boot_CPU1();
      // asm volatile("dmb");
      // asm volatile("isb");
    }
  }
  return 1;
}
