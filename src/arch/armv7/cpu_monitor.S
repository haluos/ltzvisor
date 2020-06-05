/*
 * LTZVisor, a Lightweight TrustZone-assisted Hypervisor
 *
 * Copyright (c) TZVisor Project (www.tzvisor.org), 2017-
 *
 * Authors:
 *  Sandro Pinto <sandro@tzvisor.org>
 *  Jorge Pereira <jorgepereira89@gmail.com>
 *
 * This file is part of LTZVisor.
 *
 * LTZVisor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, with a special
 * exception described below.
 *
 * Linking this code statically or dynamically with other modules
 * is making a combined work based on this code. Thus, the terms
 * and conditions of the GNU General Public License V2 cover the
 * whole combination.
 *
 * As a special exception, the copyright holders of LTZVisor give
 * you permission to link LTZVisor with independent modules to
 * produce a statically linked executable, regardless of the license
 * terms of these independent modules, and to copy and distribute
 * the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and
 * conditions of the license of that module. An independent module
 * is a module which is not derived from or based on LTZVisor.
 *
 * LTZVisor is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 * [cpu_monitor.S]
 *
 * This file contains the Monitor implementation.
 *
 * (#) $id: cpu_monitor.S 03-05-2015 s_pinto & j_pereira $
 * (#) $id: cpu_monitor.S 18-09-2017 s_pinto (modified) $
*/

/*******************************************************************************
|                 ZedBoard's LTZVisor SMP library code                         |
********************************************************************************
|@author:   Magdalena Halusek                                                  |
|@date:     04/09/2020                                                         |
|@version:  v1.0 (initial)                                                     |
|@changes:  -04/09/2020: implemented fuction that triggers waking up of CPU1   |
|           -04/17/2020: changed fixed CPU1 LTZVisor start address to address  |
|                        provided by linker.ld                                 |
|______________________________________________________________________________|
|    Besides of the Monitor implementation for one core (CPU0), this file      |
|    contains small amount of code used to trigger waking up the CPU1.         |
|    This part of code is called when NS Guest issues SMC instruction with     |
|    certain parameter defined in ltzvisor_syscall_asm.h. It is only used      |
|    when LTZVisor is to be used in SMP mode, where S Guest uses only CPU0     |
|    and NS Guest uses both CPU0 and CPU1.                                     |
*******************************************************************************/

 #include <cpu_defines.h>
 #include <asm-offsets.h>
 #include <ltzvisor_syscall_asm.h>

/* Variable defined in linker.ld which contains start address of CPU1 LTZVisor
   code */
.extern  _cpu1_entry_address

/**
 * SCR Non-Secure bit set (macro)
 *
 * @param
 *
 * @retval
 */
 .globl SCR_NS_SET
.macro SCR_NS_SET
	push 	{r0}
	mrc     p15, 0, r0, c1, c1, 0	@ Read SCR
	orr	r0, r0, #SCR_NS_BIT	@ Enable NS bit
	orr	r0, r0, #SCR_FIQ_BIT	@ Enable the route of FIQ in Monitor
  bic r0, r0, #SCR_IRQ_BIT
	mcr     p15, 0, r0, c1, c1, 0	@ Write SCR
	pop 	{r0}
.endm

.macro SCR_FIQ_CLEAR
  push {r0}
  mrc     p15, 0, r0, c1, c1, 0	@ Read SCR
  bic	r0, r0, #SCR_FIQ_BIT
  mcr     p15, 0, r0, c1, c1, 0	@ Write SCR
  pop 	{r0}
.endm

.macro SCR_FIQ_SET
  push {r0}
  mrc     p15, 0, r0, c1, c1, 0	@ Read SCR
  orr	r0, r0, #SCR_FIQ_BIT
  mcr     p15, 0, r0, c1, c1, 0	@ Write SCR
  pop 	{r0}
.endm

/**
 * SCR Non-Secure bit clear (macro)
 *
 * @param
 *
 * @retval
 */
.macro SCR_NS_CLEAR
	push 	{r0}
	mrc     p15, 0, r0, c1, c1, 0	@ Read SCR
	bic	r0, r0, #SCR_NS_BIT	@ Disable the NS bit
	bic	r0, r0, #SCR_FIQ_BIT	@ Disable the route of FIQs in Monitor
  bic r0, r0, #SCR_IRQ_BIT
	mcr     p15, 0, r0, c1, c1, 0	@ Write SCR
	pop 	{r0}
.endm

