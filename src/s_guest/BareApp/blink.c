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
 * [blink.c]
 *
 * This file contains a bare-metal Blink application.
 *
 * (#) $id: blink.c 27-09-2017 s_pinto$
*/

#include<hw_zynq.h>
#include<printk.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

void led_blink( void * pvParameters );
void vHwSetup(void);

struct sys_regs s_context;

void vApplicationIdleHook( void )
{
	// printk("yield\n");
	YIELD();
}

void secure_yield()
{
	while(1){
		YIELD();
		asm volatile("dsb\n"
									"isb");
		// vTaskDelay(1000/portTICK_RATE_MS);
	}
}

#if (configUSE_TICKLESS_IDLE != 0)
void vSecureSleep (uint32_t xSleepTime)		/* xSleepTime is in ticks */
{
	eSleepModeStatus eSleepStatus;

	ttc_disable(TTC0,TTCx_2);

	eSleepStatus = eTaskConfirmSleepModeStatus();

	if( eSleepStatus == eAbortSleep )
	{
		ttc_enable(TTC0,TTCx_2);
	}
	else
	{
		if( eSleepStatus == eNoTasksWaitingTimeout )
		{
			// printk("Yield forever\n");
			YIELD();
		}
		else
		{
			ttc_request(TTC1, TTCx_2, xSleepTime * 10000);
			ttc_enable(TTC1, TTCx_2);
			// printk("Yield\n");
			YIELD();
			ttc_disable(TTC1,TTCx_2);
			// printk("Disable TTC1\n");
			vTaskStepTick(xSleepTime+1);
		}
		// printk("Reset TTC0\n");
		ttc_request(TTC0, TTCx_2, 1 * 10000);
		ttc_enable(TTC0,TTCx_2);
	}
	// printk("Sleep for %d\n", xSleepTime);
	// YIELD();
}
#endif

void print_warning(uint32_t arg)
{
	printk("Arg 0x%x\n", arg);
}

void print_restore (uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3, uint32_t r4, uint32_t r5)
{
	printk("Restore:\nR0: 0x%x, R1: 0x%x, R2: 0x%x,\n R3: 0x%x, R4: 0x%x, R5: 0x%x\n", r0, r1, r2, r3, r4, r5);
}

void print_save (uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3, uint32_t r4, uint32_t r5)
{
	printk("Save:\nR0: 0x%x, R1: 0x%x, R2: 0x%x,\n R3: 0x%x, R4: 0x%x, R5: 0x%x\n", r0, r1, r2, r3, r4, r5);
}

int main() {

	/** Initialize hardware */
	hw_init();
	// vHwSetup();

	printk(" * Secure bare metal VM: running ... \n\t");
	// extern uint32_t _heap;

	/** Generate tick every 1s */
	tick_set(1000000);

	/* Calling Blinking Task (LED blink at 1s) */
	led_blink((void*)0);
	// printk("led blink addr: 0x%x\n", &_heap);
	// xTaskCreate( led_blink, "task", 300, NULL, 2, NULL );
	// xTaskCreate( secure_yield, "task", 300, NULL, 1, NULL );
	// vTaskStartScheduler();

	/* This point will never be reached */
	for( ;; );

}

/**
 * Blink LED "Task"
 *
 * @param
 *
 * @retval
 */
void led_blink( void * parameters ){

	static uint32_t toggle;
	/** 4GPIO (LED) in FPGA fabric */
	static uint32_t *ptr = (uint32_t *) 0x41200000;
	uint32_t state;

	for( ;; ){
		toggle ^=0xF;
		*ptr = toggle;
		YIELD();
		// vTaskDelay( 1000 / portTICK_RATE_MS);
	}
}

void vHwSetup (void)
{
	ttc_init(TTC1,TTCx_2,INTERVAL);

	/** Config TTC1_2 ISR*/
	interrupt_enable(TTC1_TTCx_2_INTERRUPT,TRUE);
	interrupt_target_set(TTC1_TTCx_2_INTERRUPT,0,1);
	interrupt_priority_set(TTC1_TTCx_2_INTERRUPT,31);
}
