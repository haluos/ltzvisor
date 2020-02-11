#ifndef DELAY_H
#define DELAY_H

#include <xil_types.h>
#include <zynq_ttc.h>

void delay_init (void);
void delayus (uint32_t useconds);

#endif