.macro SCR_NS_CLEAR_WFI
	push 	{r0}
	mrc     p15, 0, r0, c1, c1, 0	@ Read SCR
	bic	r0, r0, #SCR_NS_BIT	@ Disable the NS bit
	bic	r0, r0, #SCR_FIQ_BIT	@ Disable the route of FIQs in Monitor
  orr r0, r0, #SCR_IRQ_BIT
	mcr     p15, 0, r0, c1, c1, 0	@ Write SCR
	pop 	{r0}
.endm

/**
 * Get NS guest context's pointer (macro)
 *
 * @param
 *
 * @retval	NS_guest context pointer
 */
@  .globl GET_NS_WORLD_CONTEXT
@ .macro GET_NS_WORLD_CONTEXT
@ 	mrc p15, 0, r0, c13, c0, 4		@ Read PL1 only thread ID reg
@ 	add r0, r0, #_ASM_ARCH_REGS_OFFSET	@ Add offset
@ .endm

.macro GET_NS_WORLD_CONTEXT reg
	ldr \reg, =NS_Guest
	add \reg, \reg, #_ASM_ARCH_REGS_OFFSET	@ Add offset
.endm

/**
 * World context save (macro)
 *
 * @param	r0 = world context pointer
 *
 * @retval
 */
@ .macro WORLD_CONTEXT_SAVE
@ 	mov	r4, r0
@ 	pop	{r0 - r3}
@ 	stmia	r4, {r0 - r3}
@ 	mov	r3, r4
@ 	add	r4, r4, #_ASM_R4_OFFSET
@ 	mov	r0, r4
@ 	pop	{r4, lr}
@ 	stmia	r0!, {r4-r12}
@ 	mrs	r1, spsr
@ 	stmia	r0!, {r1, lr}
@ 	/* Save SVC */
@ 	cps #SUPERVISOR_MODE
@ 	mrs	 r1, spsr
@ 	stmia   r0!, {r1, r13, lr}
@ 	/* Save SYS */
@ 	cps #SYSTEM_MODE
@ 	mrs	 r1, spsr
@ 	stmia   r0!, {r1,r13, lr}
@ 	/* Save ABORT */
@ 	cps #ABORT_MODE
@ 	mrs	 r1, spsr
@ 	stmia   r0!, {r1, r13, lr}
@ 	/* Save UNDEF */
@ 	cps #UNDEFINED_MODE
@ 	mrs	 r1, spsr
@ 	stmia   r0!, {r1, r13, lr}
@ 	/* Save IRQ */
@ 	cps #IRQ_MODE
@ 	mrs	 r1, spsr
@ 	stmia   r0!, {r1, r13, lr}
@ 	cps #MONITOR_MODE
@ .endm

.macro NSWORLD_CONTEXT_SAVE
	push {lr}
	GET_NS_WORLD_CONTEXT lr
	stmia lr!, {r0-r12}
	mov r0, lr
	pop {lr}
	mrs	r1, spsr
	stmia	r0!, {r1, lr}
	/* Save SVC */
	cps #SUPERVISOR_MODE
	mrs	 r1, spsr
	stmia   r0!, {r1, r13, lr}
	/* Save SYS */
	cps #SYSTEM_MODE
	mrs	 r1, spsr
	stmia   r0!, {r1,r13, lr}
	/* Save ABORT */
	cps #ABORT_MODE
	mrs	 r1, spsr
	stmia   r0!, {r1, r13, lr}
	/* Save UNDEF */
	cps #UNDEFINED_MODE
	mrs	 r1, spsr
	stmia   r0!, {r1, r13, lr}
	/* Save IRQ */
	cps #IRQ_MODE
	mrs	 r1, spsr
	stmia   r0!, {r1, r13, lr}

  @ fmrx r1, fpscr
  @ vstmia r0!, {d0-d15}
  @ vstmia r0!, {d16-d31}
  @ @ mov r2, r0
  @ stmia r0!, {r1}

	cps #MONITOR_MODE
.endm

/**
 * World context restore (macro)
 *
 * @param	r0 = world context pointer
 *
 * @retval
 */
