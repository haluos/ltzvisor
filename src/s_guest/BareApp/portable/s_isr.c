/*
 * LTZVisor, a Lightweight TrustZone-assisted Hypervisor
 *
 * Copyright (c) TZVisor Project (www.tzvisor.org), 2017-
 *
 * Authors:
 *  Sandro Pinto <sandro@tzvisor.org>
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
 * [s_isr.c]
 *
 * This file contains the interrupt handling.
 *
 * (#) $id: s_isr.c 27-09-2017 s_pinto$
*/

#include <gic.h>
#include <s_isr.h>
#include <printk.h>

uint32_t interrupt;

fiq_handler sfiq_handlers[NO_OF_INTERRUPTS_IMPLEMENTED] = {NULL};

extern uint8_t ttc_flag;

/**
 * Generic FIQ handler
 *
 * @param	interrupt_ = interrupt number
 *
 * @retval
 */
// void sFIQ_handler(uint32_t interrupt_){
//
// 	uint32_t irq_num = interrupt_acknowledge();
//
// 	if (sfiq_handlers[irq_num])
// 		sfiq_handlers[irq_num]();
//
// 	interrupt_clear(irq_num , 0);
// }

void sFIQ_handler(void){

	// printk("FIQ\n");

	uint32_t irq_num = interrupt_acknowledge();

	if (sfiq_handlers[irq_num])
		sfiq_handlers[irq_num]();

	interrupt_clear(irq_num , 0);
}

void register_handler(uint32_t interrupt, fiq_handler handler){

	if(interrupt < NO_OF_INTERRUPTS_IMPLEMENTED){
		sfiq_handlers[interrupt] = handler;
	}

}

void undefined_exception (void)
{
	uint32_t par, state;
	// asm volatile("push {lr}");
	asm volatile ("sub r0, lr, #4");
	asm volatile ("mov %0, r0" : "=r"(par) ::);
	asm volatile("mrs %0, spsr" : "=r"(state)::);
	printk("Undefined at 0x%x, SPSR: 0x%x\n", par, state);
	// asm volatile("pop {lr}");
	// asm volatile("subs pc, lr, #4");
	while(1);
}

void prefetch_exception(void)
{
	uint32_t par, state;
	asm volatile("push {lr}");
	asm volatile ("sub r0, lr, #4");
	asm volatile ("mov %0, r0" : "=r"(par) ::);
	asm volatile("mrs %0, spsr" : "=r"(state)::);
	printk("Prefetch at 0x%x, SPSR: 0x%x\n", par, state);
	asm volatile("pop {lr}");
	asm volatile("subs pc, lr, #4");
	while(1);
}

void abort_exception(void)
{
	uint32_t par, state;
	asm volatile("push {lr}");
	asm volatile ("sub r0, lr, #8");
	asm volatile ("mov %0, r0" : "=r"(par) ::);
	asm volatile("mrs %0, spsr" : "=r"(state)::);
	printk("Abort at 0x%x, SPSR: 0x%x\n", par, state);
	// for(int i=0;i<1000;i++);
	asm volatile("pop {lr}");
	asm volatile("subs pc, lr, #8");
	while(1);
}

void undef_yield(void)
{
	printk("Undef yield!\n");
	#undef YIELD
	#define YIELD()\
		do{\
			printk("Wait flag\n");\
			while(!ttc_flag);\
			ttc_flag = 0;\
		}while(0)
}

void redef_yield(void)
{
	printk("Redef yield\n");
	#undef YIELD
	#define YIELD() \
	do{ \
			asm volatile(".arch_extension sec\n");\
			asm volatile("ldr r0, =0x0ffffff1\n");\
			asm volatile("smc #0");\
	}while(0)
}
