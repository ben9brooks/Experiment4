#include "twi.h"

void stopCond(volatile TWI_t* TWI_addr)
{
	TWI_addr->TWCR = ((1<<TWINT) | (0<<TWEA) | (1<<TWEN));
}

void fullStopCond(volatile TWI_t* TWI_addr)
{
	TWI_addr->TWCR = ((1<<TWINT) | (1<<TWSTO) | (1<<TWEN));
}

void startCond(volatile TWI_t* TWI_addr)
{
	TWI_addr->TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));	
}

void restartCond(volatile TWI_t* TWI_addr)
{
	TWI_addr->TWCR = ((1<<TWINT) | (1<<TWEA) | (1<<TWEN));	
}
    
uint8_t TWI_master_init(volatile TWI_t *TWI_addr, uint32_t I2C_freq)
{
    TWI_ERROR_CODES error = TWI_OK; 
    uint8_t twps_val;
    uint32_t prescale = (((F_CPU/OSC_DIV)/I2C_freq)-16UL)/(2UL*255);
    if (prescale < 1) {
        twps_val = 0x00;
    } else if (prescale < 4) {
        twps_val = 0x01;
    } else if (prescale < 16) {
        twps_val = 0x02;
    } else if (prescale < 64) {
        twps_val = 0x03;
    } else {
        return TWI_ERROR; // fail
    }

    TWI_addr->TWSR = twps_val;

    uint16_t TWBR=(((F_CPU/OSC_DIV)/I2C_freq)-16UL)/(2UL*prescale);
    if (TWBR >= 256)
    {
        return TWI_ERROR; // fail
    }

    TWI_addr->TWBR = TWBR;
    
    return 0;
}

uint8_t TWI_master_receive(volatile TWI_t *TWI_addr, uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint16_t num_bytes, uint8_t* arr)
{
	//general layout for a receive: START condition, Device Address, receive an ACK, then receive data byte(s) with an ACK after each one, except the last is a NACK, plus a STOP condition.
	uint8_t status;
	uint8_t temp8;
	uint8_t send_value;
	uint8_t rcvd_arr[10];
	uint8_t index;
	//internal address is optional and a bonus, along with int_addr_sz
	
	
	//send device address with a 1 in LSB (SLA+R). LSB being a 1 means read, see lecture
	send_value = (device_addr<<1) | 0x01;
	
	//create start condition (writes to TWCR, TWINT set, TWSTA start cond set, write 1 to TWEN To enable TWI 
	startCond(TWI_addr);
	
	//wait for TWINT (bit 7) to be set in TWCR
	do 
	{
		status = TWI_addr->TWCR;
	} while ((status&0x80)==0);
	
	//read status
	temp8 = ((TWI_addr->TWCR)&0xF8); //clear lower 3 bits
	
	//if start sent, then send SLA+R (temp8/status can be start or repeated start condition)
	if((temp8 == TWSR_START_Cond) || (temp8 == TWSR_START_Cond_repeat))
	{
		TWI_addr->TWDR = send_value;
		TWI_addr->TWCR = ((1<<TWINT) | (1<<TWEN));
	}
	//!! can check for other errors here...
	if(1 == 2)
	{
		return TWI_ERROR; //fixme
	}
	
	//read status
	temp8 = TWI_addr->TWCR;
	
	//receive ACK From slave (write 1 to TWEA, bit 6 of TWCR, when ACK should be sent after receiving data from slave)
	if(temp8 == TWSR_ACK_rcvd) //SLA+R sent, ACK received
	{
		//be prepped to send stop cond if only 1 bit received
		// if 1 byte received, send NACK to slave ( write 0 to TWEA)
		if(num_bytes == 1)
		{
			stopCond(TWI_addr);
		}
		// if >1 byte received, send ACK after all but the last byte.
		else
		{
			restartCond(TWI_addr);
		}

		//send all data bytes until all bytes sent or error
		index = 0;
		while((num_bytes != 0) && (1!=2)) //put error check here..
		{
			//wait for TWINT to be set
			do 
			{
				status = TWI_addr->TWCR;
			} while ((status&0x80)==0);

			//read status
			temp8 = ((TWI_addr->TWSR)&0xF8); // clear lower 3 bits
			
			//i think this is nested inside this loop?
			if(temp8 == TWSR_ACK_rtrnd) //data byte received, ack sent back
			{
				num_bytes--;
				rcvd_arr[index] = TWI_addr->TWDR;
				index++;
				if(num_bytes == 1)
				{
					stopCond(TWI_addr);
				}
				else
				{
					restartCond(TWI_addr);
				}
			}
			else if (temp8 == TWSR_NACK_rtrnd)
			{
				//save byte to array, dec num_bytes
				num_bytes--;
				rcvd_arr[index] = TWI_addr->TWDR;
				
				//write 1 to TWSTO (bit 4) to request stop condition
				fullStopCond(TWI_addr);
				
				//wait for twsto to return to 0
				do 
				{
					status = TWI_addr->TWCR;
				} while ((status&0x10) != 0);
				
			}
			
		}
	}
	else //NACK at the start is not expected, we didn't get to receive anything.
	{
		if (temp8 == TWSR_NACK_rcvd)
		{
			return TWI_ERROR; //fixme, maybe a bus_busy error?
		}
	}
	
}