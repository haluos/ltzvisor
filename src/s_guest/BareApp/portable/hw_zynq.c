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
 * [hw_zynq.c]
 *
 * This file contains hardware-related initializations for Zynq.
 *
 * (#) $id: hw_zynq.c 02-10-2017 s_pinto$
*/

#include<hw_zynq.h>
#include<zynq_spi.h>

extern tHandler* sfiq_handlers[NO_OF_INTERRUPTS_IMPLEMENTED];

/**
 * Zynq-specific hardware initialization
 *
 * @param
 *
 * @retval
 */
void hw_init( void ){

  /** Initialize TTC1_2 as S Tick */
  ttc_init(TTC0,TTCx_2,INTERVAL);

  /** Config TTC1_2 ISR*/
  interrupt_enable(TTC0_TTCx_2_INTERRUPT,TRUE);
  // interrupt_enable(SPI_1_INTERRUPT,TRUE);
  interrupt_target_set(TTC0_TTCx_2_INTERRUPT,0,1);
  interrupt_priority_set(TTC0_TTCx_2_INTERRUPT,5);

  interrupt_enable(SPI_1_INTERRUPT,TRUE);
  interrupt_target_set(SPI_1_INTERRUPT, 0, 1);
  interrupt_priority_set(SPI_1_INTERRUPT, 4);

}

void hw_init_2( void ){

    /** Initialize TTC1_2 as S Tick */
    ttc_init(TTC1,TTCx_1,INTERVAL);

    /** Config TTC1_2 ISR*/
    interrupt_enable(TTC1_TTCx_1_INTERRUPT,TRUE);
    interrupt_target_set(TTC1_TTCx_1_INTERRUPT,0,1);
    interrupt_priority_set(TTC1_TTCx_1_INTERRUPT,2);

}

uint32_t tick_set_2( uint32_t time ){

  uint32_t ret = 1;

  /** Set tick rate */
  ret = ttc_request(TTC1, TTCx_1, time);

  /** Start counting */
  ttc_enable(TTC1, TTCx_1);

  return ret;
}

//uint32_t time_of_timer_0(){
//  return read_ttc(TTC1, TTCx_1);// / (20*542);
//}


uint32_t time_of_timer_1(){
    return read_ttc(TTC1, TTCx_2);
}

void wait_until_20ms(){
    uint32_t now = 0;
    while(now < 630){
        now = time_of_timer_1();
    }
}


/**
 * Set secure world tick
 *
 * @param   time = time in useconds
 *
 * @retval
 */
uint32_t tick_set( uint32_t time ){

  uint32_t ret = 1;

  /** Set tick rate */
  ret = ttc_request(TTC0, TTCx_2, time);

  /** Start counting */
  ttc_enable(TTC0, TTCx_2);

  return ret;
}