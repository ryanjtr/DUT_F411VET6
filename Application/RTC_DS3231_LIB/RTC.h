/*
 * RTC.h
 *
 *  Created on: Mar 11, 2025
 *      Author: RyanDank
 */

#ifndef RTC_H_
#define RTC_H_

#include "i2c.h"

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date; //date of month
	uint8_t day;//day of week
	uint8_t month;
	uint8_t year;
	uint8_t temp_msb;
	uint8_t temp_lsb;
	uint8_t temperature;

}rtc_handler;

#define DS3231_ADDRESS (0x68)
void DS3231_WRITE(uint8_t mem_addr,uint8_t data);
uint8_t DS3231_READ(uint8_t mem_addr);
void DS3231_SetDateTime(uint8_t day, uint8_t date, uint8_t month, uint8_t year, uint8_t hour, uint8_t min, uint8_t sec);
void DS3231_GetDateTime(uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year, uint8_t *hour, uint8_t *min, uint8_t *sec);
float DS3231_GetTemperature(void);

#endif /* RTC_H_ */
