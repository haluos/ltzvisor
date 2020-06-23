#include <FreeRTOS_s_isr.h>
#include <gic.h>

FreeRTOS_SHandler FreeRTOS_FIQTable[NO_OF_INTERRUPTS_IMPLEMENTED];

void FreeRTOS_InitFIQTable ( void )
{
  int i;
  for (i = 0; i < NO_OF_INTERRUPTS_IMPLEMENTED; i++)
  {
    FreeRTOS_FIQTable[i] = NULL;
  }
}

void FreeRTOS_RegisterHandler (uint32_t interruptID, FreeRTOS_SHandler Handler)
{
  FreeRTOS_FIQTable[interruptID] = Handler;
}

void FreeRTOS_CallSHandler (uint32_t interruptID)
{
  if(FreeRTOS_FIQTable[interruptID] != NULL)
  {
    FreeRTOS_FIQTable[interruptID]();
  }
}
