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
#include <zynq_spi.h>
#include <hw_zynq.h>
#include <printk.h>
#include <zynq_uart.h>
#include "types.h"
#include <board.h>

// extern uint32_t GPOS0_start;
extern struct nsguest_conf_entry nsguest_config;

// #define sensor_data (*(volatile int32_t *)(GPOS0_start + 0x61A80))
// volatile int32_t sensor_data __attribute__((section (".shared_location")));

void test(void);

int main() {
    // int32_t last_read = sensor_data;
    volatile int32_t *sensor_data = (volatile uint32_t *)(0x100000+0x61A80);
    int32_t read;
    int i = 0;

    printk("Secure guest running, address %x\n", sensor_data );
    /** Initialize hardware */
    hw_init();

    /** Generate tick every 1s */

    static uint32_t toggle;
    /** 4GPIO (LED) in FPGA fabric */
    static uint32_t *ptr = (uint32_t *) 0x41200000;

    tick_set(1000000); // ps is 11

    // reset controller
    SPI_1_Reset();

    // enable clock for controller
    SPI_1_ClockEnable();

    // connect controller with MIO pins
    SPI_1_SignalRoute();

    // configure the SPI controller (clocks, registers etc..)
    SPI_1_Config();

    // enable the controller
    SPI_1_Enable();
    uint8_t a = 0xbc;
    uint8_t msb;
    uint32_t temp = 0;
    uint8_t lsb = 0;

    while(1){
      i++;
      SPI1_SendData(a);
      SPI1_ReadData(&lsb);
      // printk("lsb : %x\n", lsb);

      //
      // SPI1_SendData(a);
      // SPI1_ReadData(&msb);
      // printk("msb : %x\n", msb);



      // temp = ((uint32_t)msb << 8) | (uint32_t)lsb;
      // int f = (temp*100/1023)*3.3;
      // printk("primio : %d\n", f);
      // if(last_read != *sensor_data)
      // {
      // read = i++;
      // memcpy(&read, sensor_data, sizeof(read));
      if(temp || !temp)
      {
        memcpy(sensor_data, &lsb, sizeof(lsb));
        __asm("dsb");
        printk("value %d\n", *sensor_data);
      }
        // *sensor_data = i++;
      // }
      // test();
      YIELD();
    }

}


void test(void) {

  uint8_t a = 0xbc;
  uint8_t recv;
  int i;

  // uart_flag = 1;
  // uart_putc(0,a);
  // printk("%c",a );

  // while(!  SPI1_ReadData(&recv));
  // recv = (uint8_t)uart_getc(0);

  // printk("%s\n", recv );

  SPI1_SendData(a);
  SPI1_ReadData(&recv);

  // printk("primio : %c\n", recv);

  YIELD();

}