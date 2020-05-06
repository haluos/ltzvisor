/*******************************************************************************
|                            LTZVisor SMP library code                         |
********************************************************************************
|@author:   Magdalena Halusek                                                  |
|@date:     04/16/2020                                                         |
|@version:  v1.0 (initial)                                                     |
|@changes:  -04/16/2020: implemented CPU1 NS Guest initial context setting     |
|______________________________________________________________________________|

  ~ This file contains configuration of CPU1 NS Guest context and passing
    control to NS Guest on CPU1 and CPU1 Monitor exception handlers

*******************************************************************************/
#include <asm-offsets.h>

.global _cpu1_monitor_vector_table
_cpu1_monitor_vector_table:
	b	.				@ Not available on MON
	b	.				@ Not available on MON
	mov pc, lr   @ CPU1 reserved only for NS Guest -> don't do context switch
	b .
	b .
	b	.				@ Reserved for HYP
	b .       @ IRQ not routed to LTZVisor
	b .       @ FIQ not routed to LTZVisor

/*
    Input argument is the address of CPU1 NS Guest initial context
*/
.global cpu1_monitor_initial_context_nsguest
.func cpu1_monitor_initial_context_nsguest
cpu1_monitor_initial_context_nsguest:
  /*  Disable routing of FIQ, IRQ and Async Abort Exceptions to Monitor and go
      to NS World */
  mrc p15, 0, r2, c1, c1, 0
  bic r2, r2, #0x0000000E
  orr r2, r2, #0x00000001
  mcr p15, 0, r2, c1, c1, 0
  /*  Set the ACTLR register for NS World */
  ldr r2, [r0, #_ASM_CP_ACTLR_OFFSET]
  mcr p15, 0, r2, c1, c0, 1
  /*  Set the SCTLR register for NS World */
  ldr r2, [r0, #_ASM_CP_SCTLR_OFFSET]
  mcr p15, 0, r2, c1, c0, 0
  /* Go to CPU1 NS World boot sequence */
  @ ldr r2, [r0, #_ASM_MON_LR_OFFSET]
  @ mov pc, r2
	bx lr
.endfunc

.global put_cpu1_back_to_sleep
put_cpu1_back_to_sleep:
	ldr r0, =0xfffffff0
	ldr r1, =0
	str r1, [r0]
wait:
	bl unlock_cpu0
	wfe
	ldr r0, =0xfffffff0
	ldr r1, [r0]
	cmp r1, #0
	beq wait
	mov lr, r1
	mov pc, lr
