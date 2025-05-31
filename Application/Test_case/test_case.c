/*
 * test_case.c
 *
 *  Created on: May 7, 2025
 *      Author: RyanDank
 */

#include "test_case.h"

void test_case_1(void)
{
    xTaskCreate(toggle_red_led, "toggle red led", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(toggle_green_led, "toggle green led", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(toggle_blue_led, "toggle blue led", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(Task_trans_data_eeprom, "transmit data to eeprom", configMINIMAL_STACK_SIZE * 4, NULL, 2, NULL);
    xTaskCreate(read_data_eeprom, "read data from eeprom", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL);
}

void test_case_2(void)
{
    xTaskCreate(toggle_red_led, "toggle red led", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(toggle_green_led, "toggle green led", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(toggle_blue_led, "toggle blue led", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(write_data_W25Q32, "write_data_W25Q32", configMINIMAL_STACK_SIZE * 4, NULL, 2, NULL);
    xTaskCreate(read_data_W25Q32, "read_data_W25Q32", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL);
}

void test_case_3(void)
{
    xTaskCreate(Task_trans_data_eeprom, "transmit data to eeprom", configMINIMAL_STACK_SIZE * 4, NULL, 2, NULL);
    xTaskCreate(read_data_eeprom, "read data from eeprom", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL);

    xTaskCreate(write_data_W25Q32, "write_data_W25Q32", configMINIMAL_STACK_SIZE * 4, NULL, 2, NULL);
    xTaskCreate(read_data_W25Q32, "read_data_W25Q32", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL);
}

void test_case_4(void)
{
    xTaskCreate(toggle_red_led, "toggle red led", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(toggle_green_led, "toggle green led", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(toggle_blue_led, "toggle blue led", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}

void test_case_5(void)
{
    xTaskCreate(generate_square_pulse, "suqare pulse", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}
