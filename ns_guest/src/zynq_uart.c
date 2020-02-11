/********************************* Copyright ********************************
 **
 ** Copyright (c) 2015,
 ** Aerospace Information Technology (AIT), Universitat Wurzburg
 ** Embedded Systems Research Group (ESRG), Universidade do Minho
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the Aerospace Information Technology and
 **   Embedded Systems Research Group, nor the names of its contributors
 **   may be used to endorse or promote products derived from this software
 **   without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/

/**
* @file zynq_uart.c
* @date 2015/10/03 01:00
* @author Sandro Pinto
*
* Copyright 2015 AIT & ESRG
*
* @brief Zynq UART driver
*/

#include"zynq_uart.h"


Uart_Zynq * const Ptr_Uart[NUM_UART] = {(Uart_Zynq *)UART_BASE_0,(Uart_Zynq *)UART_BASE_1};


/**********************************************************************************
 * UART disable
 * Disables the specified uart (by id)
 **********************************************************************************/
void uart_disable(uint8_t uart_id){

	Uart_Zynq * ptr_uart = Ptr_Uart[uart_id];

	uint32_t ctrl_reg = ptr_uart->control;
	ctrl_reg = ( UART_CONTROL_STPBRK | UART_CONTROL_TXDIS | UART_CONTROL_RXDIS );
	ptr_uart->control = ctrl_reg;

}

/**********************************************************************************
 * UART enable
 * Enables the specified uart (by id)
 **********************************************************************************/
void uart_enable(uint8_t uart_id){

	Uart_Zynq * ptr_uart = Ptr_Uart[uart_id];

	uint32_t ctrl_reg = ptr_uart->control;
	ctrl_reg = ( UART_CONTROL_STPBRK | UART_CONTROL_TXEN | UART_CONTROL_RXEN | UART_CONTROL_RXRES | UART_CONTROL_TXRES );
	ptr_uart->control = ctrl_reg;

}

/**********************************************************************************
 * UART baudrate set
 * Sets the baudrate of the specified uart (by id)
 **********************************************************************************/
uint32_t uart_set_baud_rate(uint8_t uart_id, uint32_t baud_rate){

	Uart_Zynq * ptr_uart = Ptr_Uart[uart_id];

	uint32_t sel_clk =  UART_FREQ_CLK;
	uint32_t baud_rate_calc = 0;
	uint8_t  bdiv = 0;
	uint16_t cd_calc = 0;
	uint16_t error_calc = 0;
	uint32_t exit_loop = 0;

	if(baud_rate == 0){

		baud_rate =  UART_BAUD_RATE;
	}

	/* baud_rate = sel_clk / (CD * (BDIV+1))
	 * baud_rate -> Baud Rate
	 * sel_clk -> Selected Clock
	 * CD -> Baud Rate Generator
	 * BDIV -> Baud Rate Divider
	 */
	for(bdiv = 4; bdiv < 255; bdiv++){

		cd_calc = sel_clk / ( baud_rate * ( bdiv + 1 ) );
		baud_rate_calc = sel_clk / ( cd_calc * ( bdiv + 1 ) );
		error_calc = ( baud_rate > baud_rate_calc ) ? ( baud_rate - baud_rate_calc ) : ( baud_rate_calc - baud_rate );
		if( (error_calc * 1000)/baud_rate < UART_MAX_ERROR ){ //when calculated error satisfies maximum error (10*%) stops calculation

			exit_loop = 1;
			break;
		}
	}

	if(!exit_loop){

		return 0; // ERROR! BaudRate not possible
	}

	/****** Configure the Baud Rate *******/
	/* Disable the Rx and Tx path */
	ptr_uart->control = (UART_CONTROL_RXDIS|UART_CONTROL_TXDIS);
	/* Write the calculated CD value */
	ptr_uart->br_gen = cd_calc;
	/* Write the calculated BDIV value */
	ptr_uart->br_div = bdiv;
	/* Reset Tx and Rx paths */
	ptr_uart->control = (UART_CONTROL_TXRES|UART_CONTROL_RXRES);
	/* Enable the Rx and Tx path */
	ptr_uart->control = (UART_CONTROL_TXEN|UART_CONTROL_RXEN);

	return 1;

}


/**********************************************************************************
 * UART Initialization
 * Initializes the specified uart (by id)
 **********************************************************************************/
