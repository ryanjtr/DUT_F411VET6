/*
 * DUTtask.c
 *
 *  Created on: Mar 23, 2025
 *      Author: RyanDank
 */

#include "DUTtask.h"

rtc_handler my_rtc;
spi_data_handle_t my_data_spi;

TaskHandle_t xTaskHandle_set_date_time = NULL;
TaskHandle_t xTaskHandle_toggle_red = NULL;
TaskHandle_t xTaskHandle_toggle_green = NULL;
TaskHandle_t xTaskHandle_toggle_blue = NULL;
TaskHandle_t xTaskHandle_get_date_time = NULL;
TaskHandle_t xTaskHandle_trans_data_eeprom = NULL;
TaskHandle_t xTaskHandle_read_data_eeprom = NULL;
TaskHandle_t buttonTaskHandle = NULL;
TaskHandle_t read_id_flash = NULL;

SemaphoreHandle_t xBinarySemaphore;

bool i2c_is_running = false;

void Init_semaphore()
{
	// Tạo binary semaphore
	xBinarySemaphore = xSemaphoreCreateBinary();
	if (xBinarySemaphore == NULL)
	{
		uart_printf("Not enough heap\r\n");
		while (1)
			;
	}
	xSemaphoreGive(xBinarySemaphore);
}
void set_date_time_rtc(void *pvParameters)
{
	while (1)
	{

		if (!LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
		{
			DS3231_SetDateTime(3, 18, 3, 25, 20, 43, 30);
			i2c_flag();
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void get_date_time_rtc(void *pvParameters)
{
	while (1)
	{
		if (!LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
		{
			DS3231_GetDateTime(&my_rtc.day, &my_rtc.date, &my_rtc.month, &my_rtc.year, &my_rtc.hours, &my_rtc.minutes, &my_rtc.seconds);
			uart_printf("Thoi gian: %02d:%02d:%02d Ngay: %02d-%02d-20%02d Thu: %d\r\n", my_rtc.hours, my_rtc.minutes, my_rtc.seconds, my_rtc.date, my_rtc.month, my_rtc.year, my_rtc.day);
			i2c_flag();
		}
		else
		{
			LL_GPIO_SetOutputPin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void generate_square_pulse(void *pvParameters)
{
	while (1)
	{
		// if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
		{
			LL_GPIO_TogglePin(ORANGE_LED_GPIO_Port, ORANGE_LED_Pin);
		}
		// else
		{
			// LL_GPIO_SetOutputPin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
void toggle_red_led(void *pvParameters)
{
	while (1)
	{
		//		if(LL_GPIO_IsInputPinSet(GPIOE, LL_GPIO_PIN_8))
		//		if(!i2c_is_running)
		{
			if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
			{
				//						uart_printf("red\n");
				LL_GPIO_TogglePin(CONTROL_REDLED_SOFTWARE_GPIO_Port, CONTROL_REDLED_SOFTWARE_Pin);
			}
			else
			{
				LL_GPIO_SetOutputPin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
void toggle_green_led(void *pvParameters)
{
	while (1)
	{
		//		if(LL_GPIO_IsInputPinSet(GPIOE, LL_GPIO_PIN_8))
		//		if(!i2c_is_running)
		{
			if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
			{
				//						uart_printf("green\n");
				LL_GPIO_TogglePin(CONTROL_GREENLED_SOFTWARE_GPIO_Port, CONTROL_GREENLED_SOFTWARE_Pin);
			}
			else
			{
				LL_GPIO_SetOutputPin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void toggle_blue_led(void *pvParameters)
{
	while (1)
	{
		//		if(LL_GPIO_IsInputPinSet(GPIOE, LL_GPIO_PIN_8))
		//		if(!i2c_is_running)
		{
			if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
			{
				//			uart_printf("blue\n");
				LL_GPIO_TogglePin(CONTROL_BLUELED_SOFTWARE_GPIO_Port, CONTROL_BLUELED_SOFTWARE_Pin);
			}
			else
			{
				LL_GPIO_SetOutputPin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

char eeprom_tx_data[] = "University of Technology"; // City University of Technology - Faculty of Electrical and Electronic Engineering
uint8_t eeprom_mem_address = 0x00;
uint32_t count_rec = 1;

void Task_trans_data_eeprom(void *pvParameters)
{
	while (1)
	{
		//		if(!LL_GPIO_IsInputPinSet(GPIOE, LL_GPIO_PIN_8))
		{
			if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
			{
				if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
				{
					//				i2c_is_running=true;
					uart_printf("txdata_eeprom: %s\r\n", eeprom_tx_data);
					vTaskDelay(pdMS_TO_TICKS(10));
					//			UARTprintf("txdata: %s\r\n", eeprom_tx_data);
					if (I2C_WRITE_MEM(0x50, eeprom_mem_address, (uint8_t *)eeprom_tx_data, 8, strlen(eeprom_tx_data), 1000)) // Eeprom
					{
						//				LL_GPIO_TogglePin(ORANGE_LED_GPIO_Port, ORANGE_LED_Pin);
						eeprom_mem_address += (strlen(eeprom_tx_data));
					}
					else
					{
						uart_printf("trans fail !!!\r\n");
					}
					//			i2c_is_running=false;
					xSemaphoreGive(xBinarySemaphore);
				}
			}
			else
			{
				LL_GPIO_SetOutputPin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void read_data_eeprom(void *pvParameters)
{
	uint8_t rx_data[256] = {0};
	while (1)
	{
		//		if(!LL_GPIO_IsInputPinSet(GPIOE, LL_GPIO_PIN_8))
		{
			if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
			{

				if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
				{
					//				i2c_is_running=true;
					vTaskDelay(pdMS_TO_TICKS(20));
					if (I2C_READ_MEM(0x50, eeprom_mem_address - strlen(eeprom_tx_data), rx_data, 8, strlen(eeprom_tx_data), 1000))
					{
						//				LL_GPIO_TogglePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
						char result[100] = "";
						for (int i = 0; i < strlen(eeprom_tx_data); i++)
						{
							char temp[4];
							if (rx_data[i] >= 32 && rx_data[i] <= 126) // ASCII printable range
								sprintf(temp, "%c", rx_data[i]);
							else
								sprintf(temp, ".");
							strcat(result, temp);
						}
						uart_printf("rxdata_eeprom[%d][0x%02X-0x%02X]: %s\r\n", count_rec, (uint8_t)(eeprom_mem_address - strlen(eeprom_tx_data)), (uint8_t)(eeprom_mem_address - 1), result);
						count_rec++;
					}
					//			i2c_is_running=false;
					xSemaphoreGive(xBinarySemaphore);
				}
				//			i2c_flag();
			}
			else
			{
				LL_GPIO_SetOutputPin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

extern volatile uint32_t last_press_time;
extern volatile BaseType_t button_pressed;

// void ButtonTask(void *pvParameters)
//{
//	const uint32_t DEBOUNCE_DELAY = pdMS_TO_TICKS(50);
//	uint32_t previous_tick = 0;
//
//	while (1)
//	{
//		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//
//		if (button_pressed)
//		{
//			button_pressed = pdFALSE;
//
//			if ((last_press_time - previous_tick) > DEBOUNCE_DELAY)
//			{
//				previous_tick = last_press_time;
//
//				LL_GPIO_TogglePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin);
//			}
//		}
//	}
// }
uint32_t count = 0;
void read_id_W25Q32(void *pvParameters)
{

	while (1)
	{

		if (!LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
		{

			W25Q32JV_Master_ReadJEDECID(my_data_spi.id_slave);
			++count;
			uart_printf("ID manufacture[%d]:  0x%02X 0x%02X 0x%02X\r\n", count, my_data_spi.id_slave[1], my_data_spi.id_slave[2], my_data_spi.id_slave[3]);
		}

		vTaskDelay(pdMS_TO_TICKS(3000));
	}
}

uint8_t count_tx = 0;
uint32_t serial_flash_adress = 0x000000;
uint32_t count_rec_dataw25q32 = 1;
void write_data_W25Q32(void *pvParameters)
{
	uint8_t tx_data[512];
	while (1)
	{
		if (count_tx % 2 == 0)
		{
			strcpy((char *)tx_data, "The embedded system initializes all peripherals "
									"and begins communication with external sensors to gather environmental data");
		}
		else
		{
			strcpy((char *)tx_data, "System reboot initiated after watchdog timer expired unexpectedly");
		}
		if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
		{
			if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
			{
				uart_printf("Data to write flash: %s\r\n", tx_data);
				vTaskDelay(5);
				my_data_spi.len_txdata = strlen((const char *)tx_data);
				W25Q32JV_Master_PageProgram(serial_flash_adress, tx_data, my_data_spi.len_txdata);
				xSemaphoreGive(xBinarySemaphore);
				++count_tx;
			}
		}
		else
		{
			LL_GPIO_SetOutputPin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
		}
		vTaskDelay(2000);
	}
}

void read_data_W25Q32(void *pvParameters)
{
	while (1)
	{
		if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
		{
			if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
			{
				vTaskDelay(20);
				W25Q32JV_Master_ReadData(serial_flash_adress, my_data_spi.rx_data_spi2, my_data_spi.len_txdata);

				char result[256] = "";
				for (uint32_t i = 0; i < my_data_spi.len_txdata; i++)
				{
					char temp[4];
					if (my_data_spi.rx_data_spi2[i] >= 32 && my_data_spi.rx_data_spi2[i] <= 126) // ASCII printable range
						sprintf(temp, "%c", my_data_spi.rx_data_spi2[i]);
					else
						sprintf(temp, ".");
					strcat(result, temp);
				}
				uart_printf("rxdata_flash[%d][0x%02X-0x%02X]= %s\r\n", count_rec_dataw25q32,
							serial_flash_adress, serial_flash_adress + my_data_spi.len_txdata - 1, result);
				++count_rec_dataw25q32;
				xSemaphoreGive(xBinarySemaphore);
			}
		}
		else
		{
			LL_GPIO_SetOutputPin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void write_and_read_data_W25Q32(void *pvParameters)
{
	uint8_t tx_data[512];
	while (1)
	{
		if (count_tx % 2 == 0)
		{
			strcpy((char *)tx_data, "The embedded system initializes all peripherals "
									"and begins communication with external sensors to gather environmental data");
		}
		else
		{
			strcpy((char *)tx_data, "System reboot initiated after watchdog timer expired unexpectedly");
		}
		if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
		{

			uart_printf("Data to write: %s\r\n", tx_data);
			vTaskDelay(1500);
			my_data_spi.len_txdata = strlen((const char *)tx_data);
			W25Q32JV_Master_PageProgram(serial_flash_adress, tx_data, my_data_spi.len_txdata);
			++count_tx;
		}
		vTaskDelay(1500);
		if (LL_GPIO_IsInputPinSet(TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port, TAKE_TRIGGER_RUN_FROM_HIL_Pin))
		{
			W25Q32JV_Master_ReadData(serial_flash_adress, my_data_spi.rx_data_spi2, my_data_spi.len_txdata);

			char result[256] = "";
			for (uint32_t i = 0; i < my_data_spi.len_txdata; i++)
			{
				char temp[4];
				if (my_data_spi.rx_data_spi2[i] >= 32 && my_data_spi.rx_data_spi2[i] <= 126) // ASCII printable range
					sprintf(temp, "%c", my_data_spi.rx_data_spi2[i]);
				else
					sprintf(temp, ".");
				strcat(result, temp);
			}
			vTaskDelay(pdMS_TO_TICKS(1500));
			uart_printf("rx_data[%d][0x%02X-0x%02X]= %s\r\n", count_rec_dataw25q32,
						eeprom_mem_address, eeprom_mem_address + my_data_spi.len_txdata - 1, result);
			++count_rec_dataw25q32;
		}
		vTaskDelay(1500);
	}
}
