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
* @file vectors.S
* @date 2015/09/23 10:00
* @author Sandro Pinto
*
* Copyright 2015 AIT & ESRG
*
* @brief ARMv7-a (Cortex-A9) vectors definition
*/


.global _startp
.global undefined_handler
.global swi_handler
.global prefetchAbort_handler
.global dataAbort_handler
.global reserved_handler
.global irq_handler
.global fiq_handler

.code 32

/**********************************************************************************
 * Vector table
 * This table will be copied into the RAM vector table by the startup code.
 **********************************************************************************/
.section ".vectors", "ax"		/* ax - allocatable, executable */
.global vectors
vectors:
	ldr     pc, _reset_addr
	ldr     pc, _undefined_addr
	ldr     pc, _svc_addr
	ldr     pc, _prefetchAbort_addr
	ldr     pc, _dataAbort_addr
	ldr     pc, _reserved_addr 	/* reserved for HYP mode - not supported */
	ldr     pc, _irq_addr
	ldr     pc, _fiq_addr

_reset_addr:
    .word     _startp
_undefined_addr:
    .word     undefined_handler
_svc_addr:
    .word     swi_handler
_prefetchAbort_addr:
    .word     prefetchAbort_handler
_dataAbort_addr:
    .word     dataAbort_handler
_reserved_addr:
    .word     reserved_handler
_irq_addr:
    .word     irq_handler
_fiq_addr:
    .word     fiq_handler

.end
