#include <communicate.h>
#include <printk.h>

extern uint32_t toggle;
uint8_t toggle_mode;

uint32_t message_address, messages;

void sgi_communicate_handler (void)
{
	uint32_t led;
	uint32_t *message = (uint32_t*) 0x2010000;
	// uint32_t *message = (uint32_t *) message_address;
	led = *message & 0xf;
	toggle_mode = 0;
	if(led == 0x0)
	{
		toggle = 0x00;
	}
	else if(led == 0xf)
	{
		toggle = 0xff;
	}
	else if((led > 0) && (led < 9))
	{
		toggle = 0x1 << (led-1);
	}
	else
	{
		toggle_mode = 1;
		toggle = 0x00;
	}
	// if (messages > 1)
	// {
	// 	int i;
	// 	for(i = 0; i < messages; i++)
	// 	{
	// 		message++;
	// 		printk("CPU0 message %d: %d\n", i+1, *message);
	// 	}
	// 	messages = 1;
	// }
	// printk("Message from NS in SGI: 0x%x\n", *message);
}

void cpu1_sgi_communicate_handler (void)
{
	uint32_t led;
	uint32_t *message = (uint32_t*) 0x2010000;
	// uint32_t *message = (uint32_t *) message_address;
	led = *message & 0xf;
	toggle_mode = 0;
	if(led == 0x0)
	{
		toggle = 0x00;
	}
	else if(led == 0xf)
	{
		toggle = 0xff;
	}
	else if((led > 0) && (led < 9))
	{
		toggle = (0x1 << (led-1)) ^ 0xff;
	}
	else
	{
		toggle_mode = 1;
		toggle = 0x00;
	}
	// if (messages > 1)
	// {
	// 	int i;
	// 	for(i = 0; i < messages; i++)
	// 	{
	// 		message++;
	// 		printk("CPU1 message %d: %d\n", i+1, *message);
	// 	}
	// 	messages = 1;
	// }
	// printk("Message from CPU1 NS in SGI: 0x%x\n", *message);
}
