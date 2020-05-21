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
#include <cpu_defines.h>
#include <ltzvisor_syscall_asm.h>

/**
 * Exception return emulation (macro)
 *
 * @param
 *
 * @retval
 */
.macro cpu1_exception_return_arg
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
.macro cpu1_call_c_function_arg func_name
	stmfd   sp!, {r1-r12, lr}
	mrs	r4, spsr
	stmfd   sp!, {r4}
	bl	\func_name
.endm

.macro SET_SCR_NS_WFI
	push {r0}
	mrc p15, 0, r0, c1, c1, 0
	orr r0, r0, #SCR_NS_BIT
	orr r0, r0, #SCR_IRQ_BIT
	bic r0, r0, #SCR_FIQ_BIT
	mcr p15, 0, r0, c1, c1, 0
	pop {r0}
.endm

.macro SET_SCR_NS
	push {r0}
	mrc p15, 0, r0, c1, c1, 0
	orr r0, r0, #SCR_NS_BIT
	bic r0, r0, #SCR_IRQ_BIT
	bic r0, r0, #SCR_FIQ_BIT
	mcr p15, 0, r0, c1, c1, 0
	pop {r0}
.endm

.macro CLEAR_SCR_NS
	push {r0}
	mrc p15, 0, r0, c1, c1, 0
	bic r0, r0, #SCR_NS_BIT
	bic r0, r0, #SCR_IRQ_BIT
	bic r0, r0, #SCR_FIQ_BIT
	mcr p15, 0, r0, c1, c1, 0
	pop {r0}
.endm

.macro GET_CPU1_NS_CONTEXT_ADDR reg
	ldr \reg, =NS_CPU1_Guest
	add \reg, \reg, #_ASM_ARCH_REGS_OFFSET
.endm

.macro SAVE_CPU1_NS_CONTEXT
	push {lr} 					@ LR contains return address for NS, save it before changing it
	GET_CPU1_NS_CONTEXT_ADDR lr			@ Next regs to save are R0-R12 (only LR can be used here)
	@ R0-R12 contain NS values that need to be preserved
	stmia lr!, {r0-r12}			@ Save R0-R12 to CPU1 NS Guest global var
	@ R0-R12 now saved, we can overwrite them
	mov r0, lr			@ Save pointer to next item in NS context
	@ Next regs to save are CPSR_mon and LR_mon: first, restore LR
	pop {lr}
	mrs r1, spsr
	stmia r0!, {r1, lr}
	@ Next regs to save are Supervisor related regs: first, go to SVC
	cps #SUPERVISOR_MODE
	mrs r1, spsr
	stmia r0!, {r1, r13, lr}
	@ Next: System mode
	cps #SYSTEM_MODE
	mrs r1, spsr
	stmia r0!, {r1, r13, lr}
	@ Next: Abort mode
	cps #ABORT_MODE
	mrs r1, spsr
	stmia r0!, {r1, r13, lr}
	@ Next: Undefined mode
	cps #UNDEFINED_MODE
	mrs r1, spsr
	stmia r0!, {r1, r13, lr}
	@ Next: IRQ mode
	cps #IRQ_MODE
	mrs r1, spsr
	stmia r0!, {r1, r13, lr}
	@ IRQ banked registers need to be preserved too
	stmia r0!, {r8-r12}
	@ Go back to Monitor
	cps #MONITOR_MODE
.endm

.macro GET_CPU1_NS_CONTEXT
	@ LR contains NS return address, which is saved in NS context -> no need to push LR
	GET_CPU1_NS_CONTEXT_ADDR lr
	@ Refresh R0-R12 with NS values
	ldmia lr!, {r0-r12}
	@ R0 and R1 will be used now and NS value will be lost: need to be pushed
	push {r0,r1}
	mov r0, lr			@ Save pointer to next item in NS context
	@ Next regs to save are CPSR_mon and LR_mon
	ldmia r0!, {r1, lr}
	@ Save Control, Extensions, Status and Flags fields to SPSR
	msr spsr_cxsf, r1
	@ Next regs to refresh are Supervisor related regs: first, go to SVC
	cps #SUPERVISOR_MODE
	ldmia r0!, {r1, sp, lr}
	msr spsr_cxsf, r1
	@ Next: System mode
	cps #SYSTEM_MODE
	ldmia r0!, {r1, sp, lr}
	msr spsr_cxsf, r1
	@ Next: Abort mode
	cps #ABORT_MODE
	ldmia r0!, {r1, sp, lr}
	msr spsr_cxsf, r1
	@ Next: Undefined mode
	cps #UNDEFINED_MODE
	ldmia r0!, {r1, sp, lr}
	msr spsr_cxsf, r1
	@ Next: IRQ mode
	cps #IRQ_MODE
	ldmia r0!, {r1, sp, lr}
	msr spsr_cxsf, r1
	@ Refresh IRQ banked registers
	ldmia r0!, {r8-r12}
	@ Go back to Monitor
	cps #MONITOR_MODE
	pop {r0, r1}
.endm