@ .macro WORLD_CONTEXT_RESTORE
@ 	mov	r4, r0
@ 	ldmia 	r4!, {r0 - r3}
@ 	push 	{r0, r1}
@ 	mov 	r0, r4
@ 	ldmia	r0!, {r4 - r12}
@ 	ldmia	r0!, {r1, lr}
@ 	msr	spsr_cxsf, r1
@ 	/* Restore SVC */
@ 	cps 	#SUPERVISOR_MODE
@ 	ldmia   r0!, {r1, r13, lr}
@ 	msr 	spsr_cxsf, r1
@ 	/* Restore SYS */
@ 	cps 	#SYSTEM_MODE
@ 	ldmia   r0!, {r1,r13, lr}
@ 	msr 	spsr_cxsf, r1
@ 	/* Restore ABORT */
@ 	cps 	#ABORT_MODE
@ 	ldmia	r0!, {r1, r13, lr}
@ 	msr	spsr_cxsf, r1
@ 	/* Restore UNDEF */
@ 	cps 	#UNDEFINED_MODE
@ 	ldmia	r0!, {r1, r13, lr}
@ 	msr	spsr_cxsf, r1
@ 	/* Restore IRQ */
@ 	cps 	#IRQ_MODE
@ 	ldmia	r0!, {r1, r13, lr}
@ 	msr	spsr_cxsf, r1
@ 	cps 	#MONITOR_MODE
@ 	pop	{r0, r1}
@ .endm

.macro NSWORLD_CONTEXT_RESTORE
  @ push {lr}
	GET_NS_WORLD_CONTEXT lr
	ldmia 	lr!, {r0-r12}
	push    {r0,r1}
	mov		r0, lr
	ldmia	r0!, {r1, lr}
	msr	spsr_cxsf, r1
	/* Restore SVC */
	cps 	#SUPERVISOR_MODE
	ldmia   r0!, {r1, sp, lr}
	msr 	spsr_cxsf, r1
	/* Restore SYS */
	cps 	#SYSTEM_MODE
	ldmia   r0!, {r1, sp, lr}
	msr 	spsr_cxsf, r1
	/* Restore ABORT */
	cps 	#ABORT_MODE
	ldmia	r0!, {r1, sp, lr}
	msr	spsr_cxsf, r1
	/* Restore UNDEF */
	cps 	#UNDEFINED_MODE
	ldmia	r0!, {r1, sp, lr}
	msr	spsr_cxsf, r1
	/* Restore IRQ */
	cps 	#IRQ_MODE
	ldmia	r0!, {r1, sp, lr}
	msr	spsr_cxsf, r1

  @ vldmia r0!, {d0-d15}
  @ vldmia r0!, {d16-d31}
  @ @ mov r2, r0
  @ ldmia r0!, {r1}
  @ @ add r0, r0, #4
  @ vmsr fpscr, r1

	cps 	#MONITOR_MODE
	pop	{r0, r1}
  @ pop {lr}
.endm

/**
 * Exception return emulation (macro)
 *
 * @param
 *
 * @retval
 */
.macro exception_return_arg
	@ ldmfd	sp!, {r1}
	@ msr	spsr, r1
	@ push	{r1}
	@ mov	r1, #0
	@ dsb
	@ pop	{r1}
	@ ldmfd	sp!, {r1-r12, lr}
  @ SCR_FIQ_SET
  @ SCR_NS_SET
  @ movs pc, lr
  pop {r1-r12,lr}
  SCR_NS_SET
  rfefd sp!
.endm

/**
 * C function call prolog (macro)
 *
 * @param
 *
 * @retval
 */
.macro call_c_function_arg func_name
  SCR_NS_CLEAR
  srsfd sp!, #MONITOR_MODE
  push {r1-r12,lr}
  @ SCR_FIQ_CLEAR
	@ stmfd   sp!, {r1-r12, lr}
	@ mrs	r4, spsr
	@ stmfd   sp!, {r4}
	bl	\func_name
.endm


/**
 * Monitor vector table
 *
 * @param
 *
 * @retval
 */
.align 8
.global _monitor_vector_table
	/** Monitor Vector Table definition */
_monitor_vector_table:
	b	.				@ Not available on MON
	b	.				@ Not available on MON
	ldr	pc, __mon_smc_handler
	ldr	pc, __mon_prefetch_handler
	ldr	pc, __mon_abort_handler
	b	.				@ Reserved for HYP
	ldr	pc, __mon_irq_handler		@ Should never come here
	ldr	pc, __mon_fiq_handler

__mon_smc_handler:
	.word	_mon_smc_handler
__mon_prefetch_handler:
	.word	mon_prefetch_handler
__mon_abort_handler:
	.word	mon_abort_handler
__mon_irq_handler:
	.word	mon_irq_handler
__mon_fiq_handler:
	.word 	_mon_fiq_handler


/**
 * Monitor SMC handler
 *
 * @param
 *
 * @retval
 */
 @ .align 8
.globl _mon_smc_handler
	/** Monitor - SMC handler */
_mon_smc_handler:
	cmn	r0, #0
	blt	board_smc
	cmp 	r0, #LTZVISOR_SCHED_SYSCALL
	beq	_mon_scheduler
  /* Check if SMC came with request for booting CPU1 */
  cmp   r0, #LTZVISOR_BOOT_CPU1
  beq _secondary_wake_cpu1
	movs	pc, lr

  /**
   *
   *
   * @param
   *
   * @retval
   */
