/*
 * PCAL6524.c
 *
 *  Created on: Jul 1, 2025
 *      Author: nicola.reat
 */


#include "pcal6524.h"

// Create and initialize a PCAL6524 I/O expander instance
// - Uses the I2C1 peripheral for communication
// - Device address is 0x22 (ADDR pin tied to GND), shifted left by 1 to match HAL format (0x44)
PCAL6524 expander = {
    .hi2c = &hi2c1,
    .i2c_addr = 0x22 << 1  // I2C address = 0x44
};

// Set the direction of a specific pin on the PCAL6524 I/O expander //
void PCAL6524_SetPinDirection(PCAL6524 *expander, uint8_t port, uint8_t pin, uint8_t is_output) {
    uint8_t config_reg = PCAL6524_REG_CONFIG_PORT0 + port; // Calculate configuration register address based on port number
    uint8_t config_val;

    //Read the current configuration register value
    HAL_I2C_Mem_Read(expander->hi2c, expander->i2c_addr, config_reg, I2C_MEMADD_SIZE_8BIT, &config_val, 1, 100);

    //Modify the bit corresponding to the pin:
    //    - Clear bit to set as output (0)
    //    - Set bit to configure as input (1)
    if (is_output)
        config_val &= ~(1 << pin);  // Set pin as output by clearing the bit
    else
        config_val |= (1 << pin);   // Set pin as input by setting the bit

    //Write the updated configuration value back to the register
    HAL_I2C_Mem_Write(expander->hi2c, expander->i2c_addr, config_reg, I2C_MEMADD_SIZE_8BIT, &config_val, 1, 100);
}

// Set the direction of an entire Port on the PCAL6524 I/O expander //
void PCAL6524_SetPortDirection_io(PCAL6524 *expander, uint8_t port, uint8_t direction) {
    uint8_t config_reg = PCAL6524_REG_CONFIG_PORT0 + port;
    uint8_t config_val;

    // If direction is INPUT, all bits to 1; if OUTPUT, all bits to 0
    if (direction == INPUT)
        config_val = 0xFF;
    else
        config_val = 0x00;

    HAL_I2C_Mem_Write(expander->hi2c, expander->i2c_addr,config_reg, I2C_MEMADD_SIZE_8BIT,&config_val, 1, 100);
}

// Set output level of a specific pin on the PCAL6524 I/O expander //
void PCAL6524_WritePin(PCAL6524 *expander, uint8_t port, uint8_t pin, uint8_t logic_level) {
    uint8_t config_reg = PCAL6524_REG_OUTPUT_PORT0 + port; // Calculate configuration register address based on port number
    uint8_t config_val;

    //Read the current configuration register value
    HAL_I2C_Mem_Read(expander->hi2c, expander->i2c_addr, config_reg, I2C_MEMADD_SIZE_8BIT, &config_val, 1, 100);

    // Set all pins to HIGH or LOW depending on logic_level
    if (logic_level == HIGH)
    	config_val |= (1 << pin);    // Set bit to 1 → HIGH
    else
    	config_val &= ~(1 << pin);   // Clear bit to 0 → LOW

    //Write the updated configuration value back to the register
    HAL_I2C_Mem_Write(expander->hi2c, expander->i2c_addr, config_reg, I2C_MEMADD_SIZE_8BIT, &config_val, 1, 100);
}

// Set output level of an entire Port on the PCAL6524 I/O expander //
void PCAL6524_WritePort(PCAL6524 *expander, uint8_t port, uint8_t logic_level) {
    uint8_t config_reg = PCAL6524_REG_OUTPUT_PORT0 + port; // Calculate configuration register address based on port number
    uint8_t config_val;

    // Set all pins to HIGH or LOW depending on logic_level
    if (logic_level == HIGH)
    	config_val = 0xFF;   // Set bit to 1 → HIGH
    else
    	config_val = 0x00;   // Clear bit to 0 → LOW

    // Write the value to the output register
    HAL_I2C_Mem_Write(expander->hi2c, expander->i2c_addr, config_reg, I2C_MEMADD_SIZE_8BIT, &config_val, 1, 100);
}

// Read the logic level of a specific pin on the PCAL6524 I/O expander //
uint8_t PCAL6524_ReadPin(PCAL6524 *expander, uint8_t port, uint8_t pin) {
    uint8_t input_reg = PCAL6524_REG_INPUT_PORT0 + port;  // Input register for the selected port
    uint8_t input_val;

    // Read input register
    HAL_I2C_Mem_Read(expander->hi2c, expander->i2c_addr, input_reg, I2C_MEMADD_SIZE_8BIT, &input_val, 1, 100);

    // Extract the bit corresponding to the pin
    return (input_val >> pin) & 0x01;
}

