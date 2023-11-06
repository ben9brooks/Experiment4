/*
 * main.c
 *
 * Created: 10/5/2021 3:06:09 PM
 * Author : grogu
 */ 

#include <avr/io.h>
#include "board.h"
#include "GPIO_Outputs.h"
#include "LEDS.h"
#include "UART.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "UART_Print.h"
#include "print_memory.h"
#include "Long_Serial_In.h"
#include <stdio.h>

#include "twi.h"
#include "twi_debug.h"


const char test_string[15] PROGMEM = "Hello World!\n\r";

int main(void)
{
	uint8_t error = 0;
	uint32_t i2c_freq = 100000; /* 100k */ 
	uint8_t bkpt = 0;
	uint8_t array[3] = {0,0,0};
	uint8_t timer = 10;
	
	UART_init(UART1, 9600); /* baud = 9600 */ 
	UART_transmit_string(UART1, "\nSTART\n", 7);

	/******************
	*
	* Step 1 - init
	*
	******************/

	error = TWI_master_init(TWI1_Base, i2c_freq); 
	if (error != 0)
	{
		UART_transmit_string(UART1, "twi_init_fail\n", 14);
	}

	/******************
	*
	* Step 2
	*
	******************/

	// do 
	// {
	// 	error = TWI_master_receive(TWI1_Base, 0x43, 0, 0, 3, array);
	// 	timer--;
	// } while ((error > 0) && (timer>0));
	//printError(error);
	
	/******************
	*
	* Step 3 - Debug
	*
	******************/
	
	//error = initialize_sta013();
	if (error != 0)
	{
		UART_transmit_string(UART1, "sta_timeout\n", 12);
	}
	
	_delay_ms(1000);
	
	/************
	*
	* Step 4/5
	*
	************/
	
	step5();
	






	
	UART_transmit_string(UART1, "\nEND\n", 5);

	

	while (1)
	{
		
		
	}
}


