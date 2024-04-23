/*
 * sampling.h
 *
 *  Created on: Apr 22, 2024
 *      Author: Otavio
 */

#ifndef INC_SAMPLING_H_
#define INC_SAMPLING_H_

#include "stm32f4xx_hal.h"
#include "defs.h"

typedef struct
{
	// ======== Sampling ============ //
	uint16_t samplingInterval;
	uint16_t adcValue;

	// ======== ADC ============ //
	ADC_HandleTypeDef hadc;
} Sampling;

// ======== Init ======== //
void samplingInit(Sampling *sampling, ADC_HandleTypeDef hdac);

// ======== Sampling ============ //
void samplingExecuteAdcRead(Sampling *sampling);

// ======= Getters and Setters ======== //
uint16_t samplingGetSamplingInterval(Sampling *sampling);
void samplingSetSamplingInterval(Sampling *sampling, uint16_t samplingInterval);
uint16_t samplingGetAdcValue(Sampling *sampling);
void samplingSetAdcValue(Sampling *sampling, uint16_t adcValue);

#endif /* INC_SAMPLING_H_ */
