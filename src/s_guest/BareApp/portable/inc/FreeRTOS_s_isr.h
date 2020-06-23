#ifndef FREERTOS_S_ISR_H
#define FREERTOS_S_ISR_H

#include <types.h>

typedef void (* FreeRTOS_SHandler) (void);

void FreeRTOS_InitFIQTable ( void );

void FreeRTOS_RegisterHandler (uint32_t interruptID, FreeRTOS_SHandler Handler);

void FreeRTOS_CallSHandler (uint32_t interruptID);

#endif
