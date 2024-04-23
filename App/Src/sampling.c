/*
 * sampling.c
 *
 *  Created on: Apr 22, 2024
 *      Author: Otavio
 */

#include "sampling.h"

// ======== Init ======== //
void samplingInit(Sampling *sampling, ADC_HandleTypeDef hadc)
{
	sampling->hadc = hadc;

	samplingSetSamplingInterval(sampling, DELAY_5_MILISECONDS);
}

// ======== Sampling ============ //
void samplingExecuteAdcRead(Sampling *sampling)
{
	HAL_ADC_Start(&sampling->hadc);
	HAL_ADC_PollForConversion(&sampling->hadc, HAL_MAX_DELAY);
	sampling->adcValue = HAL_ADC_GetValue(&sampling->hadc);
	HAL_ADC_Stop(&sampling->hadc);
}

// ======= Getters and Setters ======== //
uint16_t samplingGetSamplingInterval(Sampling *sampling)
{
	return sampling->samplingInterval;
}

void samplingSetSamplingInterval(Sampling *sampling, uint16_t samplingInterval)
{
	sampling->samplingInterval = samplingInterval;
}

uint16_t samplingGetAdcValue(Sampling *sampling)
{
	return sampling->adcValue;
}

void samplingSetAdcValue(Sampling *sampling, uint16_t adcValue)
{
	sampling->adcValue = adcValue;
}
