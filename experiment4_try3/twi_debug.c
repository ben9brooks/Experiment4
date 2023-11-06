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
	_delay_us(.1);//this is 100ns, might mess up tho
	GPIO_output_set(PB, (1<<1));
	
	char * prnt_bffr;
	prnt_bffr=export_print_buffer();
	uint8_t array[3] = {0,0,0};
	uint8_t error;
	uint16_t timeout = 100;
	do 
	{
		error = TWI_master_receive(TWI1_Base, 0x43, 0, 0, 3, array);
		timeout--;
	} while ((error >= 1) && (timeout != 0)); //while there is an error (>= 1) and timeout isn't done yet
	if(timeout == 0)
	{
		
		
		
		return TWI_ERROR_TIMEOUT;
	}
	
	sprintf(prnt_bffr, "Received Value = %2.2bX\n\r", array[2]);
	UART_transmit_string(UART1, 0, prnt_bffr);
	
	return 0;
	
}

void printError(uint8_t err)
{
	switch(err)
	{
		case 0:
			UART_transmit_string(UART1, "TWI_OK\n", 7);
			break;
		case 1:
			UART_transmit_string(UART1, "TWI_ERR\n", 8);
			break;
		case 2:
			UART_transmit_string(UART1, "TWI_ERR_TWO\n", 12);
			break;
		case 3:
			UART_transmit_string(UART1, "TWI_ERR_TIME\n", 13);
			break;
		case 4:
			UART_transmit_string(UART1, "TWI_ERR_BUS\n", 12);
			break;
		case 5:
			UART_transmit_string(UART1, "TWI_ERR_NACK\n", 13);
			break;
		default:
			UART_transmit_string(UART1, "TWI_OTHER\n", 10);
	}
}