#include "printk.h"
#include "gic.h"
#include "smc_requests.h"
#include "zynq_ttc.h"

extern void invalidate_dcache(void);

void hw_init (void);

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
    // asm volatile("dsb");
    // asm volatile("dmb");
    while(!flag);
    // for (int i = 0; i < 100000000; i++);
    printk("CPU1: Hello World 0x%x\r\n", cnt);
    if(cnt == 2)
    {
      got_to_sleep();
    }
    flag = 0;
    cnt++;
  }
}

int main (void)
{
  uint32_t i, cnt = 0;
  booted = 0;
  hw_init();
  // asm volatile("smc #0");
  // asm volatile("mrs r0, cpsr\n"
  //               "bl print_addr\n");
  printk("\n****** NS World running on CPU0 ******\n");
  while(1)
  {
    if(cnt >= 10)
    {
      got_to_sleep();
    }
    else
    {
      for (i = 0; i < 100000000; i++);
      printk("NS Guest: Hello World %d %d\r\n", cnt, flag);
      flag = 1;
      cnt++;
      // while(flag);
      if(cnt == 5)
      {
        boot_CPU1();
        asm volatile("wfi");
      }
    }
  }
  return 1;
}

void hw_init (void)
{
  /** Initialize TTC0_2 as S Tick */
	ttc_init(TTC1,TTCx_2,INTERVAL);

	/** Config TTC1_2 ISR*/
	interrupt_enable(TTC1_TTCx_2_INTERRUPT,TRUE);
	interrupt_target_set(TTC1_TTCx_2_INTERRUPT,0,1);
	interrupt_priority_set(TTC1_TTCx_2_INTERRUPT,7);
  ttc_interrupt_clear(TTC1_TTCx_2_INTERRUPT);
  ttc_request(TTC1, TTCx_2, 10000);
	/** Start counting */
	ttc_enable(TTC1, TTCx_2);
}
