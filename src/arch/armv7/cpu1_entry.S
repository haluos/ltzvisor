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
 * [cpu_entry.S]
 *
 * This file contains ARMv7-A specific boot code.
 *
 * (#) $id: cpu_entry.S 03-05-2015 s_pinto & j_pereira $
 * (#) $id: cpu_entry.S 16-09-2017 s_pinto (modified)$
*/

#include <cpu_defines.h>
#include <asm-offsets.h>
#include <platform_asm.h>

.text
.section .cpu1_mon_entry

.globl _cpu1_secure_entry
_cpu1_secure_entry:

__start_secure_core_cpu1:

	/** Set CPU State */
__setup_CPU1:
	.global start_boot
	bl start_boot
	@ mrs r0, cpsr
	@ bic r0, #0x80
	@ bic r0, #0x40
	@ msr cpsr, r0
	@ bl print_addr

	mcr	 15,0,r0,cr7,cr5,0		/* Invalidate Instruction cache */
	mcr	 15,0,r0,cr7,cr5,6		/* Invalidate branch predictor array */
	dsb
	isb					/* make sure it completes */

	ldr	r4, =0
	mcr	 15,0,r4,cr1,cr0,0		/* disable the ICache and MMU */
	isb					/* make sure it completes */

	/* Set SCTLR */
	mrc	p15, 0, r1, c1, c0, 0			@ Read SCTLR
	@ mov r0, r1
	@ bl print_addr
	bic	r1, r1, #0x10000000			@ Clear TEX bit
	bic	r1, r1, #0x00002000			@ Clear Vectors bit
	@ bic r1, r1, #0x8000000
	@ orr r1, r1, #0x400
	mcr	p15, 0, r1, c1, c0, 0			@ Write SCTLR
	/* Set NSACR */
	mrc	p15, 0, r1, c1, c1, 2			@ Read NSACR
	ldr	r2, =NSACR_REG_VAL
	orr	r1, r1, r2				@ Mask r1 with r2
	mcr	p15, 0, r1, c1, c1, 2			@ Write NSACR
	isb
	@ mrc	p15, 0, r0, c1, c1, 2
	@ bl print_addr
	/* Non-Route FIQs Monitor */
	mrc	p15, 0, r1, c1, c1, 0			@ Read SCR
	@ mov r0, r1
	@ bl print_addr
	bic  	r1, r1, #SCR_FIQ_BIT			@ Clear FIQ bit (disable route FIQs monitor)
	@ orr r1, r1, #SCR_HCR_BIT
	mcr	p15, 0, r1, c1, c1, 0			@ Write SCR
	/* Set Auxiliary register reset value */
	@ mrc	p15, 0, r0, c1, c0, 1			@ Read ACTLR
	@ ldr  	r0, =0x47				@ Clear registers
	@ mcr	p15, 0, r0, c1, c0, 1			@ Write ACTLR
	mrc p15, 0, r1, c1, c0, 1			@ Read Auxiliary Control Register
	orr r1, r1, #AUXREG_SMP				@ Set CPU0 for SMP mode (cache coherency and chache and MMU broadcast)
	mcr p15, 0, r1, c1, c0, 1

	@ ldr r0, =0xF8F00000
	@ ldr r1, [r0]
	@ orr r1, r1, #0x1
	@ str r1, [r0]

	/** Set secure vector table (VBAR) */
	ldr	r0, =_cpu1_secure_vector_table		@ Read the Secure Vector Table's Base Address
	@ ldr	r0, =_secure_vector_table
	@ ldr r1, =0x0
	@ mcr	p15, 0, r1, c12, c0, 0
	@ isb
	mcr	p15, 0, r0, c12, c0, 0			@ Write VBAR
	@ isb
	@ dsb
	@ mrc	p15, 0, r0, c12, c0, 0
	@ bl print_addr

	/** Set monitor vector table (MVBAR) */
	ldr	r0, =_cpu1_monitor_vector_table		@ Read the Monitor Vector Tables Base Address
	@ bl print_addr
	mcr	p15, 0, r0, c12, c0, 1			@ Write MVBAR
	@ isb
	@ dsb
	@ mrc	p15, 0, r0, c12, c0, 1
	@ bl print_addr


	/** Setup Stacks for all CPU modes */
__setup_stacks:
	/* FIQ mode */
	msr	cpsr_c,#(FIQ_MODE | IRQ_BIT | FIQ_BIT)		@ Change CPSR to Fiq MODE and disable FIQ and IRQ
	ldr	r1,=_cpu1_fiq_stack
	add	r1, r1, r0, lsl #STACK_SIZE_SHIFT
	add	sp, r1, #STACK_SIZE
	/* ABORT mode */
	msr	cpsr_c,#(ABORT_MODE | IRQ_BIT | FIQ_BIT)	@ Change CPSR to Abort MODE and disable FIQ and IRQ
	ldr	r1,=_cpu1_abort_stack
	add	r1, r1, r0, lsl #STACK_SIZE_SHIFT
	add	sp, r1, #STACK_SIZE
	/* UNDEFINED mode */
	msr	cpsr_c,#(UNDEFINED_MODE | IRQ_BIT | FIQ_BIT)	@ Change CPSR to Undefined MODE and disable FIQ and IRQ
	ldr	r1,=_cpu1_undefined_stack
	add	r1, r1, r0, lsl #STACK_SIZE_SHIFT
	add	sp, r1, #STACK_SIZE
	/* SYSTEM mode */
	msr	cpsr_c,#(SYSTEM_MODE | IRQ_BIT | FIQ_BIT)	@ Change CPSR to System MODE and disable FIQ and IRQ interrupts
	ldr	r1,=_cpu1_user_stack
	add	r1, r1, r0, lsl #STACK_SIZE_SHIFT
	add	sp, r1, #STACK_SIZE
	/* MONITOR mode */
	msr	cpsr_c,#(MONITOR_MODE | IRQ_BIT | FIQ_BIT)	@ Change CPSR to Monitor MODE and disable only IRQ interrupts
	ldr	r1,=_cpu1_monitor_stack
	add	r1, r1, r0, lsl #STACK_SIZE_SHIFT
	add	sp, r1, #STACK_SIZE


	/** Handling cache and MMU subsystems */
__init_vmemory:
	/* Disable MMU */
	mrc 	p15, 0, r1, c1, c0, 0			@ Read SCTLR register
	bic 	r1, r1, #SCTLR_MMU_BIT			@ Clear M bit (disable MMU)
	mcr 	p15, 0, r1, c1, c0, 0			@ Write SCTLR register
	/* Disable L1 Caches */
	mrc 	p15, 0, r1, c1, c0, 0			@ Read SCTLR register
	bic 	r1, r1, #SCTLR_DCACHE_BIT		@ Clear C bit (disable D-Cache)
	bic 	r1, r1, #SCTLR_ICACHE_BIT		@ Clear I bit (disable I-Cache)
	mcr 	p15, 0, r1, c1, c0, 0			@ Write SCTLR register

	bl		mmu_tlb_invalidate
	bl		dCache_invalidate
	bl		iCache_invalidate

	@ /* Invalidate Instruction cache */
	@ mov 	r1,#0
	@ mcr 	p15, 0, r1, c7, c5, 0			@ Instruction Cache Invalidate All
	@ /* Invalidate Data caches */
	@ mov	r0, #1
	@ bl	data_cache_clean_invalidate_all		@ Invalidate data cache
	/* Invalidate Branch Predictor arrays */
	mov 	r1,#0
	mcr	p15, 0, r1, c7, c5, 6			@ Invalidate BP
	@ /* Invalidate TLBs */
	@ mov 	r1, #0x0
	@ mcr 	p15, 0, r1, c8, c3, 0			@ Invalidate entire unified TLB Inner Shareable

	ldr		r0, =s_main_page_table
	bl		mmu_ttbr0_set

	ldr		r0, =0x55555555
	bl 		mmu_set_domain_access

	mrc p15, 0, r1, c1, c0, 1			@ Read Auxiliary Control Register
	orr r1, r1, #(0x1 << 0)			@ Set CPU0 for SMP mode (cache coherency and chache and MMU broadcast)
	mcr p15, 0, r1, c1, c0, 1

	bl		iCache_enable
	bl		dCache_enable

	bl		mmu_enable

	mrc	p15,0,r0,c1,c0,0		/* flow prediction enable */
	orr	r0, r0, #(0x01 << 11)		/* #0x8000 */
	mcr	p15,0,r0,c1,c0,0

	@ mrs	r0, cpsr			/* get the current PSR */
	@ @ bic	r0, r0, #0x100			/* enable asynchronous abort exception */
	@ bic	r0, r0, #0x80
	@ msr	cpsr_c, r0


	/** Handling VFP and NEON */
__init_vfp:
	/* FIX IT */

	/**  Call Main */
__call_main:
.globl LTZVisor_CPU1_entry
	bl	LTZVisor_CPU1_entry			@ Jump to LTZVisor entry function
	/* This point should never be reached */
	b	.

@ .align 8
.globl _cpu1_secure_vector_table
_cpu1_secure_vector_table:
	b	.
	b	.
	b	.
	b .
	b .
	b	.
	b .
	b .
