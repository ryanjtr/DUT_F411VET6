/*
 * W25Q32_lib.h
 *
 *  Created on: Mar 28, 2025
 *      Author: RyanDank
 */

#ifndef W25Q32_EEPROM_LIB_W25Q32_LIB_H_
#define W25Q32_EEPROM_LIB_W25Q32_LIB_H_


#include "spi.h"

#define REAL_W25Q32 0
// if communicate with real W25Q32 serial flash: CPHA=1, DIV16

void W25Q32JV_Master_ReadJEDECID(uint8_t rxdata[4]);
void W25Q32JV_Master_PageProgram(uint32_t Address, uint8_t *pdata, uint16_t size);
void W25Q32JV_Master_ReadData(uint32_t Address, uint8_t *pdata, uint16_t size);
void W25Q32JV_Master_EraseSector(uint32_t Address);
void W25Q32JV_Master_Reset(void);
void W25Q32JV_Master_EraseChip(void);
#endif /* W25Q32_EEPROM_LIB_W25Q32_LIB_H_ */
