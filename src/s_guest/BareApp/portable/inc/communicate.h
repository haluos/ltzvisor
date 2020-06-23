#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#define CPU0_MESSAGE_INT         9
#define CPU1_MESSAGE_INT         10

#include <types.h>

void sgi_communicate_handler (void);
void cpu1_sgi_communicate_handler (void);

#endif
