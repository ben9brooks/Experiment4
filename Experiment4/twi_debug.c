/*
 * twi_debug.c
 *
 * Created: 11/2/2023 10:32:29 AM
 *  Author: Ben
 */ 
#include "twi.h"
#include "twi_debug.h"
#include "GPIO_Outputs.h"
#include <util/delay.h>

uint8_t initialize_sta013()
{
	//hold PB1 low for at least 100ns:
	GPIO_output_init(PB, (1<<1));
	GPIO_output_clear(PB, (1<<1));
	_delay_ms(.01);//this is like 1000ns or something
	GPIO_output_set(PB, (1<<1));
	
	char * prnt_bffr;
	prnt_bffr=export_print_buffer();
	uint8_t array[3] = {0,0,0};
	uint8_t error;
	uint8_t timeout = 100;
	do 
	{
		error = TWI_master_receive(TWI1_Base, 0x43, 0, 0, 3, array);
		timeout--;
	} while ((error >= 1) && (timeout != 0)); //while there is an error (>= 1) and timeout isn't done yet
	sprintf(prnt_bffr, "Received Value = %2.2bX\n\r", array[2]);
	UART_transmit_string(UART1, 0, prnt_bffr);
	
}