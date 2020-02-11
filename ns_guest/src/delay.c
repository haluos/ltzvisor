#include <delay.h>

extern uint8_t ttc_done;

void delay_init (void)
{
	ttc_done = 0;
	ttc_init(TTC1,TTCx_1,INTERVAL);

	interrupt_enable(TTC1_TTCx_1_INTERRUPT,TRUE);
	interrupt_target_set(TTC1_TTCx_1_INTERRUPT,0,1);
	interrupt_priority_set(TTC1_TTCx_1_INTERRUPT, 9 << 4);
}

void delayus (uint32_t useconds)
{
	ttc_request(TTC1, TTCx_1, useconds);
	ttc_enable(TTC1, TTCx_1);
	while(!ttc_done);
	ttc_done = 0;
	ttc_disable(TTC1, TTCx_1);
}