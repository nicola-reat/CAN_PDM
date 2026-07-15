#include "pcal6524.h"
#include "i2c.h"

static uint8_t outputPort0 = 0;
static uint8_t outputPort1 = 0;
static uint8_t outputPort2 = 0;

bool PCAL6524_WriteRegister(uint8_t reg, uint8_t value)
{
    return (HAL_I2C_Mem_Write(&hi2c1,
                              PCAL6524_ADDR,
                              reg,
                              I2C_MEMADD_SIZE_8BIT,
                              &value,
                              1,
                              100) == HAL_OK);
}

bool PCAL6524_Init(void)
{
    bool status = true;

    status &= PCAL6524_WriteRegister(PCAL6524_REG_CONFIG0, 0x00);
    status &= PCAL6524_WriteRegister(PCAL6524_REG_CONFIG1, 0x00);
    status &= PCAL6524_WriteRegister(PCAL6524_REG_CONFIG2, 0x00);

    status &= PCAL6524_WriteRegister(PCAL6524_REG_OUTPUT0, 0x00);
    status &= PCAL6524_WriteRegister(PCAL6524_REG_OUTPUT1, 0x00);
    status &= PCAL6524_WriteRegister(PCAL6524_REG_OUTPUT2, 0x00);

    outputPort0 = 0;
    outputPort1 = 0;
    outputPort2 = 0;

    return status;
}

bool PCAL6524_WritePin(uint8_t port, uint8_t pin, bool state)
{
    uint8_t *shadow;
    uint8_t reg;

    switch(port)
    {
        case 0:
            shadow = &outputPort0;
            reg = PCAL6524_REG_OUTPUT0;
            break;

        case 1:
            shadow = &outputPort1;
            reg = PCAL6524_REG_OUTPUT1;
            break;

        case 2:
            shadow = &outputPort2;
            reg = PCAL6524_REG_OUTPUT2;
            break;

        default:
            return false;
    }

    if(state)
    {
        *shadow |= (1U << pin);
    }
    else
    {
        *shadow &= ~(1U << pin);
    }

    return PCAL6524_WriteRegister(reg, *shadow);
}
