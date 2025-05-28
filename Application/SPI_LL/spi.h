/*
 * spi.h
 *
 *  Created on: Mar 28, 2025
 *      Author: RyanDank
 */

#ifndef SPI_H_
#define SPI_H_

#include "main.h"
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_ll_spi.h"
typedef struct
{
	uint8_t tx_data_spi1[256];
	uint8_t rx_data_spi2[256];
	uint8_t id_slave[4];
	uint8_t len_rxdata;
	uint8_t len_txdata;
}spi_data_handle_t;

bool spi_transmit(uint8_t *pData, uint8_t len, uint32_t timeout);
bool spi_receive(uint8_t *pData, uint8_t len, uint32_t timeout);
bool spi_transmit_receive(uint8_t *txdata, uint8_t *rxdata, uint8_t len, uint32_t timeout);

#endif /* SPI_H_ */
