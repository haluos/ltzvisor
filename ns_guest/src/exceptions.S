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
* @file exceptions.S
* @date 2015/09/23 17:00
* @author Sandro Pinto
*
* Copyright 2015 AIT & ESRG
*
* @brief ARMv7-a (Cortex-A9) exceptions implementation
*/




.set IRQ_ACKNOWLEDGE_,	(0x0C)	// Interrupt Acknowledge Register (R)


.global vectNum	 @ Variable that contain the irq number. Used in _ISR_hanlder


.code 32
.section ".exceptions", "ax"
/**********************************************************************************
 * IRQ handler
 * Handler that deals with IRQs
 **********************************************************************************/
.global irq_handler
.func irq_handler
irq_handler:
	b	irq_handler
.endfunc	@ irq_handler


/**********************************************************************************
 * FIQ handler
 * Handler that deals with FIQs
 **********************************************************************************/
.global fiq_handler
.func fiq_handler
fiq_handler:

	/********* TODO *********/
	b	fiq_handler

.endfunc	@ fiq_handler


/**********************************************************************************
 * SWI handler
 * Handler that deals with SWI requests
 **********************************************************************************/
.global swi_handler
.func swi_handler
swi_handler:

	/********* TODO *********/
	b	swi_handler
	
.endfunc	@ swi_handler


/**********************************************************************************
 * Reserved handler
 * Handler that deals with an unexpected exception in the reserved position on VT
 **********************************************************************************/
.global reserved_handler
.func reserved_handler
reserved_handler:

	/********* TODO *********/
	b	reserved_handler

.endfunc	@ reserved_handler



/**********************************************************************************
 * Common code for all abort exceptions
 * Helps the data abort, prefetch abort and undefined handler 
 **********************************************************************************/
.global  commonAbort
.func  commonAbort
commonAbort:

	/********* TODO *********/
	b	.

.endfunc	@ commonAbort


/**********************************************************************************
 * Data Abort handler
 * Handler that deals with data aborts
 **********************************************************************************/
.global dataAbort_handler
.func dataAbort_handler
dataAbort_handler:

	/********* TODO *********/
	b	dataAbort_handler
	b       commonAbort

.endfunc	@ dataAbort_handler


/**********************************************************************************
 * Prefetch Abort handler
 * Handler that deals with prefetch aborts
 **********************************************************************************/
.global prefetchAbort_handler
.func prefetchAbort_handler
prefetchAbort_handler:

	/********* TODO *********/
	b	prefetchAbort_handler
    	b       commonAbort

.endfunc	@ prefetchAbort_handler


/**********************************************************************************
 * Undefined handler
 * Handler that deals with undefined instructions
 **********************************************************************************/
.global undefined_handler
.func undefined_handler
undefined_handler:

	/********* TODO *********/
	b	undefined_handler
    	b       commonAbort

.endfunc	@ undefined_handler






.end
