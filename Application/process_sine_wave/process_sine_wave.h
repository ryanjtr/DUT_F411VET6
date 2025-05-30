/*
 * process_sine_wave.h
 *
 *  Created on: May 24, 2025
 *      Author: RyanDank
 */

#ifndef PROCESS_SINE_WAVE_PROCESS_SINE_WAVE_H_
#define PROCESS_SINE_WAVE_PROCESS_SINE_WAVE_H_

#include "main.h"
#include "uart_log.h"
#define ADC_SAMPLES 100
#define SINE_FREQ 1000
#define WINDOW_SIZE 12

extern uint16_t adc_data[ADC_SAMPLES];
void Config_DMA_sine_wave_destination();
void start_timer_trigger_read_adc();
void stop_timer_trigger_read_adc();
void moving_average(uint16_t *data);
#endif /* PROCESS_SINE_WAVE_PROCESS_SINE_WAVE_H_ */
