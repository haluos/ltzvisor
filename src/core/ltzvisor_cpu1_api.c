/*******************************************************************************
|                            LTZVisor SMP library code                         |
********************************************************************************
|@author:   Magdalena Halusek                                                  |
|@date:     04/16/2020                                                         |
|@version:  v1.0 (initial)                                                     |
|@changes:  -04/16/2020: implemented CPU1 LTZVisor code for configuration of   |
|                        initial NS Guest context of CPU1                      |
|______________________________________________________________________________|

  ~ This file contains implementation of CPU1 LTZVisor API for SMP mode.
  ~ LTZVisor on CPU1 is responsible only for configuring initial NS Guest
    context and for ignoring requests for context switch from CPU1 NS Guest.

*******************************************************************************/

#include "ltzvisor_cpu1_api.h"

void LTZVisor_CPU1_entry (void)
{
	extern void put_cpu1_back_to_sleep(void);
	// printk("LTZVisor running on CPU1\n\t");
	interrupt_interface_init();
	// printk("CPU1 interface initialized!\n\t");
	ltzvisor_cpu1_nsguest_create();
	// printk("CPU1 Ns Guest context OK!\n\t");
	put_cpu1_back_to_sleep();
}

void start_boot (void)
{
	printk("CPU1 boot started\r\n");
}

void ltzvisor_cpu1_nsguest_create (void)
{
  tzmachine NS_CPU1_Guest;
  char *NS_CPU1_name = "Bare-metal CPU1";
  /* Configuring CPU1 NS Guest's descriptors */
  strcpy(NS_CPU1_Guest.name, NS_CPU1_name);
  NS_CPU1_Guest.id = 0x1;
  NS_CPU1_Guest.booting = 1;

  /* Setting the initial values of core registers */
  memset(&NS_CPU1_Guest.core.vcpu_regs_core, 0, sizeof(struct core_regs));
  /* Set the LR to start address of CPU1 NS Guest code */
  NS_CPU1_Guest.core.vcpu_regs_core.lr_mon = 0x00120000;
  /* Set SVC mode with IRQs and Asynchronous Abort Exceptions masked */
  NS_CPU1_Guest.core.vcpu_regs_core.spsr_mon = 0x193;

  /* Setting the initial values of CP15 registers */
  memset(&NS_CPU1_Guest.core.vcpu_regs_cp15, 0, sizeof(struct cp15_regs));
  /* Set SCTLR to:
                  - enable usage of barrier functions (dmb, dsb and isb)
                  - global disable of instruction and data cache
                  - global disable branch prediction
                  - disable MMU */
  NS_CPU1_Guest.core.vcpu_regs_cp15.c1_SCTLR = 0x00c50078;
  /* Set ACTLR to:
                  - enable L1 prefetch
                  - enable L2 prefetch hint
                  - disable cache and TLB broadcast
                  - disable processor to take part in coherency */
  NS_CPU1_Guest.core.vcpu_regs_cp15.c1_ACTLR = 0x00000047;
	cpu1_monitor_initial_context_nsguest(&NS_CPU1_Guest);
}

/**
 * Handling syscalls (SMCs)
 *
 * @param
 *
 * @retval
 */
uint32_t cpu1_board_handler(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
	switch(arg0) {
		case (LTZVISOR_READ_SYSCALL):{
			arg0 = read32((volatile void*)arg1);
			break;
		}
		case (LTZVISOR_WRITE_SYSCALL):{
			write32( (volatile void*)arg1, arg2);
			break;
		}
		case (-32):{
			asm volatile("mrc p15, 0, r0, c15, c0, 0\n"
									 "orr r0, r0, #1\n"
									 "mcr p15, 0, r0, c15, c0, 0\n");
			break;
		}
		default:

			break;
	}

		return arg0;
}

void print_addr (uint32_t arg)
{
	printk("Address that Monitor received: 0x%x\n", arg);
}
