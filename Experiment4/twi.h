#ifndef TWI_H_
#define TWI_H_ 
#include "board.h"

typedef struct TWI
{
    volatile uint8_t TWBR;
    volatile uint8_t TWSR;
    volatile uint8_t TWAR;
    volatile uint8_t TWDR;
    volatile uint8_t TWCR;
    volatile uint8_t TWAMR;
} TWI_t;

#define TWI0_Base ((volatile TWI_t *) 0xB8)
#define TWI1_Base ((volatile TWI_t *) 0xD8)

uint8_t TWI_master_init(volatile TWI_t *TWI_addr, uint32_t I2C_freq);


typedef enum 
{
    TWI_OK = 0,
    TWI_ERROR = 1 

}TWI_ERROR_CODES;

#endif 