uint32_t uart_init(uint8_t uart_id/*, uint32_t baud_rate*/) {

	Uart_Zynq * ptr_uart = Ptr_Uart[uart_id];

	uint32_t baud_rate =  UART_BAUD_RATE;
	uint32_t sel_clk =  UART_FREQ_CLK;
	uint32_t baud_rate_calc = 0;
	uint8_t  bdiv = 0;
	uint16_t cd_calc = 0;
	uint16_t error_calc = 0;
	uint32_t exit_loop = 0;

	/* baud_rate = sel_clk / (CD * (BDIV+1))
	 * baud_rate -> Baud Rate
	 * sel_clk -> Selected Clock
	 * CD -> Baud Rate Generator
	 * BDIV -> Baud Rate Divider
	 */
	for(bdiv = 4; bdiv < 255; bdiv++){

		cd_calc = sel_clk / ( baud_rate * ( bdiv + 1 ) );
		baud_rate_calc = sel_clk / ( cd_calc * ( bdiv + 1 ) );
		error_calc = ( baud_rate > baud_rate_calc ) ? ( baud_rate - baud_rate_calc ) : ( baud_rate_calc - baud_rate );
		if( (error_calc * 1000)/baud_rate < UART_MAX_ERROR ){ //when calculated error satisfies maximum error (10*%) stops calculation

			exit_loop = 1;
			break;
		}
	}

	if(!exit_loop){

		return 0; // ERROR! BaudRate not possible
	}

	/* Configure UART character frame */
	ptr_uart->mode = (UART_MODE_CHRL_REF|UART_MODE_CHRL_8|UART_MODE_PAR_NP|UART_MODE_NBSTOP_1|UART_MODE_CHMODE_N);

	/* Configure the Baud Rate */
	/* Disable the Rx and Tx path */
	ptr_uart->control = (UART_CONTROL_RXDIS|UART_CONTROL_TXDIS);

	/* Write the calculated CD value */
	ptr_uart->br_gen = cd_calc;

	/* Write the calculated BDIV value */
	ptr_uart->br_div = bdiv;

	/* Reset Tx and Rx paths */
	ptr_uart->control = (UART_CONTROL_TXRES|UART_CONTROL_RXRES);

	/* Enable the Rx and Tx path */
	ptr_uart->control = (UART_CONTROL_TXEN|UART_CONTROL_RXEN);

	/* Set the level of the RxFIFO trigger level */
	ptr_uart->rx_fifo_trig = UART_RX_TRIGGER_LVL;

	/* Program the Receiver Timeout Mechanism (Disabled) */
	ptr_uart->rx_timeout = UART_RX_TIMEOUT_DIS;

	/* Clear all the interrupts in Interrupt Status Register */
	ptr_uart->isr_status = 0xFFFFFFFF;

	/* Enable RxFIFO Trigger Interrupt */
	ptr_uart->isr_en = UART_ISR_EN_RTRIG;

	/* Enable the Controller */
	ptr_uart->control = (UART_CONTROL_STPBRK|UART_CONTROL_TXEN|UART_CONTROL_RXEN|UART_CONTROL_RXRES|UART_CONTROL_TXRES);

	return 1;

}


/**********************************************************************************
 * UART get character
 * Get a character from the specified uart (by id)
 **********************************************************************************/
uint32_t uart_getc(uint8_t uart_id){

	Uart_Zynq * ptr_uart = Ptr_Uart[uart_id];
	uint32_t data = 0;

	//wait until RxFIFO is filled up to the trigger level
  while(!(char)(ptr_uart->ch_status & UART_CH_STATUS_RTRIG)){}

	data = ptr_uart->tx_rx_fifo;

	return data;

}


/**********************************************************************************
 * UART send character
 * Send a character through the specified uart (by id)
 **********************************************************************************/
void uart_putc(uint8_t uart_id,int8_t c){

	Uart_Zynq * ptr_uart = Ptr_Uart[uart_id];

	//wait until txFIFO is not full
	while(ptr_uart->ch_status & UART_CH_STATUS_TFUL);

	ptr_uart->tx_rx_fifo = c;

}


/**********************************************************************************
 * UART send string
 * Send a character through the specified uart (by id)
 * @param    uart_id - ID uart, s - String to be sent by the uart
 * @retval   No return value
 **********************************************************************************/
void uart_puts(uint8_t uart_id,const int8_t *s){

	 while (*s){

		uart_putc(uart_id,*s++);
	}

}
