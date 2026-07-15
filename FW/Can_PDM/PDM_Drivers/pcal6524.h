/*
 * pcal6524.h
 *
 *  Created on: 17 giu 2026
 *      Author: Reatn
 */

#ifndef INC_PCAL6524_H_
#define INC_PCAL6524_H_

#include <stdint.h>
#include <stdbool.h>

/*
 * ============================================================================
 * I2C ADDRESS
 * ============================================================================
 */

#define PCAL6524_ADDR             (0x22 << 1)

/*
 * ============================================================================
 * REGISTERS
 * ============================================================================
 */

#define PCAL6524_REG_OUTPUT0      0x04
#define PCAL6524_REG_OUTPUT1      0x05
#define PCAL6524_REG_OUTPUT2      0x06

#define PCAL6524_REG_CONFIG0      0x0C
#define PCAL6524_REG_CONFIG1      0x0D
#define PCAL6524_REG_CONFIG2      0x0E

/*
 * ============================================================================
 * FUNCTIONS
 * ============================================================================
 */

bool PCAL6524_Init(void);
bool PCAL6524_WriteRegister(uint8_t reg, uint8_t value);
bool PCAL6524_WritePin(uint8_t port, uint8_t pin, bool state);

#endif
