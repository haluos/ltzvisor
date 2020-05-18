/*
 * LTZVisor, a Lightweight TrustZone-assisted Hypervisor
 *
 * Copyright (c) TZVisor Project (www.tzvisor.org), 2017-
 *
 * Authors:
 *  Jose Martins <josemartins90@gmail.com>
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
 * [l2_pl310.c]
 *
 * This file contains the driver for the ARM Cache L2 controller (PL310)
 * 
 * (#) $id: page_table.S 12-04-2015 j_martins $
*/


#include <l2_pl310.h>

static volatile CacheL2pl310* l2_pl310 = (CacheL2pl310*) PL310_BASE;

void cachel2_init(){

	write32( (void *)SLCR_UNLOCK, SLCR_UNLOCK_KEY);
    write32( (void *)RESERVED, 0x020202);
	write32( (void *)SLCR_LOCK, SLCR_LOCK_KEY);

	// setup associativity and size
	l2_pl310->reg1_aux_control &= 0xFFF0EFFF;
	l2_pl310->reg1_aux_control |= (3 << 17); //0x72360000;

	// setup data and tag latencies (according to ZYNQ TRM)
	l2_pl310->reg1_tag_ram_control = 0x111;
	l2_pl310->reg1_data_ram_control = 0X121;

	cachel2_invalidate();

	// l2_pl310->reg2_ev_counter0_cfg = 0x02 << 2;	//data read hit
	// l2_pl310->reg2_ev_counter1_cfg = 0x03 << 2;	//data read req
	// l2_pl310->reg2_ev_counter_ctrl |= 0x07;
}

static inline void cachel2_sync(){
    l2_pl310->reg7_cache_sync = 0;
    while(l2_pl310->reg7_cache_sync);
}

void cachel2_enable(){
    l2_pl310->reg1_control |= 0x01;
    cachel2_sync();
}

void cachel2_disable(){
    l2_pl310->reg1_control &= ~0x01;
    cachel2_sync();
}

void cachel2_invalidate(){
	l2_pl310->reg7_inv_way = 0xFFFF; //invalidate all ways
	while(l2_pl310->reg7_inv_way);
}

void cachel2_clean(){
	l2_pl310->reg7_clean_way = 0xFFFF; //clean all ways
	while(l2_pl310->reg7_clean_way);
}

// void cachel2_resetevents(){
// 	l2_pl310->reg2_ev_counter_ctrl |= 0x06;
// }

// void cachel2_printevents(){

// 	uint32_t ev1, ev0;

// 	ev0 = l2_pl310->reg2_ev_counter0;
// 	ev1 = l2_pl310->reg2_ev_counter1;
	
// 	printk("hits = %d; reqs = %d\n\r", ev0, ev1);
// }


void cachel2_lockdown(uint32_t master, uint32_t ways){

	if(master < NUM_MASTERS){
		l2_pl310->reg9_lockdown[master].d |= ways;
		l2_pl310->reg9_lockdown[master].i |= ways;
	}

}
