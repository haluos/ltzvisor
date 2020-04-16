/*******************************************************************************
|                            LTZVisor SMP library code                         |
********************************************************************************
|@author:   Magdalena Halusek                                                  |
|@date:     04/16/2020                                                         |
|@version:  v1.0 (initial)                                                     |
|@changes:  -04/16/2020: file creation: added neccessery declarations          |
|______________________________________________________________________________|

  ~ This file contains daclarations used for initialization of CPU1 NS Guest in
    SMP mode.

*******************************************************************************/

#ifndef LTZVISOR_CPU1_API_H
#define LTZVISOR_CPU1_API_H

#include "ltzvisor.h"
#include "string.h"

/*
  @function_name:   ltzvisor_cpu1_nsguest_create
  @params:          none
  @retval:          none
  @description:     This function is part of CPU1 LTZVisor. It is used for
                    creating initial CPU1 NS Guest context.
*/
void ltzvisor_cpu1_nsguest_create (void);

void cpu1_monitor_boot_nsguest(tzmachine* NS_CPU1_Guest);

#endif
