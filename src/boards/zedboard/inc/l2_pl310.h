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

#ifndef __L2_PL310_H__
#define __L2_PL310_H__

#include <types.h>
#include <io.h>
#include <platform_zynq.h>
#include <board.h>

#define PL310_RESERVED_BEFORE_CONTROL       ((0x100 - 0x004)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_EVENT         ((0x200 - 0x10C)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_SYNC          ((0x730 - 0x220)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_INVPA         ((0x770 - 0x730)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_INVWAY        ((0x77C - 0x770)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_CLEANPA       ((0x7B0 - 0x77C)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_CLEANINDEX    ((0x7B8 - 0x7B0)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_CLEANINVPA    ((0x7F0 - 0x7BC)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_CLEANINVINDEX ((0x7F8 - 0x7F0)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_LOCKDOWN      ((0x900 - 0x7FC)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_LOCKLINE      ((0x950 - 0x93C)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_ADDRFILT      ((0xC00 - 0x954)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_DEBUG         ((0xF40 - 0xC04)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_PREFETCH      ((0xF60 - 0xF40)/sizeof(uint32_t) - 1)
#define PL310_RESERVED_BEFORE_POWER         ((0xF80 - 0xF60)/sizeof(uint32_t) - 1)

#define NUM_MASTERS 8

typedef struct {
    uint32_t d;
    uint32_t i;
} lockdown_t;

typedef struct {



    volatile uint32_t reg0_cache_id;
    volatile uint32_t reg0_cache_type;
    const uint32_t reserved0[PL310_RESERVED_BEFORE_CONTROL];
    volatile uint32_t reg1_control;
    volatile uint32_t reg1_aux_control;
    volatile uint32_t reg1_tag_ram_control;
    volatile uint32_t reg1_data_ram_control;
    const uint32_t reserved1[PL310_RESERVED_BEFORE_EVENT];
    volatile uint32_t reg2_ev_counter_ctrl;
    volatile uint32_t reg2_ev_counter1_cfg;
    volatile uint32_t reg2_ev_counter0_cfg;
    volatile uint32_t reg2_ev_counter1 ;
    volatile uint32_t reg2_ev_counter0 ;
    volatile uint32_t reg2_int_mask;
    volatile uint32_t reg2_int_mask_status ;
    volatile uint32_t reg2_int_raw_status;
    volatile uint32_t reg2_int_clear;
    const uint32_t reserved2[PL310_RESERVED_BEFORE_SYNC];
    volatile uint32_t reg7_cache_sync;
    const uint32_t reserved3[PL310_RESERVED_BEFORE_INVPA];
    volatile uint32_t reg7_inv_pa;
    const uint32_t reserved4[PL310_RESERVED_BEFORE_INVWAY];
    volatile uint32_t reg7_inv_way;
    const uint32_t reserved5[PL310_RESERVED_BEFORE_CLEANPA];
    volatile uint32_t reg7_clean_pa;
    const uint32_t reserved6[PL310_RESERVED_BEFORE_CLEANINDEX];
    volatile uint32_t reg7_clean_index;
    volatile uint32_t reg7_clean_way;
    const uint32_t reserved7[PL310_RESERVED_BEFORE_CLEANINVPA];
    volatile uint32_t reg7_clean_inv_pa;
    const uint32_t reserved8[PL310_RESERVED_BEFORE_CLEANINVINDEX];
    volatile uint32_t reg7_clean_inv_index;
    volatile uint32_t reg7_clean_inv_way;
    const uint32_t reserved9[PL310_RESERVED_BEFORE_LOCKDOWN];
    volatile lockdown_t reg9_lockdown[NUM_MASTERS];
    const uint32_t reserved10[PL310_RESERVED_BEFORE_LOCKLINE];
    volatile uint32_t reg9_lock_line_en;
    volatile uint32_t reg9_unlock_way;
    const uint32_t reserved11[PL310_RESERVED_BEFORE_ADDRFILT];
    volatile uint32_t reg12_addr_filtering_star;
    volatile uint32_t reg12_addr_filtering_end ;
    const uint32_t reserved12[PL310_RESERVED_BEFORE_DEBUG];
    volatile uint32_t reg15_debug_ctrl;
    const uint32_t reserved13[PL310_RESERVED_BEFORE_PREFETCH];
    volatile uint32_t reg15_prefetch_ctrl ;
    const uint32_t reserved14[PL310_RESERVED_BEFORE_POWER];
    volatile uint32_t reg15_power_ctrl;

} CacheL2pl310;

void cachel2_init();

void cachel2_enable();
void cachel2_disable();

void cachel2_invalidate();
void cachel2_clean();

void cachel2_lockdown(uint32_t way_mask, uint32_t master_mask);

#endif
