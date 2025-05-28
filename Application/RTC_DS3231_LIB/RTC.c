/*
 * RTC.c
 *
 *  Created on: Mar 11, 2025
 *      Author: RyanDank
 */

#include "RTC.h"

void DS3231_WRITE(uint8_t mem_addr,uint8_t data)
{
	I2C_WRITE_MEM(DS3231_ADDRESS, mem_addr, &data, 8, 1, 1000);
}


uint8_t DS3231_READ(uint8_t mem_addr)
{
	uint8_t rx_data;
	I2C_READ_MEM(DS3231_ADDRESS, mem_addr, &rx_data, 8, 1, 1000);
	return rx_data;
}



void DS3231_SetDateTime(uint8_t day, uint8_t date, uint8_t month, uint8_t year, uint8_t hour, uint8_t min, uint8_t sec)
{
	DS3231_WRITE(0x00, ((sec / 10) << 4) | (sec % 10));
	LL_mDelay(10);
	DS3231_WRITE(0x01, ((min / 10) << 4) | (min % 10));
	LL_mDelay(10);
	DS3231_WRITE(0x02, ((hour / 10) << 4) | (hour % 10));
	LL_mDelay(10);
	DS3231_WRITE(0x03, day);
	LL_mDelay(10);
	DS3231_WRITE(0x04, ((date / 10) << 4) | (date % 10));
	LL_mDelay(10);
	DS3231_WRITE(0x05, ((month / 10) << 4) | (month % 10));
	LL_mDelay(10);
	DS3231_WRITE(0x06, ((year / 10) << 4) | (year % 10));
	LL_mDelay(10);



}


void DS3231_GetDateTime(uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year, uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    *sec = DS3231_READ(0x00);
    LL_mDelay(10);
    *min = DS3231_READ(0x01);
    LL_mDelay(10);
    *hour = DS3231_READ(0x02);
    LL_mDelay(10);
    *day = DS3231_READ(0x03);
    LL_mDelay(10);
    *date = DS3231_READ(0x04);
    LL_mDelay(10);
    *month = DS3231_READ(0x05);
    LL_mDelay(10);
    *year = DS3231_READ(0x06);
    LL_mDelay(10);

    // Convert BCD -> Decimal
    *sec = ((*sec >> 4) * 10) + (*sec & 0x0F);
    *min = ((*min >> 4) * 10) + (*min & 0x0F);
    *date = ((*date >> 4) * 10) + (*date & 0x0F);
    *month = ((*month >> 4) * 10) + (*month & 0x0F);
    *year = ((*year >> 4) * 10) + (*year & 0x0F);

    // Handle hour format (12-hour or 24-hour mode)
    if (*hour & 0x40) {  // Check bit 6 (12-hour mode)
        uint8_t is_pm = (*hour & 0x20) ? 1 : 0;  // Check bit 5 (AM/PM)
        *hour = ((*hour & 0x1F) >> 4) * 10 + (*hour & 0x0F);  // Keep lower 5 bits
        if (is_pm && *hour != 12) {
            *hour += 12;  // Convert to 24-hour format if PM and not 12 PM
        }
        if (!is_pm && *hour == 12) {
            *hour = 0;  // Convert 12 AM to 00 hours
        }
    } else {
        *hour = ((*hour >> 4) * 10) + (*hour & 0x0F);  // 24-hour mode, convert BCD to decimal
    }
}




float DS3231_GetTemperature(void)
{
    uint8_t temp_msb = DS3231_READ(0x11);  // interger 8-bit
    uint8_t temp_lsb = DS3231_READ(0x12);  // float point (2-bit MSB)

    float temperature = (float)temp_msb + ((temp_lsb >> 6) * 0.25);

    return temperature;
}
