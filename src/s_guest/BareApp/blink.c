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
void secure_yield( void * pvParameters );
void setup_ttc_context (void);

uint32_t toggle;
extern uint8_t toggle_mode;
uint32_t end_counter_value;
uint32_t measures, average_value, counts, max_count;

void print_warning(uint32_t arg)
{
	printk("Arg 0x%x\n", arg);
}

int main() {

	toggle_mode = 1;
	average_value = 0;
	counts = 0;
	measures = 0;
	max_count = 0;

	/** Initialize hardware */
	// hw_init();
	// ttc_init(TTC1,TTCx_2,INTERVAL);
	// vHwSetup();

	printk(" * Secure bare metal VM: running ... \n\t");
	// extern uint32_t _heap;

	/** Generate tick every 1s */
	// tick_set(100000);

	/* Calling Blinking Task (LED blink at 1s) */
	// led_blink((void*)0);
	// printk("led blink addr: 0x%x\n", &_heap);
	xTaskCreate( led_blink, "task", 700, NULL, 2, NULL );
	// xTaskCreate( secure_yield, "task", 1024, NULL, 1, NULL );
	vTaskStartScheduler();

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
	/** 4GPIO (LED) in FPGA fabric */
	static uint32_t *ptr = (uint32_t *) 0x41200000;
	unsigned int i = 0;

	for( ;; ){
		if(toggle_mode)
		{
			toggle ^=0xFF;
			*ptr = toggle;
		}
		else
		{
			*ptr = toggle;
		}
		// printk("Ending counter value: %d\n", end_counter_value);
		// if(i != 0)
		// 	print_ttc_value();
		// i++;

		// YIELD();
		// setup_ttc_context();
		vTaskDelay( 1000 / portTICK_RATE_MS);
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

void secure_yield( void * pvParameters )
{
	while(1){
		YIELD();
		// vTaskDelay(1000/portTICK_RATE_MS);
	}
}

void vApplicationIdleHook( void )
{
	// printk("yield\n");
	YIELD();
}

void setup_ttc_context (void)
{
	ttc_request(TTC1,TTCx_2,1000);
	ttc_enable(TTC1,TTCx_2);
}

void print_ttc_value (void)
{
	float res;
	if(measures < 500)
	{
		end_counter_value = ttc_read_counter(TTC1, TTCx_2);
		if(max_count < end_counter_value)
		{
			max_count = end_counter_value;
		}
		measures++;
		counts += end_counter_value;
		// average_value =(unsigned int) counts/measures;
		printk("Complete value of %d counts: %d\nMax value: %d\n", measures, counts, max_count);
	}
}
