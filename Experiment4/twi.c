#include "twi.h"
    
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