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
  SET_SCR_NS
	ldmfd	sp!, {r1-r12, pc}^
  @ pop {r1-r12}
	@ SET_SCR_NS
  @ dsb
  @ isb
  @ rfefd sp!
.endm

/**
 * C function call prolog (macro)
 *
 * @param
 *
 * @retval
 */
.macro cpu1_call_c_function_arg func_name
  CLEAR_SCR_NS
	stmfd   sp!, {r1-r12, lr}
	mrs	r4, spsr
	stmfd   sp!, {r4}
	@ CLEAR_SCR_NS
  @ dsb
  @ isb
  @ srsfd sp!, #MONITOR_MODE
  @ push {r1-r12}
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
	orr r2, r2, #0x00000001
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
	movs pc, r8

@ .align 8
.globl _smc_handler
	/** Monitor - SMC handler */
_smc_handler:
	cmn	r0, #0
	blt	cpu1_board_smc
	ldr r3, =LTZVISOR_NS_WFI
	cmp r0, r3
	beq _cpu1_ns_wfi
	cmp r0, #0x9
  beq cpu1_communicate
	movs pc, lr

@ .globl cpu1_board_smc
@ cpu1_board_smc:
@ 	cpu1_call_c_function_arg cpu1_board_handler
@ 	cpu1_exception_return_arg
@ 	movs pc, lr

cpu1_board_smc:
	@ call_c_function_arg board_handler
	@ exception_return_arg
	@ srsfd sp!, #MONITOR_MODE
	@ push {r0-r12,lr}
  @ bl setup_ttc_context
  @ pop {r0-r12,lr}
  @ push {r0,r1}
  @ ldr r0, =0xf8002010
  @ ldr r1, [r0]
  @ orr r1,r1,#(1<<4) @reset TTC1
  @ bic r1,r1,#1 @enable TTC1
  @ str r1, [r0]
  @ pop {r0,r1}
	CLEAR_SCR_NS
  cmp r0, #-30        @ Read handler
  bne 1f
  ldr r0, [r1]
	b smc_handler_end
1:
  cmp r0, #-31        @ Write handler
	bne 2f
  str r2, [r1]
	dsb
	isb
	b smc_handler_end
2:
  cmp r0, #-32        @ Power control CP15 write handler
	bne smc_handler_end
  mrc p15, 0, r0, c15, c0, 0
  orr r0, r0, #1
  mcr p15, 0, r0, c15, c0, 0
  dsb
  isb
smc_handler_end:
	SET_SCR_NS
	@ push {r0,r1}
  @ ldr r0, =0xf8002010
  @ ldr r1, [r0]
  @ orr r1,r1,#1 @stop TTC1
  @ str r1, [r0]
  @ pop {r0,r1}
  @ push {r0-r12,lr}
  @ bl print_ttc_value
  @ pop {r0-r12,lr}
	movs pc, lr
	@ rfefd sp!

_cpu1_ns_wfi:
	mrs r3, cpsr
	bic r3, #0x80
	msr cpsr_c, r3
	CLEAR_SCR_NS			@ Make sure that NS context variable is reachable (in Secure memory)
	SAVE_CPU1_NS_CONTEXT
	SET_SCR_NS_WFI
	dsb
	isb
	wfi
	movs pc, lr

cpu1_communicate:
  push {r0-r12,lr}
	bl flush_icache_and_dcache
  bl cachel2_invalidate
  bl cachel2_clean
	mov r0, #10
  mov r1, #1
  bl interrupt_IPI_generate
  pop {r0-r12,lr}
  @ cpu1_call_c_function_arg print_message
  @ cpu1_exception_return_arg
  movs pc, lr

.globl _prefetch_handler
_prefetch_handler:
	push {r0-r12,lr}
	bl cpu1_prefetch_handler
	pop {r0-r12,lr}
	b .

.globl _abort_handler
_abort_handler:
	push {r0-r12,lr}
	bl cpu1_abort_handler
	pop {r0-r12,lr}
	b .

.globl _irq_handler
_irq_handler:
	push {r0-r12,lr}
	bl cpu1_irq_handler
	pop {r0-r12,lr}
	b .

.globl _fiq_handler
_fiq_handler:
	push {r0-r12,lr}
	bl cpu1_fiq_handler
	pop {r0-r12,lr}
	b .
