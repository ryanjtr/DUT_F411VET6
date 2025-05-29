/*
 * DUTtask.h
 *
 *  Created on: Mar 23, 2025
 *      Author: RyanDank
 */

#ifndef DUTTASK_H_
#define DUTTASK_H_

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "RTC.h"

#include "queue.h"
#include "semphr.h"
#include "string.h"
#include "spi.h"
#include "W25Q32_EEPROM_Lib/W25Q32_lib.h"
#include "main.h"
#include "uartstdio.h"

#include "process_sine_wave.h"
extern uint8_t eeprom_mem_address;
extern SemaphoreHandle_t spi_mutex;

void Init_semaphore();
void set_date_time_rtc(void *pvParameters);
void toggle_red_led(void *pvParameters);
void toggle_green_led(void *pvParameters);
void toggle_blue_led(void *pvParameters);
void get_date_time_rtc(void *pvParameters);
void Task_trans_data_eeprom(void *pvParameters);
void read_data_eeprom(void *pvParameters);
void ButtonTask(void *pvParameters);
void read_id_W25Q32(void *pvParameters);
void write_data_W25Q32(void *pvParameters);
void read_data_W25Q32(void *pvParameters);
void write_and_read_data_W25Q32(void *pvParameters);
void generate_square_pulse(void *pvParameters);
#endif /* DUTTASK_H_ */
