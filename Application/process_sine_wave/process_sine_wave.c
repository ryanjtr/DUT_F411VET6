/*
 * process_sine_wave.c
 *
 *  Created on: May 24, 2025
 *      Author: RyanDank
 */

#include "process_sine_wave.h"

uint16_t adc_data[ADC_SAMPLES];
void Config_DMA_sine_wave_destination()
{
	LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_0);
	NVIC_SetPriority(DMA2_Stream0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 6, 0));
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_0);
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_0, (uint32_t)&ADC1->DR);
	LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_0, (uint32_t)adc_data);
	LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_0, ADC_SAMPLES);
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_0);
}

void start_timer_trigger_read_adc()
{
	LL_TIM_EnableCounter(TIM2);
	LL_TIM_SetCounter(TIM2, 0);
}
void moving_average(uint16_t *data)
{
	uint16_t temp[ADC_SAMPLES]; // Mảng tạm để lưu kết quả lọc
	uint32_t sum;
	int i, j;

	// Xử lý toàn bộ mảng
	for (i = 0; i < ADC_SAMPLES; i++)
	{
		sum = 0;
		int count = 0;

		// Tính tổng các mẫu trong cửa sổ
		for (j = i - WINDOW_SIZE / 2; j <= i + WINDOW_SIZE / 2; j++)
		{
			// Kiểm tra biên
			if (j >= 0 && j < ADC_SAMPLES)
			{
				sum += data[j];
				count++;
			}
		}
		// Tính trung bình và lưu vào mảng tạm
		temp[i] = (uint16_t)(sum / count);
	}

	// Sao chép kết quả từ mảng tạm về mảng gốc
	for (i = 0; i < ADC_SAMPLES; i++)
	{
		data[i] = temp[i];
	}
}
void stop_timer_trigger_read_adc()
{
	LL_TIM_DisableCounter(TIM2);
	uint8_t trans_packet[200];
	uint8_t header_and_freq[3] = {0xAA, (SINE_FREQ >> 8) & 0xFF, SINE_FREQ & 0xFF};
	for (uint8_t j = 0; j < 3; j++)
	{
		while (!LL_USART_IsActiveFlag_TXE(USART1))
			;
		LL_USART_TransmitData8(USART1, header_and_freq[j]);
	}
	// moving_average(adc_data);
	for (uint8_t i = 0; i < 100; i++)
	{
		trans_packet[i * 2] = (adc_data[i] >> 8) & 0xFF; // MSB
		trans_packet[i * 2 + 1] = adc_data[i] & 0xFF;	 // LSB
		while (!LL_USART_IsActiveFlag_TXE(USART1))
			;
		LL_USART_TransmitData8(USART1, (uint8_t)trans_packet[i * 2]);
		while (!LL_USART_IsActiveFlag_TXE(USART1))
			;
		LL_USART_TransmitData8(USART1, (uint8_t)trans_packet[i * 2 + 1]);
	}
}
