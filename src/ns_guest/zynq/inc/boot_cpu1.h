/*******************************************************************************
|                            LTZVisor SMP library code                         |
********************************************************************************
|@author:   Magdalena Halusek                                                  |
|@date:     04/09/2020                                                         |
|@version:  v1.0 (initial)                                                     |
|@changes:  -04/09/2020: implemented function that triggers booting of CPU1    |
|______________________________________________________________________________|

~This file contains macro implemented for NS Guest running in SMP mode~

*/

#ifndef BOOT_CPU1_H
#define BOOT_CPU1_H

/*
* Macro used from NS Guest to generate request to LTZVisor for booting CPU1:   *
*                                                                              *
* ~0x0ffffffb is identifier of request (set in ltzvisor_syscall_asm.h file)~   *
* ~CPU1 needs to boot into Monitor mode, so that LTZVisor on CPU1 can set      *
* initial context of CPU1~                                                     *
*                                                                              *
*/
#define boot_CPU1()\
  asm volatile ("ldr r0, =0x0ffffffb");\
  asm volatile ("smc #0")\

#endif
