/*
 * i2c.h
 *
 *  Created on: Nov 19, 2024
 *      Author: dell
 */

#ifndef I2C_H_
#define I2C_H_

#include "main.h"
#include <stdbool.h>
#include "stm32f4xx.h"
#include "uart_log.h"

void I2C1_CMSIS_CONFIG(void);
bool i2c_I2C1_isSlaveAddressExist(uint8_t Addr);

bool i2c_I2C1_masterTransmit_IT(uint8_t Addr, uint8_t *pData, uint8_t len, uint32_t timeout);
bool i2c_I2C1_masterTransmit(uint8_t Addr, uint8_t *pData, uint8_t len, uint32_t timeout);
bool i2c_I2C1_masterReceive(uint8_t Addr, uint8_t *pData, uint8_t len, uint32_t timeout);
bool I2C1_masterReceive(uint8_t Addr, uint8_t *pData, uint8_t len, uint32_t timeout);
void receive_handler();
bool I2C1_masterReceive_IT(uint8_t Addr, uint8_t *pData, uint8_t len, uint32_t timeout);
void error_i2c_handler();
void i2c_flag(void);

bool I2C_WRITE_MEM(uint8_t slave_addr, uint8_t mem_addr, uint8_t *pData, uint8_t mem_size, uint8_t len, uint32_t timeout);
bool I2C_READ_MEM(uint8_t slave_addr, uint16_t mem_addr,uint8_t *pData,  uint8_t mem_size, uint8_t len, uint32_t timeout);
#endif /* I2C_H_ */
