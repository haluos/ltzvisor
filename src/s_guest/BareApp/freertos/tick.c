#include "FreeRTOS.h"
#include "task.h"

/* Xilinx includes. */
// #include "xscutimer.h"
#include <gic.h>
#include <zynq_ttc.h>
#include "printk.h"
// #include "xscugic.h"

#define TICK_TIMER_INTR		TTC0_TTCx_2_INTERRUPT

uint8_t ttc_done;

void vApplicationFIQHandler( uint32_t ulICCIAR );

// XScuGic xInterruptController; 	/* Interrupt controller instance */

void vApplicationFIQHandler( uint32_t ulICCIAR )
{
// extern const XScuGic_Config XScuGic_ConfigTable[];
extern void FreeRTOS_Tick_Handler (void);
// extern void XEmacPs_IntrHandler (void);
// static const XScuGic_VectorTableEntry *pxVectorTable = XScuGic_ConfigTable[ XPAR_SCUGIC_SINGLE_DEVICE_ID ].HandlerTable;
uint32_t ulInterruptID;
// const XScuGic_VectorTableEntry *pxVectorEntry;

	/* The ID of the interrupt is obtained by bitwise anding the ICCIAR value
	with 0x3FF. */
	ulInterruptID = ulICCIAR & 0x3FFUL;
	if( ulInterruptID < NO_OF_INTERRUPTS_IMPLEMENTED )
	{
		// xil_printf("%d\n", ulInterruptID);
		if (ulInterruptID == TICK_TIMER_INTR)
		{
			FreeRTOS_Tick_Handler();
		}
		// else
		// {
		// 	/* Call the function installed in the array of installed handler functions. */
		// 	pxVectorEntry = &( pxVectorTable[ ulInterruptID ] );
		// 	pxVectorEntry->Handler( pxVectorEntry->CallBackRef );
		// }
	}
}

void FreeRTOS_SetupTTCTickInterrupt( void )
{
	ttc_init(TTC0,TTCx_2,INTERVAL);

	interrupt_enable(TTC0_TTCx_2_INTERRUPT,TRUE);
	interrupt_target_set(TTC0_TTCx_2_INTERRUPT,0,1);
	interrupt_priority_set(TTC0_TTCx_2_INTERRUPT, 6);

	FreeRTOS_ClearTTCTickInterrupt();
	/* Set tick every 10 ms */
	ttc_request(TTC0, TTCx_2, 10000);
	ttc_enable(TTC0, TTCx_2);
}

void FreeRTOS_ClearTTCTickInterrupt( void )
{
	ttc_interrupt_clear(TTC0_TTCx_2_INTERRUPT);
}
