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

/**
 * Exception return emulation (macro)
 *
 * @param
 *
 * @retval
 */
.macro exception_return_arg
	ldmfd	sp!, {r1}
	msr	spsr, r1
	push	{r1}
	mov	r1, #0
	dsb
	pop	{r1}
	ldmfd	sp!, {r1-r12, pc}^
.endm

/**
 * C function call prolog (macro)
 *
 * @param
 *
 * @retval
 */
.macro call_c_function_arg func_name
	stmfd   sp!, {r1-r12, lr}
	mrs	r4, spsr
	stmfd   sp!, {r4}
	bl	\func_name
.endm

.global _cpu1_monitor_vector_table
_cpu1_monitor_vector_table:
	b	.				@ Not available on MON
	b	.				@ Not available on MON
	ldr	pc, _smc_handler   @ CPU1 reserved only for NS Guest -> don't do context switch
	@ mov pc, lr
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
	ldr r2, [r0, #_ASM_MON_SPSR_OFFSET]
	msr spsr, r2
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
	bl unlock_cpu0
wait:
	wfe
	ldr r0, =0xfffffff0
	ldr r1, [r0]
	cmp r1, #0
	beq wait
	@ mrc p15, 0, r2, c1, c1, 0
  @ orr r2, r2, #0x00000001
  @ mcr p15, 0, r2, c1, c1, 0
	mov r3, #0x113
	mrs r4, cpsr
	mvn r5, #0x1f
	and r4, r4, r5
	orr r4, r4, r3
	msr cpsr, r4
	@ mov r3, #0x8000
	@ movt r3, #0x0200
	@ cmp r1, r3
	@ addlt r1, r1, r3
	mov pc, r1

.globl _smc_handler
	/** Monitor - SMC handler */
_smc_handler:
	cmn	r0, #0
	blt	cpu1_board_smc

.globl cpu1_board_smc
cpu1_board_smc:
	call_c_function_arg cpu1_board_handler
	exception_return_arg