board_smc:
	call_c_function_arg board_handler
	exception_return_arg
  @ movs pc, lr

/*
    _wake_cpu1 is part of ZedBoard's LTZVisor SMP library code. It follows
    ZedBoard's protocol of waking up CPU1:
        1. Write address of LTZVisor CPU1 entry code to 0xFFFFFFF0
        2. Issue SEV command - this triggers CPU1 jumping to address written in
           0xfffffff0

    This function boots LTZVisor only! CPU1 NS Guest is booted with
    _secondary_wake_cpu1.
*/
.globl _wake_cpu1
_wake_cpu1:
  ldr r0,=0xfffffff0    @ location to which code address needs to be written
  ldr r1,=_cpu1_entry_address   @ fetch start address of code
  str r1,[r0]   @ store start address to predefined location
  @ bl flush_icache_and_dcache
  dmb         @ wait till address is written to 0xfffffff0
  sev         @ issue Send Event instruction
  bl wait_till_cpu1_boots
  b ltzvisor_kickoff    @ go back to NS World

/*
    Start booting sequence of CPU1 NS Guest!
    R1 holds address of initalization code for CPU1 NS Guest: this parameter
    must be passed to LTZVisor by NS Guest.
*/
.globl _secondary_wake_cpu1
_secondary_wake_cpu1:
  @ push {r0-r12,lr}
  @ bl flush_icache_and_dcache
  @ pop {r0-r12,lr}
  SCR_NS_CLEAR
  srsfd sp!, #MONITOR_MODE
  ldr r3, =0xfffffff0
  str r1, [r3]
  dmb
  dsb
  isb
  sev
  SCR_NS_SET
  @ movs pc, lr
  rfefd sp!


/**
 * Monitor FIQ handler
 *
 * @param
 *
 * @retval
 */