.align 8
.globl _cpu1_monitor_vector_table
_cpu1_monitor_vector_table:
	b	.				@ Not available on MON
	b	.				@ Not available on MON
	ldr	pc, =_smc_handler   @ CPU1 reserved only for NS Guest -> don't do context switch
	ldr	pc, =_prefetch_handler
	ldr	pc, =_abort_handler
	b	.				@ Reserved for HYP
	ldr	pc, =_irq_handler		@ Should never come here
	ldr	pc, =_fiq_handler

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
  @ orr r2, r2, #0x00000100
	orr r2, r2, #0x00000003
  mcr p15, 0, r2, c1, c1, 0
	isb
	dsb
  /*  Set the ACTLR register for NS World */
  ldr r2, [r0, #_ASM_CP_ACTLR_OFFSET]
  mcr p15, 0, r2, c1, c0, 1
  /*  Set the SCTLR register for NS World */
  ldr r2, [r0, #_ASM_CP_SCTLR_OFFSET]
  mcr p15, 0, r2, c1, c0, 0
	ldr r2, [r0, #_ASM_MON_SPSR_OFFSET]
	msr spsr, r2
	@ bl print_warning
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
	bl flush_icache_and_dcache
	@ dsb
	@ isb
	bl unlock_cpu0
wait:
	wfe
	ldr r0, =0xfffffff0
	ldr r1, [r0]
	cmp r1, #0
	beq wait
	mov r8, r1
	ldr r0, =NS_CPU1_Guest
	/*  Disable routing of FIQ, IRQ and Async Abort Exceptions to Monitor and go
      to NS World */
  SET_SCR_NS
	isb
	dsb
  /*  Set the ACTLR register for NS World */
  ldr r2, [r0, #_ASM_CP_ACTLR_OFFSET]
  mcr p15, 0, r2, c1, c0, 1
  /*  Set the SCTLR register for NS World */
  ldr r2, [r0, #_ASM_CP_SCTLR_OFFSET]
  mcr p15, 0, r2, c1, c0, 0
	ldr r2, [r0, #_ASM_MON_SPSR_OFFSET]
	msr spsr, r2
	@ mrc p15, 0, r2, c1, c1, 0
  @ orr r2, r2, #0x00000001
  @ mcr p15, 0, r2, c1, c1, 0
	mov r3, #0x113
	mrs r4, cpsr
	mvn r5, #0x1f
	and r4, r4, r5
	orr r4, r4, r3
	msr cpsr, r4
	movs pc, r8

@ .align 8
.globl _smc_handler
	/** Monitor - SMC handler */
_smc_handler:
	@ bl print_warning
	@ push {lr}
	cmn	r0, #0
	blt	cpu1_board_smc
	@ push {r3}
	ldr r3, =LTZVISOR_NS_WFI
	cmp r0, r3
	beq _cpu1_ns_wfi
	@ pop {lr}
	@ cpu1_call_c_function_arg print_warning
	@ cpu1_exception_return_arg
	movs pc, lr
	@ bl print_warning
	@ mov pc, lr

.globl cpu1_board_smc
cpu1_board_smc:
	cpu1_call_c_function_arg cpu1_board_handler
	cpu1_exception_return_arg
	@ pop {lr}
	mov pc, lr

_cpu1_ns_wfi:
	push {r3}
	@ mrs r3, spsr
	@ bic r3, #0x80
	@ msr spsr_c, r3
	mrs r3, cpsr
	bic r3, #0x80
	msr cpsr_c, r3
	@ bl flush_icache_and_dcache
	pop {r3}
	@ push {r0}
	@ GET_CPU1_NS_CONTEXT_ADDR r0
	@ pop {r0}
	CLEAR_SCR_NS
	SAVE_CPU1_NS_CONTEXT
	@ dsb
	@ isb
	SET_SCR_NS_WFI
	dsb
	isb
	wfi
	movs pc, lr

.globl _prefetch_handler
_prefetch_handler:
	push {r0-r12,lr}
	bl cpu1_prefetch_handler
	@ cpu1_call_c_function_arg print_warning
	@ cpu1_exception_return_arg
	pop {r0-r12,lr}
	b .

.globl _abort_handler
_abort_handler:
	push {r0-r12,lr}
	bl cpu1_abort_handler
	@ cpu1_call_c_function_arg print_warning
	@ cpu1_exception_return_arg
	pop {r0-r12,lr}
	b .

.globl _irq_handler
_irq_handler:
	bl flush_icache_and_dcache
	@ push {r0-r12, lr}
	@ bl print_warning
	@ cpu1_call_c_function_arg print_warning
	@ cpu1_exception_return_arg
	mrs r3, cpsr
	orr r3, #0x80
	msr cpsr_c, r3
	@ pop {r0-r12, lr}
	CLEAR_SCR_NS
	GET_CPU1_NS_CONTEXT
	SET_SCR_NS
	@ pop {lr}
	isb
	dsb
	movs pc, lr
	@ subs pc, lr, #4

.globl _fiq_handler
_fiq_handler:
	push {r0-r12,lr}
	bl cpu1_fiq_handler
	@ cpu1_call_c_function_arg print_warning
	@ cpu1_exception_return_arg
	pop {r0-r12,lr}
	b .
