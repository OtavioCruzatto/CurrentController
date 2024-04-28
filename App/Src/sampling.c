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
	sampling->adcHysteresisCriticalValue = 3000;
	sampling->adcHysteresisThreshold = 500;
	sampling->currentMagnitudeOrder = LOW_CURRENT;

	samplingSetSamplingInterval(sampling, DELAY_5_MILISECONDS);
}

// ======== Sampling ============ //
void samplingExecuteAdcRead(Sampling *sampling)
{
	HAL_ADC_Start(&sampling->hadc);

	HAL_ADC_PollForConversion(&sampling->hadc, HAL_MAX_DELAY);
	uint16_t adc1In1Value = HAL_ADC_GetValue(&sampling->hadc);

	HAL_ADC_PollForConversion(&sampling->hadc, HAL_MAX_DELAY);
	uint16_t adc1In6Value = HAL_ADC_GetValue(&sampling->hadc);

	HAL_ADC_Stop(&sampling->hadc);

	sampling->adcValueHigh = adc1In1Value;
	sampling->adcValueLow = adc1In6Value;

	if (adc1In6Value >= (sampling->adcHysteresisCriticalValue + sampling->adcHysteresisThreshold))
	{
		sampling->currentMagnitudeOrder = HIGH_CURRENT;
	}
	else if (adc1In6Value <= (sampling->adcHysteresisCriticalValue - sampling->adcHysteresisThreshold))
	{
		sampling->currentMagnitudeOrder = LOW_CURRENT;
	}

	if (sampling->currentMagnitudeOrder == LOW_CURRENT)
	{
		sampling->adcValue = adc1In6Value;
	}
	else
	{
		sampling->adcValue = adc1In1Value;
	}
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

uint8_t samplingGetCurrentMagnitudeOrder(Sampling *sampling)
{
	return sampling->currentMagnitudeOrder;
}

void samplingSetCurrentMagnitudeOrder(Sampling *sampling, uint8_t currentMagnitudeOrder)
{
	sampling->currentMagnitudeOrder = currentMagnitudeOrder;
}
