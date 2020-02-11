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
* @file platform_zynq.h
* @date 2015/10/03 02:00
* @author Sandro Pinto
*
* Copyright 2015 AIT & ESRG
*
* @brief Zynq plataform registers definition
*/


#ifndef __PLATFORM_ZYNQ_
#define __PLATFORM_ZYNQ_

/************************** CLOCK *************************/
//#define CLOCK_FREQUENCY			800000000		// 800MHz
#define CLOCK_FREQUENCY			600000000		// 600MHz


/********************* ZYNQ_TTC_TIMER *********************/
#define TTC0_BASE			(0xF8001000)	// Triple Timer Counter 0 Base Register
#define TTC1_BASE			(0xF8002000)	// Triple Timer Counter 0 Base Register


/************************ ZYNQ_UART ***********************/
#define UART_BASE_0				0xE0000000 		//uart0
#define UART_BASE_1				0xE0001000 		//uart1


/************************** XSCU **************************/
#define	XSCU_BASE			0xF8F00000			// Application Processing Unit (mpcore) - Configuration/Peripheral Base Address Register
	#define SCU_CTR_OFFSET		0x00000000			// SCU Control Register Offset
	#define SCU_CONFIG_OFFSET	0x00000004			// SCU Configuration Register Offset
	// ...
	#define ICCPMR_OFFSET		0x00000104			// Interrupt Priority Mask Register


/*************************** GIC **************************/
#define MPIC_BASE			(XSCU_BASE + 0x0100)	// CPU Interrupt Interface Base Register
#define MPID_BASE			(XSCU_BASE + 0x1000)	// Processor Interrupt Distributor Base Register
#define MPTW_BASE			(XSCU_BASE + 0x0600)	// Private Timer Base Register (Load Register)


#endif /* __PLATFORM_ZYNQ_ */