@ .globl _mon_fiq_handler
@ 	/** Monitor - FIQ handler */
@ _mon_fiq_handler:
@ 	sub	lr, lr, #4			@ Adjust LR
@ 	push	{r4, lr}
@ 	push	{r0 - r3}
@ 	/* ACK FIQ */
@ 	mrc 	p15,4,r0,c15,c0,0		@ Read Periph Base Addr
@ 	add 	r0,r0, #0x0100			@ Add GIC interface offset (A9)
@ 	ldr 	r1,[r0,#_ASM_GIC_INT_ID]	@ Get interrupt ID
@ 	ldr 	r2,=interrupt
@ 	str 	r1,[r2]
@ 	ldr 	r0,[r0,#0x0C]			@ ACK FIQ
@ 	/* Save NS context */
@ 	SCR_NS_CLEAR				@ Disable NS bit and route FIQs in mon
@ 	GET_NS_WORLD_CONTEXT
@ 	WORLD_CONTEXT_SAVE
@ 	/* Handling FIQ */
@ 	ldr 	r2,=interrupt
@ 	ldr 	r0,[r2]
@ 	//bl 	ttc_interrupt_clear		/* FIXME - supposing always ttc_isr */
@ 	bl 	fiq_handler			/* FIXME - supposing always ttc_isr */
@ 	ldr 	r2,=interrupt
@ 	ldr 	r0,[r2]
@ 	ldr 	r1,=0x00
@ 	bl 	interrupt_clear
@ 	/* Restore FIQ state */
@ 	pop 	{r1-r2}				@ Pop SPSR_FIQ and LR_FIQ
@ 	mov 	r0, sp				@ Update stack pointer
@ 	cps 	#FIQ_MODE
@ 	msr 	spsr,r1
@ 	mov 	lr,r2
@ 	ldmfd 	r0, {r8-r13}			@ Restore all banked FIQ regs (R8-R14)
@ 	add 	r0,r0,#(6*4)			@ Update stack pointer
@ 	/* Get back to MON */
@ 	cps 	#MONITOR_MODE
@ 	mov 	sp,r0
@ 	/* Go S world */
@ 	pop 	{r0-r1}
@ 	msr 	spsr,r0
@ 	mov 	lr,r1
@ 	ldmfd 	sp, {r0-r14}^
@ 	add 	sp,sp,#(15*4)
@ 	movs	pc,lr				@ Return from exception (MON -> S_SVC)

.globl _mon_fiq_handler
	/** Monitor - FIQ handler */
_mon_fiq_handler:
	sub	lr, lr, #4			@ Adjust LR

  push {r1}
  mov 	r1,#0
	mcr	p15, 0, r1, c7, c5, 6
  pop {r1}

  SCR_NS_CLEAR				@ Disable NS bit and route FIQs in mon
  @ dsb
  @ isb
	NSWORLD_CONTEXT_SAVE

	mov r0, sp

	cps #FIQ_MODE
	ldmfd    r0!, {r1, sp, lr}
	msr		spsr, r1
	ldmfd 	r0!, {r8-r12}

	@ cps #IRQ_MODE
	@ ldmfd   r0!, {r1, sp, lr}
	@ msr	 	spsr, r1

	cps #UNDEFINED_MODE
	ldmfd   r0!, {r1, sp, lr}
	msr	 	spsr, r1

	cps #ABORT_MODE
	ldmfd   r0!, {r1, sp, lr}
	msr	 	spsr, r1

	cps #SYSTEM_MODE
	ldmfd   r0!, {r1, sp, lr}
	msr	 	spsr, r1

	cps #SUPERVISOR_MODE
	ldmfd   r0!, {r1, sp, lr}
	msr	 	spsr, r1

	cps #MONITOR_MODE
	mov sp, r0

  pop 	{r0}
	vpop	{d16-d31}
	vpop	{d0-d15}
	vmsr  fpscr, r0

  pop {r1}
  msr spsr, r1

	pop {r0-r12,r14}

  movs pc, lr
  @ rfefd sp!


/**
 * Monitor scheduler - schedule NS_guest
 *
 * @param
 *
 * @retval
 */
@ .globl _mon_scheduler
@ _mon_scheduler:
@ 	stmfd 	sp,{r0-r14}^			@ Push SYS regs to monitor stack
@ 	sub 	sp, sp, #(15*4)      		@ Point stack to the top
@ 	mrs 	r0,spsr				@ Read the SPSR
@ 	stmfd	sp, {r0,lr}			@ Push SPSR_mon & lr_mon
@ 	sub 	sp, sp, #(2*4)      		@ Update stack pointer
@ 	/* Save FIQ state */
@ 	mov	r0, sp				@ Save pointer to monitor stack
@ 	cps #FIQ_MODE
@ 	stmfd 	r0,{r8-r13}			@ Save all banked FIQ regs (R8-R14)
@ 	sub 	r0, r0, #(6*4)      		@ Update stack pointer
@ 	mrs 	r1,spsr				@ Read the SPSR_FIQ
@ 	stmfd	r0, {r1,lr}			@ Push SPSR_FIQ and lr_FIQ
@ 	sub 	r0, r0, #(2*4)      		@ Update stack pointer
@ 	/* Get back to MON */
@ 	cps #MONITOR_MODE
@ 	mov 	sp, r0
@ 	/* Restore NS context */
@ 	GET_NS_WORLD_CONTEXT
@ 	WORLD_CONTEXT_RESTORE
@ 	/* Set NS bit and go NS world */
@ 	SCR_NS_SET
@ 	isb
@ 	dsb
@ 	movs    pc, lr				@ Return from exception (MON -> NS_SVC)

.globl _mon_scheduler
_mon_scheduler:
  @ srsfd sp!, #MONITOR_MODE
  @ cps #MONITOR_MODE

	push {r0-r12,r14}

  mrs r1, spsr
  push {r1}

  @ stmfd	sp!, {r0,r1}
  @ sub 	sp, sp, #(2*4)

  fmrx  r1,  fpscr
	vpush {d0-d15}
  vpush	{d16-d31}
  push	{r1}

  cps #MONITOR_MODE

	mov r0, sp

	cps #SUPERVISOR_MODE
	mrs	 r1, spsr
	stmfd    r0!, {r1, sp, lr}

	cps #SYSTEM_MODE
	mrs	 r1, spsr
	stmfd   r0!, {r1, sp, lr}

	cps #ABORT_MODE
	mrs	 r1, spsr
	stmfd    r0!, {r1, sp, lr}

	cps #UNDEFINED_MODE
	mrs	 r1, spsr
	stmfd    r0!, {r1, sp, lr}

	@ cps #IRQ_MODE
	@ mrs	 r1, spsr
  @ stmfd    r0!, {r1, sp, lr}

	cps #FIQ_MODE
  stmfd 	 r0!, {r8-r12}
	mrs	 r1, spsr
	stmfd    r0!, {r1, sp, lr}

	cps #MONITOR_MODE
	mov sp, r0

  NSWORLD_CONTEXT_RESTORE

	/* Set NS bit and go NS world */
  SCR_NS_SET
	isb
	dsb
	movs    pc, lr
