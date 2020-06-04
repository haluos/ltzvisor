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
 * [cpu_vcpu.h]
 *
 * This file contains (V)CPU-specific registers definition (header).
 *
 * (#) $id: cpu_vcpu.h 10-05-2015 s_pinto & j_pereira $
*/

#ifndef __ARMV7_CPU_VCPU_H
#define __ARMV7_CPU_VCPU_H

#include <types.h>

/** ARMv7-A CPU core registers */
struct core_regs {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t r12;
	uint32_t spsr_mon;
	uint32_t lr_mon;
	uint32_t spsr_svc;
	uint32_t r13_svc;
	uint32_t lr_svc;
	uint32_t spsr_sys;
	uint32_t r13_sys;
	uint32_t lr_sys;
	uint32_t spsr_abt;
	uint32_t r13_abt;
	uint32_t lr_abt;
	uint32_t spsr_undef;
	uint32_t r13_undef;
	uint32_t lr_undef;
	uint32_t spsr_irq;
	uint32_t r13_irq;
	uint32_t lr_irq;
	uint32_t d0;
	uint32_t d1;
	uint32_t d2;
	uint32_t d3;
	uint32_t d4;
	uint32_t d5;
	uint32_t d6;
	uint32_t d7;
	uint32_t d8;
	uint32_t d9;
	uint32_t d10;
	uint32_t d11;
	uint32_t d12;
	uint32_t d13;
	uint32_t d14;
	uint32_t d15;
	uint32_t d16;
	uint32_t d17;
	uint32_t d18;
	uint32_t d19;
	uint32_t d20;
	uint32_t d21;
	uint32_t d22;
	uint32_t d23;
	uint32_t d24;
	uint32_t d25;
	uint32_t d26;
	uint32_t d27;
	uint32_t d28;
	uint32_t d29;
	uint32_t d30;
	uint32_t d31;
	uint32_t fpscr;
};

/** ARMv7-A CPU CP15 registers */
struct cp15_regs {
	uint32_t c0_CSSELR;      /* Cache Size Selection Register */
	uint32_t c1_SCTLR;       /* System Control Register */
	uint32_t c1_ACTLR;       /* Auxilliary Control Register */
	uint32_t c2_TTBR0;       /* Translation Table Base Register 0 */
	uint32_t c2_TTBR1;       /* Translation Table Base Register 1 */
	uint32_t c2_TTBCR;       /* Translation Table Base Register Control */
	uint32_t c3_DACR;        /* Domain Access Control Register */
	uint32_t c5_DFSR;        /* Data Fault Status Register */
	uint32_t c5_IFSR;        /* Instruction Fault Status Register */
	uint32_t c6_DFAR;        /* Data Fault Address Register */
	uint32_t c6_IFAR;        /* Instruction Fault Address Register */
	uint32_t c7_PAR;         /* Physical Address Register */
	uint32_t c10_PRRR;       /* PRRR */
	uint32_t c10_NMRR;       /* NMRR */
	uint32_t c12_VBAR;       /* VBAR register */
	uint32_t c13_FCSEIDR;    /* FCSE PID Register */
	uint32_t c13_CONTEXTIDR; /* Context ID Register */
	uint32_t c13_TPIDRURW;   /* User Read/Write Thread and Process ID */
	uint32_t c13_TPIDRURO;   /* User Read-only Thread and Process ID */
	uint32_t c13_TPIDRPRW;   /* Privileged only Thread and Process ID */
};

/** ARMv7-A CPU GIC registers */
struct gic_regs {
	uint32_t gic_icdiser[4];
};

/** ARMv7-A CPU architecture registers */
struct vcpu_arch {
	struct core_regs vcpu_regs_core;
	struct cp15_regs vcpu_regs_cp15;
	struct gic_regs vcpu_regs_gic;
};

struct sys_regs {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t r12;
	uint32_t spsr_mon;
	uint32_t lr_mon;
	uint32_t spsr_svc;
	uint32_t r13_svc;
	uint32_t lr_svc;
	uint32_t spsr_sys;
	uint32_t r13_sys;
	uint32_t lr_sys;
	uint32_t spsr_fiq;
	uint32_t r13_fiq;
	uint32_t lr_fiq;
	uint32_t r8_fiq;
	uint32_t r9_fiq;
	uint32_t r10_fiq;
	uint32_t r11_fiq;
	uint32_t r12_fiq;
};

#endif /* __ARMV7_CPU_VCPU_H */
